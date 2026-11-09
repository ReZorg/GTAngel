/*
 * lucy_atoms.h — the bridge from a lucyspace AtomSpace to the generator.
 */

#ifndef LUCY_ATOMS_H
#define LUCY_ATOMS_H

#include <opencog/atomspace/AtomSpace.h>

#include "lucy_infer.h"

namespace lucy {

// Read the architecture, tensor table and vocabulary from the atoms.
LucySpec spec_from_atomspace(opencog::AtomSpace* as);

} // namespace lucy

#endif
