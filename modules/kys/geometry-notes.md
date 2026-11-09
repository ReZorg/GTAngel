# Geometry Page Notes (/geometry)

## Diagram 2 target: ng2p-3 (viewed)
ng2p-3.gif: a 9-point enneagram-style circle (S3, base 10). Structure:
- 9 points on circle + zero point at top (black square markers at top, lower-left, lower-right = the 3/6/9 "informing" points, division by 3)
- RED solid thick lines: the 1-4-2-8-5-7 hexad path (1/7 = 0.142857 repeating), drawn as the classic enneagram hexad — these are "resolving patterns"
- BLACK dashed lines: radial spokes from center (asterisk at center) to all 9 points + connections; center marked with *
- Black square dots at 3, 6, 9 positions (the triangle points, pattern 3→3→3 from 1/3)
So "ng2p-3" = N-Gram 2nd-Power #3 = S3 = 9 points, base 10, divisor pair 3 & 7. Enneagram.
NOTE: title on solbakkn: S3 is THE Enneagram. Our 12-step cycle maps onto S3 per SGRAMS_ANALYSIS.md:
- Steps {1,5,9}: Stream 0 division by 3
- Steps {2,6,10}: Stream 1 div by 7 pattern 1
- Steps {3,7,11}: Stream 2 div by 7 pattern 2
- Steps {4,8,12}: Integration/zero point
- 1/7 hexad: 1→4→2→8→5→7; 1/3 triangle: 3,6,9; zero point top (0/9)

## S4-Ennead-4-Steps.jpg (viewed) — KEY for diagram 3 (tetrahedron permutation)
Shows "Four Steps in Each Cycle": 3 Particular Sets transform through six-term sequence one Step apart → alternating subjective/objective synchronous groupings, regulated by Primary (T9/T8R) + Secondary Universal sets in repeating 4-step Cycles. Primary T9 shifts to subjective T8R alignment in Step 3, reverts to objective at new cycle.
Each step panel: 9 points around a big triangle (ennead), two overlaid triads (yellow + mauve triangles), black arrows showing term transitions:
- Step 1: T8E(8), T9(9), T7R(7), T4E(4), T3(3) active; Cycle #1
- Step 2: T9(9), T1R(1), T2E(2), T6(6), T5E(5)
- Step 3: T8R(8), T8E(8/blue), T7E(7), T6(6), T4R(4) — triangles rotate (mauve triad now pointing right)
- Step 4: T8R(8), T1E(1), T2E/T2R(2), T5R(5)
- Step 1 (Cycle #2): back to T8E/T9/T7R/T4E/T3 arrangement
The triangles visibly ROTATE/permute across steps — this is the "faces permuting" dynamic.

## Diagram 3: tetrahedron
- 4 triads as 4 faces of K4 tetrahedron; permute over 4 vertices & 6 edges
- From Gauge Theory doc §4: System 5 tetradic structure = K4: 4 vertices, 6 edges P(1,2)...P(3,4), 4 faces; thread multiplexing = parallel transport along all 6 edges; two gauge orbits:
  MP1: P[1,2,3] → P[1,2,4] → P[1,3,4] → P[2,3,4]
  MP2: P[1,3,4] → P[2,3,4] → P[1,2,3] → P[1,2,4]
- Triads {1,5,9},{2,6,10},{3,7,11},{4,8,12} = holonomy groups of the three 120° sections; 4 triads = 4 faces.
- SGRAMS mapping: 6-length patterns = 6 dyadic edges per tetrahedron; 4M₃ tetrahedral coordinates (4 coords sum to zero).

## Confirmed from N-Grams-SystemsTalk.pdf (Sigurd Andersen, solbakkn.com/math/n-grams.htm)
- ng2p-N gif naming = "N-Gram 2nd Power" figure #N = S-Gram S_N. So ng2p-3 = S3 = THE ENNEAGRAM: 9 points, base 10, divisor pair 3 & 7.
- ng2p-3.gif structure confirmed: 9 perimeter points; top point = 0/9 (zero point, small circle/black square); black square dots at 0/9, 3, 6 positions (single repeating digits from 1/3, 2/3, 0.999) = "informing" digits; RED thick lines = "resolving pattern" = hexad 1→4→2→8→5→7 (1/7=0.142857); dashed lines = radial reference spokes to center (*).
- Andersen convention: small circles at 0/9, 3, 6 instead of the classic triangle. No arrows in S-gram figures.
- ng2p-4 = S4 (16 points base 17, divisors 4 & 13): red hexads 1/13 (1,5,3,15,11,13) + 2/13 (2,10,7,14,6,9), orange = second pattern family, black squares at 4,8,12 (1/4 pattern) + zero.
- S-gram key: almost all patterns length 6; pattern-of-2 every 3rd S-gram; S3 hexad + 3-6-9 informing + zero point.
- S3 numerical: 1/7 hexad digits in order 1→4→2→8→5→7; 1/3: 3→3 (static); 0/9 top.

## Page plan (/geometry)
Three synced diagrams, all driven by shared TickContext:
1. RotorPanel: reuse EnneadRotor (12-step ternary mask rotor)
2. SGramPanel: ng2p-3 (S3 enneagram) with 9 circle points + zero point; red hexad path 1-4-2-8-5-7 animated; 3-6-9 triangle; map current tick: step-triple {g+1,g+5,g+9} highlight; integration steps {4,8,12} pulse zero point
3. TetraPanel: SVG 3D-projected rotating tetrahedron, 4 faces = 4 triads (colored), 6 edges labeled P(i,j), MP1/MP2 gauge orbit face-highlight sequence advancing each tick (4-step cycle); vertices = 4 interfaces C1-C4
Design: same Phase-Locked Luminescence tokens; nav link GEOMETRY; route /geometry in App.tsx.

## FINAL derivations (verified)

### Panel 2 — SGramRotor (ng2p-3 = S3 enneagram)
Geometry: 9 perimeter points, 0/9 at TOP; angle(d) = -90° + d·40° (clockwise). Andersen convention: small circles/black squares at 0/9, 3, 6 = informing points (no triangle drawn); RED hexad path 1→4→2→8→5→7 (resolving, 1/7=0.142857); dashed radial spokes to center *.
Tick mapping (per SGRAMS_ANALYSIS.md cognitive-loop table): step family f = ((s-1)%4)+1:
- family 1 (steps 1,5,9) → informing stream (div 3): marker cycles 3 → 6 → 0(9)
- family 2 (steps 2,6,10) → hexad pattern-1 cursor: 1 → 2 → 5
- family 3 (steps 3,7,11) → hexad pattern-2 cursor: 4 → 8 → 7
- family 4 (steps 4,8,12) → zero-point integration pulse at top
Explicit 12-entry table in geometryData.ts (tick 1..12):
 t1:inf@3, t2:p1@1, t3:p2@4, t4:zero, t5:inf@6, t6:p1@2, t7:p2@8, t8:zero, t9:inf@0, t10:p1@5, t11:p2@7, t12:zero.
p1 (1,2,5) + p2 (4,8,7) together cover the full hexad per 12-tick cycle — the two counter-running division-by-7 patterns. Tint cursors with thread colors A/B/C (family 1→C? no: family1=informing amber/gold; p1=cyan(A); p2=magenta(B); zero=gate-green pulse).

### Panel 3 — TetraPermute (K4)
4 vertices = threads/interfaces V1..V4 (C1..C4); 6 edges P(i,j); 4 faces = 4 triads: F123↔{1,5,9}, F124↔{2,6,10}, F134↔{3,7,11}, F234↔{4,8,12}.
Gauge orbits (Gauge Theory §4): MP1: F123→F124→F134→F234; MP2: F134→F234→F123→F124. Advance one face per tick (4-step cycle, 3 cycles per 12 ticks). At each step MP1/MP2 faces share exactly one edge — alternates 1-3, 2-4, 1-3, 2-4 → render shared edge GOLD ("entanglement of order 2" / shared fiber).
Render: SVG wireframe tetrahedron, manual 3D rotation (rAF continuous spin around Y + fixed X tilt, ~24s period) + perspective projection; MP1 active face fill cyan 22%, MP2 magenta 22%, shared edge gold glow, vertices labeled with C1..C4.

### Panel 1 — reuse EnneadRotor as-is (already tick-synced).

### Page assembly
Route /geometry in App.tsx; page wraps 3 panels in TickProvider with TickControls; responsive grid; station labels + short prose per panel; header with back-to-home + GEOMETRY nav link added on Home; reference strip at bottom with uploaded ng2p-3.gif + S4-Ennead-4-Steps.jpg (upload via manus-upload-file --webdev), credit Sigurd Andersen (solbakkn.com/math/n-grams.htm).
