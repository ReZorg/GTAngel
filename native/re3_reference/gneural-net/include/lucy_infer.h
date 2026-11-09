/*
 * lucy_infer.h — text generation for a Qwen3 model described by lucyspace.
 *
 * The model is read from atoms, not from the GGUF: the architecture from the
 * Model atom's lucy:param:* values, the tensor table (type, shape, files and
 * row ranges) from the Tensor atoms, and the vocabulary from the Token atoms.
 * The weights stay in the exploded folder (gguf-ops); they are memory-mapped
 * and read through dequantize-and-dot kernels (Q4_K, Q6_K, F32), so the
 * resident cost is the KV cache, not a float copy of the weights.
 *
 * LucySpec is plain data, so the inference core does not depend on the
 * AtomSpace; lucy_atoms.cc fills it from an AtomSpace.
 */

#ifndef LUCY_INFER_H
#define LUCY_INFER_H

#include <cstdint>
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace lucy {

struct TensorPart { std::string file; size_t bytes; size_t row0, row1; };

struct TensorSpec {
	std::string name, type;          // GGML type name: Q4_K, Q6_K, F32
	std::vector<size_t> shape;       // ne0 (the row length) first
	std::vector<TensorPart> parts;   // files relative to the folder
};

struct LucySpec {
	std::string folder;
	int layers = 0, d = 0, ffn = 0, heads = 0, kv_heads = 0, head_dim = 0;
	double eps = 1e-6, rope_base = 1e6;
	double yarn_factor = 0, yarn_orig_ctx = 0;  // 0: no YaRN
	std::map<std::string, TensorSpec> tensors;
	std::vector<std::string> vocab_raw;          // GPT-2 byte-level strings, by id
	std::vector<std::string> vocab_kind;         // normal, control, ...
};

// One mapped tensor: rows of quantized blocks, possibly across several files.
class Tensor {
public:
	std::string type;
	size_t cols = 0, rows = 0, row_bytes = 0;
	const uint8_t* row(size_t r) const;
	const float* f32() const;                    // F32 vectors only
	struct Part { const uint8_t* base; size_t row0, row1; };
	std::vector<Part> parts;
};

struct GenerateResult {
	std::vector<int> prompt_ids, ids;   // ids: generated only
	std::string text;                   // generated text
	double prompt_seconds = 0, gen_seconds = 0;
};

class Model {
public:
	explicit Model(const LucySpec& spec);
	~Model();

	std::vector<int> tokenize(const std::string& text) const;
	std::string detokenize(const std::vector<int>& ids) const;

	// Greedy generation of up to n tokens after the prompt; stops at an
	// end-of-text control token.
	GenerateResult generate(const std::string& prompt, int n);

	// Logits for the next token after feeding ids from position pos.
	const std::vector<float>& step(int token, int pos);
	void reset();

	const LucySpec& spec() const { return _spec; }

	// Called by step() with each layer's FFN activations h (F floats,
	// h_j = silu(gate_j . n) * (up_j . n)) for the token just fed.
	std::function<void(int layer, const float* h, int F)> on_ffn;

	// Pieces, for experiments that run parts of the model elsewhere:
	// one layer's FFN (SwiGLU) on an already normalized input, and a whole
	// tensor decoded to floats, row-major (rows x cols).
	std::vector<float> ffn(int layer, const std::vector<float>& n) const;
	std::vector<float> decode(const std::string& tensor, size_t& rows, size_t& cols) const;

private:
	LucySpec _spec;
	std::vector<std::pair<void*, size_t>> _maps;
	std::unordered_map<std::string, Tensor> _t;
	std::unordered_map<std::string, int> _raw_to_id;
	size_t _max_raw_len = 0;
	std::vector<int> _stop;
	// KV cache: [layer][pos][kv_heads * head_dim]
	std::vector<std::vector<float>> _kcache, _vcache;
	int _ctx = 0;
	std::vector<float> _logits;
	std::vector<double> _inv_freq, _ramp;
	double _mscale = 1.0;

	const Tensor& T(const std::string& name) const;
	const Tensor& L(int layer, const char* what) const;
	void map_tensor(const TensorSpec& ts);
	void ensure_ctx(int n);
};

// GGML block decoding (exposed for tests).
void dequant_row(const std::string& type, const uint8_t* row, size_t cols, float* out);
float dot_row(const std::string& type, const uint8_t* row, size_t cols, const float* x);

} // namespace lucy

#endif
