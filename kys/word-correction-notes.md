# Final Canonical Word — 5×4 Window Encoding (Dan, 2026-07-21, revision 2)

## The word (authoritative, from Dan)
```
step:  1   2   3   4   5   6   7   8   9   10  11  12
       T8E T5E T7E T1E T4E T2E T8E T5R T7R T1R T4R T2R
```
Mode word: `EEEEEEERRRRR` (7E + 5R contiguous — E run of 7, R run of 5).

## Verified properties (script-checked)
| Property | Result |
|---|---|
| Thread patterns (4-step offsets) | A: `EEEE\|EEER\|RRRR` · B: `EEER\|RRRR\|EEEE` · C: `RRRR\|EEEE\|EEER` |
| Per-thread budget | 7E + 5R each ✓ |
| Mode balance per tick | 2E1R ×9, 1E2R ×3 (at g=3,7,11) — never 3E/3R ✓ |
| Triangles | odd steps {4,7,8} Structure, even {1,2,5} Process ✓ |
| T8 in triad | every odd tick (g even); U rows carry coherence on the others ✓ |
| Polar pairs | full complement every tick ✓ |

Note: mode balance is NO LONGER strictly alternating 2E1R/1E2R — it is
9× 2E1R and 3× 1E2R (1E2R at ticks 3,7,11 i.e. step-triples {4,8,12}).
INV-4 text must be updated accordingly.

## The 5×4 window — a single 4-step window encodes all 3 cycles of 12
Base alignment (word rows = steps 1-4 / 5-8 / 9-12):
```
U1:  T8R  T8R  T9E  T9E
U2:  T6_  T2R  T3_  T6_
     T8E  T5E  T7E  T1E     ← steps 1-4
     T4E  T2E  T8E  T5R     ← steps 5-8
     T7R  T1R  T4R  T2R     ← steps 9-12
```
Shifted 2 steps to center the T6 symmetry:
```
U1:  T9E  T9E  T8R  T8R
U2:  T3_  T6_  T6_  T2R
     T4R  T2R  T8E  T5E     ← steps 11,12,1,2
     T7E  T1E  T4E  T2E     ← steps 3-6
     T8E  T5R  T7R  T1R     ← steps 7-10
```
Verified: Dan's base U-rows are exactly the shifted U-rows rotated by 2.
The canonical U sequences remain U1 = UT9E·UT9E·UT8R·UT8R (2-2) and
U2 = UT3_·UT6_·UT6_·UT2R (1-2-1), with window alignment choosing the rotation.

The three word-rows of the window ARE the three concurrent Particular
sequences: at any tick, the three threads sit in the same column of the
three rows (offset 4 = one row). So one 5×4 block = U1 + U2 + A/B/C rows
encodes the entire 36-step system.

## Files updated
- engineData.ts: CANONICAL_WORD, THREAD_PHASE_PATTERNS, INV-4/INV-6 text, corpus, console lines
- New component: WindowEncoding.tsx (5×4 block with base/centered toggle)
- Home.tsx prose + footer
