/*
 * lucy_infer.c — the inference core: GGML block decoding, a Qwen3 forward
 * pass with a KV cache, a byte-level greedy tokenizer, greedy generation.
 * Follows ggml (dequantize_row_q4_K / q6_K) and llama.cpp (NeoX RoPE with
 * YaRN) so that it computes what lucyspace/qwen3.py computes.
 */

#include "lucy_infer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace lucy {

// --- block formats --------------------------------------------------------------
static inline float fp16(const uint8_t* p)
{
	uint16_t h = (uint16_t)p[0] | ((uint16_t)p[1] << 8);
	uint32_t sign = (uint32_t)(h & 0x8000) << 16, exp = (h >> 10) & 0x1F, man = h & 0x3FF, f;
	if (exp == 0) {
		if (man == 0) f = sign;
		else {                                    // subnormal
			exp = 127 - 15 + 1;
			while (!(man & 0x400)) { man <<= 1; exp--; }
			f = sign | (exp << 23) | ((man & 0x3FF) << 13);
		}
	} else if (exp == 31) f = sign | 0x7F800000 | (man << 13);
	else f = sign | ((exp + 127 - 15) << 23) | (man << 13);
	float r; std::memcpy(&r, &f, 4); return r;
}

static constexpr size_t QK = 256;

static size_t block_bytes(const std::string& type)
{
	if (type == "Q4_K") return 144;
	if (type == "Q6_K") return 210;
	if (type == "F32") return 4 * QK;
	throw std::runtime_error("lucy: unsupported tensor type " + type);
}

static inline void scale_min_k4(int j, const uint8_t* q, uint8_t* d, uint8_t* m)
{
	if (j < 4) { *d = q[j] & 63; *m = q[j + 4] & 63; }
	else {
		*d = (q[j + 4] & 0xF) | ((q[j - 4] >> 6) << 4);
		*m = (q[j + 4] >> 4) | ((q[j] >> 6) << 4);
	}
}

static void dequant_q4_k(const uint8_t* b, float* y)
{
	const float d = fp16(b), dmin = fp16(b + 2);
	const uint8_t* scales = b + 4;
	const uint8_t* q = b + 16;
	int is = 0;
	for (int j = 0; j < 256; j += 64) {
		uint8_t sc, m;
		scale_min_k4(is, scales, &sc, &m);
		const float d1 = d * sc, m1 = dmin * m;
		scale_min_k4(is + 1, scales, &sc, &m);
		const float d2 = d * sc, m2 = dmin * m;
		for (int l = 0; l < 32; l++) y[l] = d1 * (q[l] & 0xF) - m1;
		for (int l = 0; l < 32; l++) y[32 + l] = d2 * (q[l] >> 4) - m2;
		q += 32; is += 2; y += 64;
	}
}

static void dequant_q6_k(const uint8_t* b, float* y)
{
	const uint8_t* ql = b;
	const uint8_t* qh = b + 128;
	const int8_t* sc = (const int8_t*)(b + 192);
	const float d = fp16(b + 208);
	for (int n = 0; n < 2; n++) {
		for (int l = 0; l < 32; l++) {
			const int is = l / 16;
			const int q1 = (int)((ql[l] & 0xF) | (((qh[l] >> 0) & 3) << 4)) - 32;
			const int q2 = (int)((ql[l + 32] & 0xF) | (((qh[l] >> 2) & 3) << 4)) - 32;
			const int q3 = (int)((ql[l] >> 4) | (((qh[l] >> 4) & 3) << 4)) - 32;
			const int q4 = (int)((ql[l + 32] >> 4) | (((qh[l] >> 6) & 3) << 4)) - 32;
			y[l] = d * sc[is + 0] * q1;
			y[l + 32] = d * sc[is + 2] * q2;
			y[l + 64] = d * sc[is + 4] * q3;
			y[l + 96] = d * sc[is + 6] * q4;
		}
		y += 128; ql += 64; qh += 32; sc += 8;
	}
}

void dequant_row(const std::string& type, const uint8_t* row, size_t cols, float* out)
{
	if (type == "F32") { std::memcpy(out, row, cols * 4); return; }
	const size_t bb = block_bytes(type);
	for (size_t b = 0; b < cols / QK; b++) {
		if (type == "Q4_K") dequant_q4_k(row + b * bb, out + b * QK);
		else dequant_q6_k(row + b * bb, out + b * QK);
	}
}

float dot_row(const std::string& type, const uint8_t* row, size_t cols, const float* x)
{
	float buf[QK];
	float acc = 0;
	if (type == "F32") {
		const float* w = (const float*)row;
		for (size_t i = 0; i < cols; i++) acc += w[i] * x[i];
		return acc;
	}
	const size_t bb = block_bytes(type);
	const bool q4 = (type == "Q4_K");
	for (size_t b = 0; b < cols / QK; b++) {
		if (q4) dequant_q4_k(row + b * bb, buf);
		else dequant_q6_k(row + b * bb, buf);
		const float* xb = x + b * QK;
		float s = 0;
		for (size_t i = 0; i < QK; i++) s += buf[i] * xb[i];
		acc += s;
	}
	return acc;
}

// --- tensors -----------------------------------------------------------------------
const uint8_t* Tensor::row(size_t r) const
{
	for (const Part& p : parts)
		if (r >= p.row0 && r < p.row1) return p.base + (r - p.row0) * row_bytes;
	throw std::out_of_range("lucy: row out of range");
}

const float* Tensor::f32() const { return (const float*)parts.at(0).base; }

// y[r] = W[r] · x for every row, in parallel
static void matvec(const Tensor& W, const float* x, float* y)
{
	const long rows = (long)W.rows;
	const bool single = W.parts.size() == 1;
	const uint8_t* base = W.parts[0].base;
#pragma omp parallel for schedule(static)
	for (long r = 0; r < rows; r++)
		y[r] = dot_row(W.type, single ? base + (size_t)r * W.row_bytes : W.row(r), W.cols, x);
}

static void rms_norm(const float* x, const float* w, float* out, int n, double eps)
{
	double ss = 0;
	for (int i = 0; i < n; i++) ss += (double)x[i] * x[i];
	const float inv = (float)(1.0 / std::sqrt(ss / n + eps));
	for (int i = 0; i < n; i++) out[i] = x[i] * inv * w[i];
}

// --- the model -------------------------------------------------------------------------
Model::Model(const LucySpec& spec) : _spec(spec)
{
	for (const auto& kv : spec.tensors) map_tensor(kv.second);
	for (size_t i = 0; i < spec.vocab_raw.size(); i++) {
		_raw_to_id.emplace(spec.vocab_raw[i], (int)i);
		_max_raw_len = std::max(_max_raw_len, spec.vocab_raw[i].size());
		const std::string& r = spec.vocab_raw[i];
		if (r == "<|endoftext|>" || r == "<|im_end|>") _stop.push_back((int)i);
	}
	// RoPE frequencies, with llama.cpp's YaRN ramp when configured
	const int hd = spec.head_dim;
	_inv_freq.resize(hd / 2);
	_ramp.assign(hd / 2, 1.0);
	for (int i = 0; i < hd / 2; i++) _inv_freq[i] = std::pow(spec.rope_base, -(2.0 * i) / hd);
	if (spec.yarn_factor > 0) {
		auto corr = [&](double n_rot) {
			return hd * std::log(spec.yarn_orig_ctx / (n_rot * 2 * M_PI)) / (2 * std::log(spec.rope_base));
		};
		const double low = std::max(0.0, std::floor(corr(32))), high = std::min(hd - 1.0, std::ceil(corr(1)));
		for (int i = 0; i < hd / 2; i++)
			_ramp[i] = 1 - std::min(1.0, std::max(0.0, (i - low) / std::max(0.001, high - low)));
		_mscale = 1 + 0.1 * std::log(spec.yarn_factor);
	}
	_logits.resize(T("token_embd.weight").rows);
	reset();
}

Model::~Model()
{
	for (auto& m : _maps) munmap(m.first, m.second);
}

void Model::map_tensor(const TensorSpec& ts)
{
	Tensor t;
	t.type = ts.type;
	t.cols = ts.shape.at(0);
	t.rows = 1;
	for (size_t i = 1; i < ts.shape.size(); i++) t.rows *= ts.shape[i];
	t.row_bytes = (ts.type == "F32") ? t.cols * 4 : t.cols / QK * block_bytes(ts.type);
	for (const TensorPart& p : ts.parts) {
		const std::string path = _spec.folder + "/" + p.file;
		int fd = open(path.c_str(), O_RDONLY);
		if (fd < 0) throw std::runtime_error("lucy: cannot open " + path);
		struct stat st;
		fstat(fd, &st);
		void* m = mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
		close(fd);
		if (m == MAP_FAILED) throw std::runtime_error("lucy: cannot map " + path);
		_maps.emplace_back(m, st.st_size);
		const size_t r0 = (p.row1 > p.row0) ? p.row0 : 0, r1 = (p.row1 > p.row0) ? p.row1 : t.rows;
		t.parts.push_back({(const uint8_t*)m, r0, r1});
	}
	_t.emplace(ts.name, std::move(t));
}

const Tensor& Model::T(const std::string& name) const
{
	auto it = _t.find(name);
	if (it == _t.end()) throw std::runtime_error("lucy: no tensor " + name);
	return it->second;
}

const Tensor& Model::L(int layer, const char* what) const
{
	return T("blk." + std::to_string(layer) + "." + what + ".weight");
}

void Model::reset() { _ctx = 0; _kcache.assign(_spec.layers, {}); _vcache.assign(_spec.layers, {}); }

void Model::ensure_ctx(int n)
{
	const size_t per = (size_t)_spec.kv_heads * _spec.head_dim;
	for (int l = 0; l < _spec.layers; l++) {
		if (_kcache[l].size() < n * per) { _kcache[l].resize(n * per); _vcache[l].resize(n * per); }
	}
}

const std::vector<float>& Model::step(int token, int pos)
{
	const int d = _spec.d, H = _spec.heads, K = _spec.kv_heads, hd = _spec.head_dim, F = _spec.ffn;
	const int group = H / K, half = hd / 2;
	ensure_ctx(pos + 1);
	std::vector<float> x(d), n(d), q(H * hd), k(K * hd), v(K * hd), att(H * hd), o(d), g(F), u(F), sc(pos + 1);
	const Tensor& E = T("token_embd.weight");
	dequant_row(E.type, E.row(token), d, x.data());

	std::vector<float> cs(half), sn(half);
	for (int i = 0; i < half; i++) {
		const double th = pos * _inv_freq[i];
		const double t = (th / (_spec.yarn_factor > 0 ? _spec.yarn_factor : 1.0)) * (1 - _ramp[i]) + th * _ramp[i];
		cs[i] = (float)(std::cos(t) * _mscale);
		sn[i] = (float)(std::sin(t) * _mscale);
	}
	auto rope = [&](float* h) {
		for (int i = 0; i < half; i++) {
			const float a = h[i], b = h[i + half];
			h[i] = a * cs[i] - b * sn[i];
			h[i + half] = a * sn[i] + b * cs[i];
		}
	};

	for (int l = 0; l < _spec.layers; l++) {
		rms_norm(x.data(), L(l, "attn_norm").f32(), n.data(), d, _spec.eps);
		matvec(L(l, "attn_q"), n.data(), q.data());
		matvec(L(l, "attn_k"), n.data(), k.data());
		matvec(L(l, "attn_v"), n.data(), v.data());
		const float* qn = L(l, "attn_q_norm").f32();
		const float* kn = L(l, "attn_k_norm").f32();
		for (int h = 0; h < H; h++) { rms_norm(&q[h * hd], qn, &q[h * hd], hd, _spec.eps); rope(&q[h * hd]); }
		for (int h = 0; h < K; h++) { rms_norm(&k[h * hd], kn, &k[h * hd], hd, _spec.eps); rope(&k[h * hd]); }
		std::memcpy(&_kcache[l][(size_t)pos * K * hd], k.data(), K * hd * 4);
		std::memcpy(&_vcache[l][(size_t)pos * K * hd], v.data(), K * hd * 4);
		const float scale = 1.0f / std::sqrt((float)hd);
		for (int h = 0; h < H; h++) {
			const int j = h / group;
			const float* qh = &q[h * hd];
			float mx = -INFINITY;
			for (int t = 0; t <= pos; t++) {
				const float* kt = &_kcache[l][((size_t)t * K + j) * hd];
				float s = 0;
				for (int i = 0; i < hd; i++) s += qh[i] * kt[i];
				sc[t] = s * scale;
				mx = std::max(mx, sc[t]);
			}
			float sum = 0;
			for (int t = 0; t <= pos; t++) { sc[t] = std::exp(sc[t] - mx); sum += sc[t]; }
			float* out = &att[h * hd];
			std::fill(out, out + hd, 0.0f);
			for (int t = 0; t <= pos; t++) {
				const float w = sc[t] / sum;
				const float* vt = &_vcache[l][((size_t)t * K + j) * hd];
				for (int i = 0; i < hd; i++) out[i] += w * vt[i];
			}
		}
		matvec(L(l, "attn_output"), att.data(), o.data());
		for (int i = 0; i < d; i++) x[i] += o[i];
		rms_norm(x.data(), L(l, "ffn_norm").f32(), n.data(), d, _spec.eps);
		matvec(L(l, "ffn_gate"), n.data(), g.data());
		matvec(L(l, "ffn_up"), n.data(), u.data());
		for (int i = 0; i < F; i++) g[i] = g[i] / (1.0f + std::exp(-g[i])) * u[i];
		if (on_ffn) on_ffn(l, g.data(), F);
		matvec(L(l, "ffn_down"), g.data(), o.data());
		for (int i = 0; i < d; i++) x[i] += o[i];
	}
	rms_norm(x.data(), T("output_norm.weight").f32(), n.data(), d, _spec.eps);
	const auto out = _t.find("output.weight");
	matvec(out != _t.end() ? out->second : E, n.data(), _logits.data());
	_ctx = pos + 1;
	return _logits;
}

std::vector<float> Model::ffn(int l, const std::vector<float>& n) const
{
	const int d = _spec.d, F = _spec.ffn;
	std::vector<float> g(F), u(F), o(d);
	matvec(L(l, "ffn_gate"), n.data(), g.data());
	matvec(L(l, "ffn_up"), n.data(), u.data());
	for (int i = 0; i < F; i++) g[i] = g[i] / (1.0f + std::exp(-g[i])) * u[i];
	matvec(L(l, "ffn_down"), g.data(), o.data());
	return o;
}

std::vector<float> Model::decode(const std::string& name, size_t& rows, size_t& cols) const
{
	const Tensor& t = T(name);
	rows = t.rows; cols = t.cols;
	std::vector<float> out(rows * cols);
#pragma omp parallel for schedule(static)
	for (long r = 0; r < (long)rows; r++) dequant_row(t.type, t.row(r), cols, &out[r * cols]);
	return out;
}

// --- tokens ---------------------------------------------------------------------------------
// GPT-2 byte-level BPE (Qwen's): each byte has a printable stand-in character.
static const std::vector<std::string>& byte_chars()
{
	static std::vector<std::string> table = [] {
		std::vector<int> bs;
		for (int b = '!'; b <= '~'; b++) bs.push_back(b);
		for (int b = 0xA1; b <= 0xAC; b++) bs.push_back(b);
		for (int b = 0xAE; b <= 0xFF; b++) bs.push_back(b);
		std::vector<int> cs(bs);
		int n = 0;
		for (int b = 0; b < 256; b++)
			if (std::find(bs.begin(), bs.end(), b) == bs.end()) { bs.push_back(b); cs.push_back(256 + n++); }
		std::vector<std::string> t(256);
		for (size_t i = 0; i < bs.size(); i++) {
			const int c = cs[i];
			std::string s;
			if (c < 0x80) s += (char)c;
			else { s += (char)(0xC0 | (c >> 6)); s += (char)(0x80 | (c & 0x3F)); }
			t[bs[i]] = s;
		}
		return t;
	}();
	return table;
}

std::vector<int> Model::tokenize(const std::string& text) const
{
	// The text as GPT-2 byte characters, then greedy longest match on the
	// raw vocabulary. Not BPE's merge order, but every result is valid
	// and every byte is covered.
	std::string raw;
	std::vector<size_t> starts;                 // byte offsets in raw where a text byte begins
	for (unsigned char c : text) { starts.push_back(raw.size()); raw += byte_chars()[c]; }
	starts.push_back(raw.size());
	std::vector<int> ids;
	size_t i = 0;                                // index into starts
	while (i + 1 < starts.size()) {
		size_t best = 0; int best_id = -1;
		for (size_t j = i + 1; j < starts.size(); j++) {
			const size_t len = starts[j] - starts[i];
			if (len > _max_raw_len) break;
			auto it = _raw_to_id.find(raw.substr(starts[i], len));
			if (it != _raw_to_id.end()) { best = j; best_id = it->second; }
		}
		if (best_id < 0) throw std::runtime_error("lucy: no token for byte " + std::to_string((unsigned char)text[i]));
		ids.push_back(best_id);
		i = best;
	}
	return ids;
}

std::string Model::detokenize(const std::vector<int>& ids) const
{
	static std::unordered_map<std::string, unsigned char> inverse = [] {
		std::unordered_map<std::string, unsigned char> m;
		for (int b = 0; b < 256; b++) m[byte_chars()[b]] = (unsigned char)b;
		return m;
	}();
	std::string out;
	for (int id : ids) {
		const std::string& r = _spec.vocab_raw.at(id);
		if (_spec.vocab_kind.at(id) != "normal") { out += r; continue; }
		for (size_t i = 0; i < r.size();) {
			const size_t len = ((unsigned char)r[i] < 0x80) ? 1 : 2;
			auto it = inverse.find(r.substr(i, len));
			if (it != inverse.end()) out += (char)it->second;
			i += len;
		}
	}
	return out;
}

GenerateResult Model::generate(const std::string& prompt, int n)
{
	using clk = std::chrono::steady_clock;
	GenerateResult r;
	r.prompt_ids = tokenize(prompt);
	reset();
	auto t0 = clk::now();
	int pos = 0;
	const std::vector<float>* logits = nullptr;
	for (int id : r.prompt_ids) logits = &step(id, pos++);
	auto t1 = clk::now();
	for (int i = 0; i < n && logits; i++) {
		const int next = (int)(std::max_element(logits->begin(), logits->end()) - logits->begin());
		if (std::find(_stop.begin(), _stop.end(), next) != _stop.end()) break;
		r.ids.push_back(next);
		logits = &step(next, pos++);
	}
	auto t2 = clk::now();
	r.prompt_seconds = std::chrono::duration<double>(t1 - t0).count();
	r.gen_seconds = std::chrono::duration<double>(t2 - t1).count();
	r.text = detokenize(r.ids);
	return r;
}

} // namespace lucy
