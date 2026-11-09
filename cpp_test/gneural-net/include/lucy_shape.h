/*
 * lucy_shape.h — the shape of Lucy computed inside the AtomSpace: direct
 * neuron -> neuron links, gauge-invariant motifs and roles, and neuron
 * activations on questions. Every input is read through lucyspace atoms
 * (tensors located by their lucy:files values, neurons as Concept atoms) and
 * every result is written back as atoms and values.
 */

#ifndef _LUCY_SHAPE_H
#define _LUCY_SHAPE_H

#include <string>
#include <vector>

#include <opencog/atomspace/AtomSpace.h>

#include "lucy_infer.h"

namespace lucy {

// Direct skip-path weights d_i . (n_M * g_j) and d_i . (n_M * u_j) as cosines,
// for M - L <= horizon; every pair with |cos| sqrt(d) >= zmin becomes
//   (Inheritance (Concept A) (Concept B))  TV (|cos| of the stronger path,
//   z^2 / (z^2 + 800)); values lucy:gate, lucy:up (signed cos), lucy:span.
// Neurons get TV (1/F, 0.9). Returns the number of (pair, path) links.
long direct_links(opencog::AtomSpace* as, Model& m, double zmin, int horizon);

// Feed each question in the chat template and record mean |h_j| over the
// question's own tokens; appended per question to the FloatValue under
// (Predicate "lucy:act/<label>") on every neuron atom. Returns the token
// counts per question.
std::vector<int> probe(opencog::AtomSpace* as, Model& m, const std::string& label,
                       const std::vector<std::string>& questions);

// Census of feed-forward loops A(L) -> B(L+1) -> C(L+2) with skip A -> C over
// the Inheritance links in the AtomSpace, by ports (AB, BC, AC); per class:
// loops, coherent loops, two-paths, expected closures. Then 12 invariant
// features per neuron (lucy:motif-features) and k-means roles written as
// (Member neuron (Concept "lucy:Role/<k>")). Returns the census as a flat
// list: 8 classes x (loops, coherent, expected).
std::vector<double> motifs(opencog::AtomSpace* as, int K, unsigned seed, int restarts);

// Welch t per neuron between the lucy:act/<a> and lucy:act/<b> values,
// written as lucy:t/<a>-<b>; with a label-permutation null. Returns, for
// |t| >= 3..8: observed count, expected count.
std::vector<double> separation(opencog::AtomSpace* as, const std::string& a, const std::string& b,
                               int permutations, unsigned seed);

} // namespace lucy

#endif
