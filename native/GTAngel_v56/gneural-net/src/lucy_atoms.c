/*
 * lucy_atoms.c — read a LucySpec from a lucyspace AtomSpace.
 *
 *   (Concept "lucy")          lucy:folder, lucy:param:<name> values
 *   members of lucy:Tensor    lucy:ggml-type, lucy:shape, lucy:files,
 *                             lucy:file-bytes, lucy:file-rows
 *   members of lucy:Token     lucy:index, lucy:raw; kind from lucy:Token:<kind>
 *
 * Spaces saved before eps, rope-base and the YaRN parameters were exported
 * as atoms get them from the exploded folder's metadata.json instead.
 */

#include "lucy_atoms.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <opencog/atoms/atom_types/atom_types.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/value/FloatValue.h>
#include <opencog/atoms/value/StringValue.h>

using namespace opencog;

namespace lucy {

static ValuePtr value(AtomSpace* as, const Handle& atom, const std::string& key)
{
	Handle k = as->get_node(PREDICATE_NODE, std::string(key));
	return k ? atom->getValue(k) : nullptr;
}

static std::vector<double> floats(AtomSpace* as, const Handle& atom, const std::string& key)
{
	ValuePtr v = value(as, atom, key);
	FloatValuePtr f = FloatValueCast(v);
	return f ? f->value() : std::vector<double>{};
}

static std::vector<std::string> strings(AtomSpace* as, const Handle& atom, const std::string& key)
{
	ValuePtr v = value(as, atom, key);
	StringValuePtr s = StringValueCast(v);
	return s ? s->value() : std::vector<std::string>{};
}

static HandleSeq members(AtomSpace* as, const std::string& type)
{
	HandleSeq out;
	Handle t = as->get_node(CONCEPT_NODE, "lucy:" + type);
	if (!t) return out;
	for (const Handle& m : t->getIncomingSetByType(MEMBER_LINK))
		if (m->getOutgoingAtom(1) == t) out.push_back(m->getOutgoingAtom(0));
	return out;
}

// A number from metadata.json: {"<key>": {"type": ..., "value": <number>}}
static bool metadata_number(const std::string& folder, const std::string& key, double& out)
{
	std::ifstream f(folder + "/metadata.json");
	if (!f) return false;
	std::stringstream ss;
	ss << f.rdbuf();
	const std::string s = ss.str();
	size_t at = s.find("\"" + key + "\"");
	if (at == std::string::npos) return false;
	at = s.find("\"value\"", at);
	if (at == std::string::npos) return false;
	at = s.find(':', at);
	out = std::strtod(s.c_str() + at + 1, nullptr);
	return true;
}

LucySpec spec_from_atomspace(AtomSpace* as)
{
	LucySpec spec;
	Handle model = as->get_node(CONCEPT_NODE, "lucy");
	if (!model) throw std::runtime_error("lucy: no (Concept \"lucy\") in this AtomSpace");
	auto folder = strings(as, model, "lucy:folder");
	if (folder.empty()) throw std::runtime_error("lucy: the model atom has no lucy:folder");
	spec.folder = folder[0];

	auto param = [&](const char* name, double& out, bool required) {
		auto v = floats(as, model, std::string("lucy:param:") + name);
		if (!v.empty()) { out = v[0]; return true; }
		if (required) throw std::runtime_error(std::string("lucy: missing parameter ") + name);
		return false;
	};
	double layers, width, ffn, heads, kv, hd;
	param("layers", layers, true); param("width", width, true); param("ffn", ffn, true);
	param("q-heads", heads, true); param("kv-heads", kv, true); param("head-dim", hd, true);
	spec.layers = (int)layers; spec.d = (int)width; spec.ffn = (int)ffn;
	spec.heads = (int)heads; spec.kv_heads = (int)kv; spec.head_dim = (int)hd;

	auto arch = strings(as, model, "lucy:param:arch");
	const std::string a = arch.empty() ? "qwen3" : arch[0];
	if (!param("eps", spec.eps, false))
		metadata_number(spec.folder, a + ".attention.layer_norm_rms_epsilon", spec.eps);
	if (!param("rope-base", spec.rope_base, false))
		metadata_number(spec.folder, a + ".rope.freq_base", spec.rope_base);
	if (!param("yarn-factor", spec.yarn_factor, false)) {
		std::ifstream f(spec.folder + "/metadata.json");
		std::stringstream ss; ss << f.rdbuf();
		if (ss.str().find("\"yarn\"") != std::string::npos) {
			metadata_number(spec.folder, a + ".rope.scaling.factor", spec.yarn_factor);
			metadata_number(spec.folder, a + ".rope.scaling.original_context_length", spec.yarn_orig_ctx);
		}
	} else param("yarn-orig-ctx", spec.yarn_orig_ctx, true);

	const std::string prefix = "lucy/tensor/";
	for (const Handle& t : members(as, "Tensor")) {
		TensorSpec ts;
		ts.name = t->get_name().substr(prefix.size());
		ts.type = strings(as, t, "lucy:ggml-type").at(0);
		for (double x : floats(as, t, "lucy:shape")) ts.shape.push_back((size_t)x);
		auto files = strings(as, t, "lucy:files");
		auto bytes = floats(as, t, "lucy:file-bytes");
		auto rows = floats(as, t, "lucy:file-rows");
		for (size_t i = 0; i < files.size(); i++)
			ts.parts.push_back({files[i], (size_t)bytes.at(i), (size_t)rows.at(2 * i), (size_t)rows.at(2 * i + 1)});
		spec.tensors.emplace(ts.name, std::move(ts));
	}

	HandleSeq tokens = members(as, "Token");
	spec.vocab_raw.assign(tokens.size(), "");
	spec.vocab_kind.assign(tokens.size(), "normal");
	for (const Handle& t : tokens) {
		auto idx = floats(as, t, "lucy:index");
		auto raw = strings(as, t, "lucy:raw");
		if (idx.empty() || raw.empty()) continue;
		const size_t i = (size_t)idx[0];
		if (i < spec.vocab_raw.size()) spec.vocab_raw[i] = raw[0];
	}
	for (const char* kind : {"control", "user-defined", "pad", "unused", "byte", "unknown"})
		for (const Handle& t : members(as, std::string("Token:") + kind)) {
			auto idx = floats(as, t, "lucy:index");
			if (!idx.empty() && (size_t)idx[0] < spec.vocab_kind.size()) spec.vocab_kind[(size_t)idx[0]] = kind;
		}
	if (spec.tensors.empty() || spec.vocab_raw.empty())
		throw std::runtime_error("lucy: no tensors or no tokens in this AtomSpace");
	return spec;
}

} // namespace lucy
