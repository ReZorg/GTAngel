/*
 * lucy_scm.c — lucyspace text generation as Scheme primitives and as a
 * "lib:" GroundedSchema, the two ways the OpenCog subsystems (miner, ECAN,
 * URE) expose C++ to the AtomSpace.
 *
 *   (use-modules (opencog lucy))
 *   (lucy-generate "The capital of France is" 16)   ; => " Paris. The ..."
 *   (lucy-tokenize "The capital of France is")      ; => (785 6722 315 9625 374)
 *
 *   (cog-execute!
 *     (ExecutionOutput
 *       (GroundedSchema "lib:<build>/liblucy.so\\lucy_generate_gs")
 *       (List (Concept "The capital of France is") (Number 16))))
 *   ; => (StringValue " Paris. The ...")
 *
 * The model is read from the caller's AtomSpace (the lucyspace atoms) the
 * first time, then kept, one per (Concept "lucy") atom.
 */

#include <map>
#include <memory>
#include <mutex>
#include <cstdio>
#include <stdexcept>

#include <opencog/atoms/atom_types/atom_types.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/value/FloatValue.h>
#include <opencog/atoms/value/StringValue.h>
#include <opencog/atoms/core/NumberNode.h>
#include <opencog/guile/SchemeModule.h>
#include <opencog/guile/SchemePrimitive.h>
#include <opencog/guile/SchemeSmob.h>

#include "lucy_atoms.h"
#include "lucy_shape.h"

using namespace opencog;

namespace lucy {

static std::mutex models_mtx;
static std::map<const Atom*, std::unique_ptr<Model>> models;

// The model whose atoms this AtomSpace sees, read on first use and kept,
// keyed by the (Concept "lucy") atom itself: a scratch frame (as
// ExecutionOutput makes) sees the same atom as its parent, so it shares
// the model.
static Handle model_atom(AtomSpace* as)
{
	Handle m = as->get_node(CONCEPT_NODE, "lucy");
	if (!m) throw std::runtime_error("lucy: no (Concept \"lucy\") in this AtomSpace");
	return m;
}

static Model& model_for(AtomSpace* as)
{
	Handle key = model_atom(as);
	auto it = models.find(key.get());
	if (it != models.end()) return *it->second;
	auto m = std::make_unique<Model>(spec_from_atomspace(as));
	return *(models[key.get()] = std::move(m));
}

std::string generate_text(AtomSpace* as, const std::string& prompt, int n)
{
	std::lock_guard<std::mutex> lock(models_mtx);
	return model_for(as).generate(prompt, n).text;
}

} // namespace lucy

// --- Scheme -------------------------------------------------------------------------------
class LucySCM : public ModuleWrap
{
protected:
	void init()
	{
		define_scheme_primitive("lucy-generate", &LucySCM::generate, this, "lucy");
		define_scheme_primitive("lucy-tokenize", &LucySCM::tokenize, this, "lucy");
		define_scheme_primitive("lucy-forget-model", &LucySCM::forget, this, "lucy");
		define_scheme_primitive("lucy-attach-ffn!", &LucySCM::attach_ffn, this, "lucy");
		define_scheme_primitive("lucy-ffn", &LucySCM::ffn, this, "lucy");
		define_scheme_primitive("lucy-direct-links!", &LucySCM::direct_links, this, "lucy");
		define_scheme_primitive("lucy-probe!", &LucySCM::probe, this, "lucy");
		define_scheme_primitive("lucy-motifs!", &LucySCM::motifs, this, "lucy");
		define_scheme_primitive("lucy-separation!", &LucySCM::separation, this, "lucy");
	}

	// The shape of Lucy, computed in the AtomSpace (lucy_shape.h).
	int direct_links(double zmin, int horizon)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-direct-links!");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		return (int)lucy::direct_links(as.get(), lucy::model_for(as.get()), zmin, horizon);
	}

	ValuePtr probe(const std::string& label, ValuePtr questions)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-probe!");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		StringValuePtr q = StringValueCast(questions);
		if (!q) throw std::runtime_error("lucy-probe!: questions must be a StringValue");
		std::vector<double> out;
		for (int n : lucy::probe(as.get(), lucy::model_for(as.get()), label, q->value())) out.push_back(n);
		return createFloatValue(out);
	}

	ValuePtr motifs(int K, int seed, int restarts)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-motifs!");
		return createFloatValue(lucy::motifs(as.get(), K, (unsigned)seed, restarts));
	}

	ValuePtr separation(const std::string& a, const std::string& b, int permutations)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-separation!");
		return createFloatValue(lucy::separation(as.get(), a, b, permutations, 0));
	}

	std::string generate(const std::string& prompt, int n)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-generate");
		return lucy::generate_text(as.get(), prompt, n);
	}

	ValuePtr tokenize(const std::string& text)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-tokenize");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		std::vector<double> ids;
		for (int id : lucy::model_for(as.get()).tokenize(text)) ids.push_back(id);
		return createFloatValue(ids);
	}

	bool forget()
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-forget-model");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		return lucy::models.erase(lucy::model_atom(as.get()).get()) > 0;
	}

	// Put layer L's FFN weights on its Neuron atoms as FloatValues:
	// lucy:w-gate (gate row n), lucy:w-up (up row n), lucy:w-down (down
	// column n). Each neuron then holds everything it needs, a membrane.
	int attach_ffn(int layer)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-attach-ffn!");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		lucy::Model& m = lucy::model_for(as.get());
		const std::string blk = "blk." + std::to_string(layer) + ".";
		size_t gr, gc, ur, uc, dr, dc;
		std::vector<float> gate = m.decode(blk + "ffn_gate.weight", gr, gc);
		std::vector<float> up = m.decode(blk + "ffn_up.weight", ur, uc);
		std::vector<float> down = m.decode(blk + "ffn_down.weight", dr, dc);   // dr = d rows, dc = F
		Handle kg = as->add_node(PREDICATE_NODE, "lucy:w-gate");
		Handle ku = as->add_node(PREDICATE_NODE, "lucy:w-up");
		Handle kd = as->add_node(PREDICATE_NODE, "lucy:w-down");
		char name[64];
		int count = 0;
		for (size_t n = 0; n < gr; n++) {
			std::snprintf(name, sizeof name, "lucy/L%02d/ffn/n%zu", layer, n);
			Handle h = as->get_node(CONCEPT_NODE, name);
			if (!h) continue;
			std::vector<double> g(gate.begin() + n * gc, gate.begin() + (n + 1) * gc);
			std::vector<double> u(up.begin() + n * uc, up.begin() + (n + 1) * uc);
			std::vector<double> col(dr);
			for (size_t i = 0; i < dr; i++) col[i] = down[i * dc + n];
			as->set_value(h, kg, createFloatValue(std::move(g)));
			as->set_value(h, ku, createFloatValue(std::move(u)));
			as->set_value(h, kd, createFloatValue(std::move(col)));
			count++;
		}
		return count;
	}

	// The C++ FFN of layer L on input x (a FloatValue), for comparison.
	ValuePtr ffn(int layer, ValuePtr x)
	{
		AtomSpacePtr as = SchemeSmob::ss_get_env_as("lucy-ffn");
		std::lock_guard<std::mutex> lock(lucy::models_mtx);
		FloatValuePtr fx = FloatValueCast(x);
		if (!fx) throw std::runtime_error("lucy-ffn: x must be a FloatValue");
		std::vector<float> n(fx->value().begin(), fx->value().end());
		std::vector<float> o = lucy::model_for(as.get()).ffn(layer, n);
		return createFloatValue(std::vector<double>(o.begin(), o.end()));
	}

public:
	LucySCM() : ModuleWrap("opencog lucy") {}
};

extern "C" {
void opencog_lucy_init(void);

// GroundedSchema "lib:<path>/liblucy.so\\lucy_generate_gs"
// args: (List (Concept <prompt>) (Number <n>)); returns a StringValue.
// ExecutionOutput runs in a scratch frame; the lucyspace atoms are read
// through it from the parent space.
ValuePtr* lucy_generate_gs(AtomSpace* as, Handle* args);
}

void opencog_lucy_init(void)
{
	static LucySCM lucy_scm;
	lucy_scm.module_init();
}

ValuePtr* lucy_generate_gs(AtomSpace* as, Handle* args)
{
	const Handle& list = *args;
	if (!list || list->get_arity() < 1) return nullptr;
	const std::string prompt = list->getOutgoingAtom(0)->get_name();
	int n = 16;
	if (list->get_arity() > 1) {
		NumberNodePtr num = NumberNodeCast(list->getOutgoingAtom(1));
		if (num) n = (int)num->get_value();
	}
	return new ValuePtr(createStringValue(lucy::generate_text(as, prompt, n)));
}
