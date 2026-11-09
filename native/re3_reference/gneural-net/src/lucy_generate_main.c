/*
 * lucy-generate — load a saved lucyspace (plain Atomese) into an AtomSpace,
 * read the model from its atoms, and generate text greedily.
 *
 *   lucy-generate SPACE.atomese.scm "The capital of France is" [N]
 */

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/file/fast_load.h>

#include "lucy_atoms.h"

using namespace opencog;

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cerr << "usage: lucy-generate SPACE.atomese.scm PROMPT [N]\n";
		return 2;
	}
	const int n = argc > 3 ? std::atoi(argv[3]) : 16;
	using clk = std::chrono::steady_clock;
	auto t0 = clk::now();
	AtomSpacePtr as = createAtomSpace();
	load_file(argv[1], as);
	auto t1 = clk::now();
	lucy::Model model(lucy::spec_from_atomspace(as.get()));
	auto t2 = clk::now();
	lucy::GenerateResult r = model.generate(argv[2], n);
	auto secs = [](auto a, auto b) { return std::chrono::duration<double>(b - a).count(); };
	std::printf("atoms %zu, loaded in %.1f s; model read from atoms and mapped in %.2f s\n",
	            as->get_size(), secs(t0, t1), secs(t1, t2));
	std::printf("prompt: %zu tokens in %.2f s; generated %zu tokens in %.2f s (%.2f tokens/s)\n",
	            r.prompt_ids.size(), r.prompt_seconds, r.ids.size(), r.gen_seconds,
	            r.ids.size() / (r.gen_seconds > 0 ? r.gen_seconds : 1));
	std::printf("%s%s\n", argv[2], r.text.c_str());
	return 0;
}
