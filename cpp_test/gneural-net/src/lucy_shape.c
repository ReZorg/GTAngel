/*
 * lucy_shape.c — see lucy_shape.h. Reads tensors only through the Model,
 * which is built from the lucyspace atoms; reads and writes neurons, links
 * and values only through the AtomSpace.
 */

#include <algorithm>
#include <array>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <cmath>
#include <cstdio>
#include <numeric>
#include <random>
#include <stdexcept>
#include <unordered_map>

#include <opencog/atoms/atom_types/atom_types.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/truthvalue/TruthValue.h>
#include <opencog/atoms/value/FloatValue.h>

#include "lucy_shape.h"

using namespace opencog;

namespace lucy {

static Handle neuron(AtomSpace* as, int L, int i)
{
	char b[64];
	std::snprintf(b, sizeof b, "lucy/L%02d/ffn/n%d", L, i);
	return as->get_node(CONCEPT_NODE, b);
}

static bool parse_neuron(const std::string& s, int& L, int& i)
{
	return std::sscanf(s.c_str(), "lucy/L%d/ffn/n%d", &L, &i) == 2;
}

static void normalize_rows(std::vector<float>& a, size_t rows, size_t cols)
{
#pragma omp parallel for schedule(static)
	for (long r = 0; r < (long)rows; r++) {
		double s = 0;
		for (size_t k = 0; k < cols; k++) s += (double)a[r * cols + k] * a[r * cols + k];
		const float inv = (float)(1.0 / (std::sqrt(s) + 1e-12));
		for (size_t k = 0; k < cols; k++) a[r * cols + k] *= inv;
	}
}

// C = R . D^T for R (n x d), D (m x d), both row-major; calls hit(j, i, c)
// for every |c| >= thr. Tiled for cache, 4 x 4 register blocks.
template <class Hit>
static void gram(const float* R, size_t n, const float* D, size_t m, size_t d, float thr, Hit hit)
{
	const size_t TJ = 64, TI = 64;
#pragma omp parallel for schedule(dynamic)
	for (long jt = 0; jt < (long)n; jt += TJ)
		for (size_t it = 0; it < m; it += TI)
			for (size_t j = jt; j < std::min(n, (size_t)jt + TJ); j += 4)
				for (size_t i = it; i < std::min(m, it + TI); i += 4) {
					const size_t nj = std::min<size_t>(4, n - j), ni = std::min<size_t>(4, m - i);
					float s[4][4] = {};
					const float* r[4]; const float* q[4];
					for (size_t a = 0; a < 4; a++) r[a] = R + std::min(j + a, n - 1) * d;
					for (size_t b = 0; b < 4; b++) q[b] = D + std::min(i + b, m - 1) * d;
					for (size_t k = 0; k < d; k++) {
						const float r0 = r[0][k], r1 = r[1][k], r2 = r[2][k], r3 = r[3][k];
						const float q0 = q[0][k], q1 = q[1][k], q2 = q[2][k], q3 = q[3][k];
						s[0][0] += r0 * q0; s[0][1] += r0 * q1; s[0][2] += r0 * q2; s[0][3] += r0 * q3;
						s[1][0] += r1 * q0; s[1][1] += r1 * q1; s[1][2] += r1 * q2; s[1][3] += r1 * q3;
						s[2][0] += r2 * q0; s[2][1] += r2 * q1; s[2][2] += r2 * q2; s[2][3] += r2 * q3;
						s[3][0] += r3 * q0; s[3][1] += r3 * q1; s[3][2] += r3 * q2; s[3][3] += r3 * q3;
					}
					for (size_t a = 0; a < nj; a++)
						for (size_t b = 0; b < ni; b++)
							if (std::fabs(s[a][b]) >= thr) hit(j + a, i + b, s[a][b]);
				}
}

long direct_links(AtomSpace* as, Model& m, double zmin, int horizon)
{
	const int layers = m.spec().layers;
	const size_t d = m.spec().d, F = m.spec().ffn;
	const float thr = (float)(zmin / std::sqrt((double)d));
	// writers: down columns as rows (F x d), unit length
	std::vector<std::vector<float>> down(layers), rgate(layers), rup(layers);
	for (int l = 0; l < layers; l++) {
		const std::string blk = "blk." + std::to_string(l) + ".";
		size_t r, c;
		std::vector<float> dn = m.decode(blk + "ffn_down.weight", r, c);       // d x F
		if (r != d || c != F) throw std::runtime_error("lucy: unexpected ffn_down shape");
		down[l].resize(F * d);
		for (size_t i = 0; i < F; i++)
			for (size_t k = 0; k < d; k++) down[l][i * d + k] = dn[k * F + i];
		normalize_rows(down[l], F, d);
		std::vector<float> nrm = m.decode(blk + "ffn_norm.weight", r, c);
		if (r * c != d) throw std::runtime_error("lucy: unexpected ffn_norm shape");
		for (int p = 0; p < 2; p++) {
			std::vector<float> w = m.decode(blk + (p ? "ffn_up.weight" : "ffn_gate.weight"), r, c);  // F x d
			for (size_t j = 0; j < F; j++)
				for (size_t k = 0; k < d; k++) w[j * d + k] *= nrm[k];
			normalize_rows(w, F, d);
			(p ? rup : rgate)[l] = std::move(w);
		}
	}
	Handle kgate = as->add_node(PREDICATE_NODE, "lucy:gate");
	Handle kup = as->add_node(PREDICATE_NODE, "lucy:up");
	Handle kspan = as->add_node(PREDICATE_NODE, "lucy:span");
	ValuePtr prior = createTruthValue(1.0 / F, 0.9);
	const double sd = std::sqrt((double)d);
	long count = 0;
	for (int L = 0; L < layers; L++)
		for (int M = L + 1; M < std::min(layers, L + 1 + horizon); M++)
			for (int p = 0; p < 2; p++) {
				struct H { uint32_t j, i; float c; };
				std::vector<std::vector<H>> per_thread(64);
				gram((p ? rup : rgate)[M].data(), F, down[L].data(), F, d, thr,
				     [&](size_t j, size_t i, float c) {
					     int t = 0;
#ifdef _OPENMP
					     t = omp_get_thread_num() % 64;
#endif
					     per_thread[t].push_back({(uint32_t)j, (uint32_t)i, c});
				     });
				for (auto& v : per_thread)
					for (const H& h : v) {
						Handle a = neuron(as, L, h.i), b = neuron(as, M, h.j);
						if (!a || !b) continue;
						Handle link = as->add_link(INHERITANCE_LINK, a, b);
						const double z = std::fabs(h.c) * sd, n = z * z, conf = n / (n + 800.0);
						ValuePtr tv = link->getValue(truth_key());
						if (!tv || TruthValueCast(tv)->get_confidence() < conf)
							as->set_value(link, truth_key(), createTruthValue(std::fabs(h.c), conf));
						as->set_value(link, p ? kup : kgate, createFloatValue(std::vector<double>{h.c}));
						as->set_value(link, kspan, createFloatValue(std::vector<double>{(double)(M - L)}));
						as->set_value(a, truth_key(), prior);
						as->set_value(b, truth_key(), prior);
						count++;
					}
				std::fprintf(stderr, "lucy-direct-links: L%02d -> L%02d %s: %ld links so far\n",
				             L, M, p ? "up" : "gate", count);
			}
	return count;
}

std::vector<int> probe(AtomSpace* as, Model& m, const std::string& label,
                       const std::vector<std::string>& questions)
{
	const int layers = m.spec().layers, F = m.spec().ffn;
	Handle key = as->add_node(PREDICATE_NODE, "lucy:act/" + label);
	const std::string pre = "<|im_start|>user\n";
	const std::string post = " /no_think<|im_end|>\n<|im_start|>assistant\n";
	std::vector<std::vector<double>> acc((size_t)layers * F);   // per neuron, one entry per question
	std::vector<int> counts;
	for (const std::string& q : questions) {
		const std::vector<int> a = m.tokenize(pre), b = m.tokenize(q), c = m.tokenize(post);
		std::vector<int> ids = a;
		ids.insert(ids.end(), b.begin(), b.end());
		ids.insert(ids.end(), c.begin(), c.end());
		const int q0 = (int)a.size(), q1 = q0 + (int)b.size();
		std::vector<double> sum((size_t)layers * F, 0.0);
		int pos_now = 0;
		m.on_ffn = [&](int l, const float* h, int n) {
			if (pos_now < q0 || pos_now >= q1) return;
			double* s = &sum[(size_t)l * F];
			for (int j = 0; j < n; j++) s[j] += std::fabs(h[j]);
		};
		m.reset();
		for (int pos = 0; pos < (int)ids.size(); pos++) { pos_now = pos; m.step(ids[pos], pos); }
		m.on_ffn = nullptr;
		for (size_t k = 0; k < sum.size(); k++) acc[k].push_back(sum[k] / b.size());
		counts.push_back((int)b.size());
		std::fprintf(stderr, "lucy-probe %s: %zu question tokens (%zu total): %s\n",
		             label.c_str(), b.size(), ids.size(), q.c_str());
	}
	for (int l = 0; l < layers; l++)
		for (int j = 0; j < F; j++) {
			Handle h = neuron(as, l, j);
			if (h) as->set_value(h, key, createFloatValue(std::move(acc[(size_t)l * F + j])));
		}
	return counts;
}

// ---------------------------------------------------------------- motifs

namespace {
struct Edge { int src, dst, span, port; int sign; };
inline uint64_t ekey(int a, int b, int port) { return ((uint64_t)a << 33) | ((uint64_t)b << 1) | (uint64_t)port; }
}

std::vector<double> motifs(AtomSpace* as, int K, unsigned seed, int restarts)
{
	Handle kgate = as->get_node(PREDICATE_NODE, "lucy:gate");
	Handle kup = as->get_node(PREDICATE_NODE, "lucy:up");
	Handle kspan = as->get_node(PREDICATE_NODE, "lucy:span");
	if (!kspan) throw std::runtime_error("lucy-motifs: no direct links (run lucy-direct-links! first)");
	int layers = 0, F = 0;
	std::vector<Edge> edges;
	HandleSeq links;
	as->get_handles_by_type(links, INHERITANCE_LINK);
	for (const Handle& l : links) {
		if (!l->getValue(kspan)) continue;
		int La, ia, Lb, ib;
		if (!parse_neuron(l->getOutgoingAtom(0)->get_name(), La, ia) ||
		    !parse_neuron(l->getOutgoingAtom(1)->get_name(), Lb, ib)) continue;
		layers = std::max(layers, Lb + 1);
		F = std::max({F, ia + 1, ib + 1});
		for (int p = 0; p < 2; p++) {
			FloatValuePtr v = FloatValueCast(l->getValue(p ? kup : kgate));
			if (!v) continue;
			edges.push_back({0, 0, Lb - La, p, v->value()[0] > 0 ? 1 : -1});
			edges.back().src = La * 8192 + ia;     // ids with room for F <= 8192
			edges.back().dst = Lb * 8192 + ib;
		}
	}
	const int ID = layers * 8192;
	std::unordered_map<uint64_t, int> sign_of;     // (src, dst, port) -> sign
	sign_of.reserve(edges.size() * 2);
	std::vector<std::vector<int>> out1(ID);         // span-1 edges by source
	for (int e = 0; e < (int)edges.size(); e++) {
		sign_of[ekey(edges[e].src, edges[e].dst, edges[e].port)] = edges[e].sign;
		if (edges[e].span == 1) out1[edges[e].src].push_back(e);
	}
	// span-2 densities per (source layer, port)
	std::vector<double> dens2(layers * 2, 0.0);
	for (const Edge& e : edges) if (e.span == 2) dens2[(e.src / 8192) * 2 + e.port] += 1.0 / ((double)F * F);
	// census and per-neuron features
	double loops[8] = {}, coh[8] = {}, expd[8] = {};
	std::vector<std::array<double, 12>> feat(ID);
	for (auto& f : feat) f.fill(0.0);
	for (int A = 0; A < ID; A++)
		for (int e1 : out1[A]) {
			const Edge& ab = edges[e1];
			for (int e2 : out1[ab.dst]) {
				const Edge& bc = edges[e2];
				for (int p3 = 0; p3 < 2; p3++) {
					const int cls = ab.port * 4 + bc.port * 2 + p3;
					expd[cls] += dens2[(A / 8192) * 2 + p3];
					auto it = sign_of.find(ekey(A, bc.dst, p3));
					if (it == sign_of.end()) continue;
					// the skip A -> C must be a span-2 edge
					loops[cls] += 1;
					const int sigma = ab.sign * bc.sign * it->second;
					if (sigma > 0) coh[cls] += 1;
					const bool inv = ab.port == 1 && bc.port == p3;
					if (inv) {
						const int k = sigma > 0 ? 0 : 1;
						feat[A][0 + k] += 1; feat[ab.dst][2 + k] += 1; feat[bc.dst][4 + k] += 1;
					}
				}
			}
		}
	for (const Edge& e : edges) {
		feat[e.dst][6 + (e.span - 1) * 2 + e.port] += 1;     // in by span and port
		feat[e.src][10 + (e.span - 1)] += 1;                 // out by span
	}
	// roles: k-means on log1p-standardized features of linked neurons
	std::vector<int> ids;
	for (int n = 0; n < ID; n++) {
		double s = 0; for (double x : feat[n]) s += x;
		if (s > 0) ids.push_back(n);
	}
	const size_t N = ids.size(), D = 12;
	std::vector<double> X(N * D), mu(D, 0.0), sdv(D, 0.0);
	for (size_t r = 0; r < N; r++) for (size_t k = 0; k < D; k++) X[r * D + k] = std::log1p(feat[ids[r]][k]);
	for (size_t r = 0; r < N; r++) for (size_t k = 0; k < D; k++) mu[k] += X[r * D + k] / N;
	for (size_t r = 0; r < N; r++) for (size_t k = 0; k < D; k++) sdv[k] += std::pow(X[r * D + k] - mu[k], 2) / N;
	for (size_t k = 0; k < D; k++) sdv[k] = std::sqrt(sdv[k]) + 1e-9;
	for (size_t r = 0; r < N; r++) for (size_t k = 0; k < D; k++) X[r * D + k] = (X[r * D + k] - mu[k]) / sdv[k];
	std::mt19937_64 rng(seed);
	std::vector<int> best_lab(N);
	double best_sse = 1e300;
	for (int rs = 0; rs < restarts; rs++) {
		// k-means++ initialization
		std::vector<double> C(K * D), dist(N, 1e300);
		size_t first = std::uniform_int_distribution<size_t>(0, N - 1)(rng);
		std::copy(&X[first * D], &X[first * D] + D, &C[0]);
		for (int c = 1; c < K; c++) {
			double tot = 0;
			for (size_t r = 0; r < N; r++) {
				double s = 0; for (size_t k = 0; k < D; k++) s += std::pow(X[r * D + k] - C[(c - 1) * D + k], 2);
				dist[r] = std::min(dist[r], s); tot += dist[r];
			}
			double u = std::uniform_real_distribution<double>(0, tot)(rng);
			size_t pick = 0;
			for (; pick + 1 < N && (u -= dist[pick]) > 0; pick++) {}
			std::copy(&X[pick * D], &X[pick * D] + D, &C[c * D]);
		}
		std::vector<int> lab(N, 0);
		double sse = 0;
		for (int iter = 0; iter < 100; iter++) {
			sse = 0;
			bool changed = false;
#pragma omp parallel for reduction(+:sse) reduction(||:changed)
			for (long r = 0; r < (long)N; r++) {
				int bc = 0; double bd = 1e300;
				for (int c = 0; c < K; c++) {
					double s = 0; for (size_t k = 0; k < D; k++) s += std::pow(X[r * D + k] - C[c * D + k], 2);
					if (s < bd) { bd = s; bc = c; }
				}
				if (lab[r] != bc) { lab[r] = bc; changed = true; }
				sse += bd;
			}
			std::vector<double> S(K * D, 0.0); std::vector<long> cnt(K, 0);
			for (size_t r = 0; r < N; r++) { cnt[lab[r]]++; for (size_t k = 0; k < D; k++) S[lab[r] * D + k] += X[r * D + k]; }
			for (int c = 0; c < K; c++) if (cnt[c]) for (size_t k = 0; k < D; k++) C[c * D + k] = S[c * D + k] / cnt[c];
			if (!changed && iter > 0) break;
		}
		std::fprintf(stderr, "lucy-motifs: k-means restart %d, SSE per neuron %.4f\n", rs, sse / N);
		if (sse < best_sse) { best_sse = sse; best_lab = lab; }
	}
	Handle kfeat = as->add_node(PREDICATE_NODE, "lucy:motif-features");
	std::vector<Handle> roles(K);
	for (int c = 0; c < K; c++) roles[c] = as->add_node(CONCEPT_NODE, "lucy:Role/" + std::to_string(c));
	for (size_t r = 0; r < N; r++) {
		Handle h = neuron(as, ids[r] / 8192, ids[r] % 8192);
		if (!h) continue;
		as->set_value(h, kfeat, createFloatValue(std::vector<double>(feat[ids[r]].begin(), feat[ids[r]].end())));
		as->add_link(MEMBER_LINK, h, roles[best_lab[r]]);
	}
	std::vector<double> out;
	for (int c = 0; c < 8; c++) { out.push_back(loops[c]); out.push_back(coh[c]); out.push_back(expd[c]); }
	out.push_back((double)N); out.push_back(best_sse / N);
	return out;
}

// ---------------------------------------------------------------- separation

std::vector<double> separation(AtomSpace* as, const std::string& a, const std::string& b,
                               int permutations, unsigned seed)
{
	Handle ka = as->get_node(PREDICATE_NODE, "lucy:act/" + a);
	Handle kb = as->get_node(PREDICATE_NODE, "lucy:act/" + b);
	if (!ka || !kb) throw std::runtime_error("lucy-separation: run lucy-probe! for both labels first");
	Handle kt = as->add_node(PREDICATE_NODE, "lucy:t/" + a + "-" + b);
	HandleSeq hs;
	as->get_handles_by_type(hs, CONCEPT_NODE);
	std::vector<Handle> neurons;
	std::vector<std::vector<double>> rows;
	size_t na = 0, nb = 0;
	for (const Handle& h : hs) {
		FloatValuePtr va = FloatValueCast(h->getValue(ka)), vb = FloatValueCast(h->getValue(kb));
		if (!va || !vb) continue;
		std::vector<double> r = va->value();
		na = r.size(); nb = vb->value().size();
		r.insert(r.end(), vb->value().begin(), vb->value().end());
		neurons.push_back(h); rows.push_back(std::move(r));
	}
	auto welch = [&](const std::vector<double>& r, const std::vector<int>& lab) {
		double sa = 0, sb = 0, qa = 0, qb = 0; size_t ca = 0, cb = 0;
		for (size_t k = 0; k < r.size(); k++) (lab[k] ? (sa += r[k], ca++) : (sb += r[k], cb++));
		const double ma = sa / ca, mb = sb / cb;
		for (size_t k = 0; k < r.size(); k++) (lab[k] ? qa : qb) += std::pow(r[k] - (lab[k] ? ma : mb), 2);
		return (ma - mb) / std::sqrt(qa / (ca - 1) / ca + qb / (cb - 1) / cb + 1e-12);
	};
	std::vector<int> lab(na + nb, 0);
	for (size_t k = 0; k < na; k++) lab[k] = 1;
	std::vector<double> t(rows.size());
#pragma omp parallel for
	for (long n = 0; n < (long)rows.size(); n++) t[n] = welch(rows[n], lab);
	for (size_t n = 0; n < rows.size(); n++)
		as->set_value(neurons[n], kt, createFloatValue(std::vector<double>{t[n]}));
	std::vector<double> null;
	std::mt19937_64 rng(seed);
	for (int p = 0; p < permutations; p++) {
		std::vector<int> pl = lab;
		std::shuffle(pl.begin(), pl.end(), rng);
		std::vector<double> tp(rows.size());
#pragma omp parallel for
		for (long n = 0; n < (long)rows.size(); n++) tp[n] = std::fabs(welch(rows[n], pl));
		null.insert(null.end(), tp.begin(), tp.end());
	}
	std::vector<double> out;
	for (int thr = 3; thr <= 8; thr++) {
		long obs = 0, ex = 0;
		for (double x : t) obs += std::fabs(x) >= thr;
		for (double x : null) ex += x >= thr;
		out.push_back((double)obs); out.push_back((double)ex / permutations);
	}
	out.push_back((double)rows.size()); out.push_back((double)na); out.push_back((double)nb);
	return out;
}

} // namespace lucy
