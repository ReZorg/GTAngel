#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "lucy_persona.h"
#include "lucy_infer.h"
#include "lucy_atoms.h"
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/file/fast_load.h>
static const double kTarget[LUCY_PERSONA_OUT] = {0.55, -0.25, 0.40, 0.10};
static const char *kName[LUCY_PERSONA_OUT] = {"warmth", "brevity", "curiosity", "formality"};
static void pool(const std::vector<float>& h, double obs[LUCY_PERSONA_IN])
{
	int i;
	for (i = 0; i < LUCY_PERSONA_IN; i++)
		obs[i] = 0;
	if (h.empty())
		return;
	double peak = 1e-8;
	for (i = 0; i < LUCY_PERSONA_IN; i++) {
		size_t a = (size_t)i * h.size() / LUCY_PERSONA_IN;
		size_t b = (size_t)(i + 1) * h.size() / LUCY_PERSONA_IN;
		double s = 0;
		for (size_t j = a; j < b; j++)
			s += std::fabs(h[j]);
		obs[i] = s / (double)(b > a ? (b - a) : 1);
		if (obs[i] > peak)
			peak = obs[i];
	}
	for (i = 0; i < LUCY_PERSONA_IN; i++)
		obs[i] = std::tanh(obs[i] / peak);
}
static void synthetic(int iter, const double knobs[LUCY_PERSONA_OUT], double obs[LUCY_PERSONA_IN])
{
	for (int i = 0; i < LUCY_PERSONA_IN; i++) {
		double k = knobs[i % LUCY_PERSONA_OUT];
		obs[i] = std::tanh(0.12 * iter + 0.25 * i + 0.5 * k);
	}
}
int main(int argc, char** argv)
{
	const char* space = 0;
	int iters = 6;
	if (argc >= 2 && argv[1][0] != 0 && std::string(argv[1]) != "-")
		space = argv[1];
	if (argc >= 3)
		iters = std::atoi(argv[2]);
	if (iters < 1)
		iters = 1;
	if (iters > 1000)
		iters = 1000;
	std::unique_ptr<lucy::Model> model;
	opencog::AtomSpacePtr as;
	if (space) {
		try {
			as = opencog::createAtomSpace();
			opencog::load_file(space, as);
			model.reset(new lucy::Model(lucy::spec_from_atomspace(as.get())));
			std::printf("lucy model: %d layers, d=%d, ffn=%d\n",
			            model->spec().layers, model->spec().d, model->spec().ffn);
		} catch (const std::exception& e) {
			std::printf("lucy model not used (%s); signature is synthetic\n", e.what());
			model.reset();
		}
	} else {
		std::printf("no Atomese path; signature is synthetic\n");
	}
	lucy_persona* persona = lucy_persona_new();
	if (!persona) {
		std::fprintf(stderr, "persona alloc failed\n");
		return 1;
	}
	double knobs[LUCY_PERSONA_OUT] = {0, 0, 0, 0};
	std::printf("iter  err     warmth   brevity  curiosity formality  source\n");
	for (int iter = 1; iter <= iters; iter++) {
		double obs[LUCY_PERSONA_IN];
		const char* source = "synthetic";
		if (model && model->spec().d > 0 && model->spec().layers > 0) {
			try {
				std::vector<float> n((size_t)model->spec().d, 0.f);
				for (int i = 0; i < model->spec().d; i++)
					n[i] = (float)(0.05 * knobs[i % LUCY_PERSONA_OUT]);
				std::vector<float> h = model->ffn((iter - 1) % model->spec().layers, n);
				pool(h, obs);
				source = "lucy-ffn";
			} catch (const std::exception& e) {
				synthetic(iter, knobs, obs);
				source = "synthetic";
				std::printf("ffn skipped: %s\n", e.what());
			}
		} else {
			synthetic(iter, knobs, obs);
		}
		double err = lucy_persona_step(persona, obs, kTarget, knobs);
		std::printf("%4d  %.4f", iter, err);
		for (int k = 0; k < LUCY_PERSONA_OUT; k++)
			std::printf("  %7.3f", knobs[k]);
		std::printf("   %s\n", source);
	}
	std::printf("target");
	for (int k = 0; k < LUCY_PERSONA_OUT; k++)
		std::printf(" %s=%.2f", kName[k], kTarget[k]);
	std::printf("\n");
	lucy_persona_free(persona);
	return 0;
}