# System 2: The Primordial Digon and the Group Theory of the Simplex Hierarchy

## System 2 — The Digon (1-simplex)

Pascal row: [1, 2, 1] = 4
Nesting: ((a)b)
base[3] -> (2)(1)

### f-vector decomposition
- **1 void**: Self-grounding
- **2 vertices v(2) = {a, b}**: The 2 terms of the primordial dyad
- **1 edge e(2) = {ab}**: The single bond connecting them

### Step sequence derivation
Following the pattern:
- System 4: 3 threads, 12 steps, 7E+5R, phase offset = 4 steps (120°)
- System 3: 2 threads, 4 steps, 3E+1R, phase offset = 3 steps (270°)
- System 2: 1 thread, 2 steps, ?E+?R

With 1 thread and 2 terms {a, b} (or equivalently term 1), the cycle is:

Set 2A: 1E 1R  (or equivalently: aE bR? or 1E 1R?)

Actually — with only 1 polar pair [a(b)] and 1 thread, there's no
concurrent partner. The system simply FLIPS between its two orientations.

The mode split: with 2 steps, the minimal viable split is 1E + 1R.
This is the **perceptual shift transformation** — one step forward (E),
one step backward (R). Pure alternation. The primordial oscillation.

### The C_2 foundation
The symmetry group of the 1-simplex (line segment/digon) is S_2 = C_2 = Z_2.
|C_2| = 2! = 2 elements: {identity, flip}

This IS the irreducible building block. Every higher system contains C_2
as its fundamental subgroup — the E/R duality itself.

## Group Theory Analysis

### System 2: C_2
- |C_2| = 2
- Elements: {e, σ} where σ² = e
- The flip. The bit. The qubit. The primordial distinction.
- 2 steps = |C_2| = 2

### System 3: S_3 ≅ D_3
- |S_3| = 6
- But the CYCLE has 4 steps, not 6.
- The cycle length = number of PARTICULAR terms × 2 = 2 × 2 = 4
- Or: cycle = (N-1)! + (N-2)! = 2! + 1! = 3 + 1 = 4? No...
- Actually: System 3 has 4 steps = 2 × 2 (2 particular terms × 2 modes)
- The 4-step cycle lives inside S_3 as... what?
  
  S_3 has subgroups: {e}, C_2, C_3, S_3
  |=1, |=2, |=3, |=6
  
  4 is NOT a divisor of 6! So the 4-step cycle is NOT a subgroup of S_3.
  
  This means the cycle is not purely the simplex symmetry group.
  The cycle operates on the TERMS, not on the VERTICES.
  
  The 4 steps cycle through: 1R, 3E, 1E, 3E
  This is a sequence over {1,3} × {E,R} = 4 possible states.
  The transformation is a permutation of these 4 states.
  
  The transformation group acting on the step sequence:
  - Shift by 3: Set 3A -> Set 3B (rotation)
  - This generates C_4 acting on the 4 positions.
  - But C_4 has order 4, and we only have 2 threads.
  - The 2 threads = C_4 / C_2 ≅ C_2 (quotient)
  
  So System 3's thread structure is C_4 with 2 cosets of C_2.

### System 4: The Big Question

The 12-step cycle with 3 threads offset by 4 steps.

Candidate groups:
1. **T_h** (pyritohedral symmetry): |T_h| = 24 = 2 × 12
   T_h = A_4 × C_2 (direct product of alternating group and inversion)
   
2. **A_4** (alternating/chiral tetrahedral): |A_4| = 12
   The group of EVEN permutations of 4 elements.
   Subgroups: V_4 (Klein four), C_3, C_2, {e}
   
3. **S_4** (full tetrahedral): |S_4| = 24
   All permutations. S_4 = A_4 ⋊ C_2

4. **T_d** (full tetrahedral symmetry): |T_d| = 24
   T_d ≅ S_4

5. **T** (chiral tetrahedral): |T| = 12
   T ≅ A_4

The 12-step cycle = |A_4| = 12. This is the CHIRAL tetrahedral group.
The 3 threads correspond to the 3 elements of order 3 in A_4 (the C_3 subgroup).
The 4-step offset = 12/3 = the index [A_4 : C_3] = 4.

But what about the E/R duality? Each thread has 7E + 5R = 12 steps.
The E/R flip is a C_2 action. So the FULL transformation group is:

**A_4 × C_2 = T_h** (order 24)

Wait — but T_h is the pyritohedral group, which has the structure:
T_h = A_4 × Z_2

The 12 cycle steps = A_4 (the even permutations = rotations)
The E/R duality = Z_2 (the inversion/reflection)
Together = T_h = A_4 × Z_2 of order 24.

But the user asks about Z_3 × A_4 or Z_3 × D_2h × D_2h...

Let's check: Z_3 × A_4 has order 3 × 12 = 36. That's too big for
the basic structure but could describe the FULL triad system
(3 threads × 12 steps = 36 total thread-steps).

Actually: the 3 concurrent threads executing 12 steps each gives
3 × 12 = 36 total operations. And Z_3 × A_4 has order 36.

The Z_3 factor = the cyclic permutation of the 3 threads (A->B->C->A).
The A_4 factor = the 12-step cycle within each thread.

So the FULL transformation group of the triad system is:
**Z_3 × A_4** of order 36

And T_h = A_4 × Z_2 of order 24 describes the single-thread + mode structure.

### The Hierarchy of Groups

System 2: C_2 (order 2) — the flip
System 3: C_4 (order 4) — the 4-step rotation with 2 cosets
System 4: Z_3 × A_4 (order 36) — 3 threads × 12 rotations

But there's a deeper pattern. Each system CONTAINS the previous:
- C_2 ⊂ C_4 (as the index-2 subgroup)
- C_4 ... hmm, C_4 is NOT a subgroup of A_4.

Let me reconsider. The groups should nest:

System 2: Z_2 (order 2)
System 3: Z_2 × Z_2 = V_4 (order 4)? — the Klein four-group
  - This would make the 4 steps correspond to the 4 elements of V_4
  - V_4 = {e, a, b, ab} with a²=b²=(ab)²=e
  - The 2 threads = the 2 generators {a, b}
  - V_4 IS a subgroup of A_4! (the normal subgroup)

System 4: A_4 (order 12) for the cycle, or Z_3 ⋉ V_4 = A_4 
  since A_4 = V_4 ⋊ Z_3

So the nesting is:
**Z_2 ⊂ V_4 ⊂ A_4**

And the full triad system: Z_3 × A_4 or A_4 ⋊ Z_3 depending on
whether the thread permutation commutes with the cycle steps.

Since shifting all 3 threads by 1 step is the same as not shifting
(it's a global phase), the thread permutation Z_3 and the cycle A_4
DO commute, giving the direct product Z_3 × A_4.

But wait: A_4 already contains Z_3 as a subgroup (the 3-cycles).
So Z_3 × A_4 has Z_3 appearing twice — once as the thread permutation
and once inside A_4 as the face rotations.

This is the key insight: the EXTERNAL Z_3 (thread cycling) and the
INTERNAL Z_3 (within A_4, as face rotations) are DIFFERENT actions
on different spaces. The external one permutes threads; the internal
one rotates within the cycle.
