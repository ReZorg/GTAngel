# Simplex Polytope Hierarchy — Formal Analysis

## The Core Insight

Each System N uses the (N-1)-simplex as its base polytope.
The Pascal row [C(N-1,0), C(N-1,1), ..., C(N-1,N-1)] gives the f-vector.
The simplex is self-grounding — no external ground needed because the
phase space is balanced by the simplex geometry itself.

## System 3: Triangular Simplex (2-simplex)

Pascal row: [1, 3, 3, 1] = 8 total

The base number is 5 (from the Ontelecho terms: sys3 has 4 terms a(4)=4).
Wait — let me re-read the user's notation.

User says: base[5]->(4)(2)(1,3)
- base = 5 (total terms including the base/ground)
- (4) = the universal term (like T9 in System 4)
- (2) = the coherence term (like T8 in System 4)  
- (1,3) = the 2 particular terms

So System 3 has:
- 4 terms total (a(4) = 4 from A000081)
- 2 particular terms: {1, 3}
- 1 polar pair: [1(3)] — but wait, user says "3 polar pair edges"

Let me reconsider. The user says:
- v(3) = {a,b,c} — 3 vertices = 3 steps
- e(3) = {ab,ac,bc} — 3 edges = 2 concurrent threads form dyads  
- f(3) = {abc} — 1 face = holds 3 vertices in 3 polar pair edges of 1 triadic face

And the user's table:
Set 3A: 1R 3E 1E 3E  (4 steps)
Set 3B: 3E 1E 3E 1R  (4 steps)

Modes:
Set 3A: R E E E  → 3E + 1R
Set 3B: E E E R  → 3E + 1R

So System 3 has:
- 2 threads (not 3 — because 2-simplex has 3 edges but 2 concurrent threads)
- 4 steps per cycle (not 3!)
- 3E + 1R per thread (the 1R is the "Neutron Mode" / Regenerative pivot)
- The 2 threads are offset by 3 steps (= 270° in the 4-step cycle? Or 1 step?)

Wait: Set 3A starts with R, Set 3B ends with R.
If we look at the offset: Set 3B = Set 3A shifted by 1 step forward.
  3A: 1R 3E 1E 3E
  3B: 3E 1E 3E 1R

Actually 3B is 3A rotated by 1 position:
  3A: [1R, 3E, 1E, 3E]
  3B: [3E, 1E, 3E, 1R]

3B = 3A shifted left by 1. That's 1/4 of the cycle = 90°.
But for 2 threads in a 4-step cycle, the offset should be 4/2 = 2 steps (180°)?

Hmm, let me look again:
  3A: R  E  E  E
  3B: E  E  E  R

3B is 3A shifted by 3 steps (or equivalently -1 step).
In a 4-step cycle: 3/4 * 360° = 270° (or equivalently -90°).

For 2 threads: 360°/2 = 180° would be the balanced offset.
But the user shows 270° (or 90°) offset. 

Actually, let me reconsider: maybe the offset is determined differently.
The 2-simplex has vertices {a,b,c}. The 2 threads correspond to 2 of the 
3 edges (the faces of the simplex minus 1?). 

Or: the number of concurrent threads = N-2 for System N?
System 4: 3 threads (N-1 = 3)
System 3: 2 threads (N-1 = 2)

Steps per cycle: 
System 4: 12 steps = 3 × 4 (threads × vertices)
System 3: 4 steps... hmm, that's 2 × 2? Or just 4 (the number of terms)?

Actually: System 3 has a(4) = 4 terms. The cycle length = number of terms?
System 4 has a(5) = 9 terms, but the cycle is 12 steps.

Let me think about this differently.
System 4: 12 steps = 2 × 6 particular terms (each visited twice per cycle)
System 3: 4 steps = 2 × 2 particular terms (each visited twice per cycle)

That works! The cycle length = 2 × |particular terms|.
System 4: 2 × 6 = 12 ✓
System 3: 2 × 2 = 4 ✓

And the mode split:
System 4: 7E + 5R (E = 2×|particular|+1, R = 2×|particular|-1? No... 7+5=12)
System 3: 3E + 1R (E = 3, R = 1, total = 4)

Ratio E:R:
System 4: 7:5 
System 3: 3:1

Hmm. The Regenerative count:
System 4: 5R out of 12 steps
System 3: 1R out of 4 steps

The user calls the single R step "Neutron Mode" — interesting!

## Simultaneity Check for System 3

Step 1: 3A=1R, 3B=3E → terms {1,3} → the single polar pair [1(3)] is active ✓
Step 2: 3A=3E, 3B=1E → terms {3,1} → the single polar pair [1(3)] is active ✓  
Step 3: 3A=1E, 3B=3E → terms {1,3} → the single polar pair [1(3)] is active ✓
Step 4: 3A=3E, 3B=1R → terms {3,1} → the single polar pair [1(3)] is active ✓

At every step, both sides of the polar pair are active! ✓
At every step, E and R co-occur! (1E+1R at steps 1,4; 2E+0R at steps 2,3... wait)

Step 1: R + E = mixed ✓
Step 2: E + E = pure E ✗?
Step 3: E + E = pure E ✗?
Step 4: E + R = mixed ✓

Hmm, steps 2 and 3 are pure Expressive. That's different from System 4 where
every step is mixed. Maybe this is the "Neutron Mode" insight — in System 3,
the single R step per thread creates a "pulse" rather than continuous mixing.

## The f-vector Mapping

### System 3 (2-simplex / triangle): [1, 3, 3, 1]
- 1 empty set (ground/void)
- 3 vertices v(3) = {a,b,c} → 3 steps? Or 3 terms?
  Actually: the user says v(3)={a,b,c} marks 3 vertices.
  But the cycle has 4 steps. So vertices ≠ steps here.
  
  Let me re-read: "v(3)={a,b,c} so it has 3 steps"
  But then the table shows 4 steps. The user notices this:
  "interesting.. so it looks like system 3 has a 4 step total??"
  
  So the user is discovering that the cycle length exceeds the vertex count!
  The extra step is the Regenerative/Neutron step.

### System 4 (3-simplex / tetrahedron): [1, 4, 6, 4, 1]
- 1 void
- 4 vertices v(4) = {a,b,c,d} → 4 distinct steps (the nesting ((((a)b)c)d))
- 6 edges e(4) = {ab,ac,ad,bc,bd,cd} → 6 particular terms with E+R = 6 dyads
- 4 faces f(4) = {abc,abd,acd,bcd} → 4 triads (3 concurrent + 1 = the 4 faces)
  Wait: 3 concurrent threads but 4 faces. The 4th face is the "view from outside"?
- 1 cell c(4) = {abcd} → the tetrad holding all 4 vertices simultaneously

## The Nesting Structure

System 4: ((((a)b)c)d) — 4 levels of nesting = 4 vertices
System 3: (((a)b)c) — 3 levels of nesting = 3 vertices

This is the Matula-number nesting! Each vertex adds one level of depth.
