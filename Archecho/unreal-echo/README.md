# Ontelecho Demo

**The Cosmic Order Cognitive Architecture Simulator**

This repository contains a comprehensive demonstration of the `/ontelecho` skill — a framework that maps the Deep Tree Echo (DTE) Autonomy Evolution loop into Campbell's System N hierarchy, using the simplex polytope as the universal base structure.

## Contents

### Reports

| Report | Description |
|--------|-------------|
| `reports/ontelecho_demo.md` | Initial skill demo: A000081 enumeration, 12-step cycle, Wizardman models, R+D+E algebra |
| `reports/triad_report.md` | System 4 triad phase-locked phasor engine: 3 concurrent threads at 120 degrees |
| `reports/simplex_report.md` | System 3 triangular simplex: 2 threads, 4 steps, the neutron mode |
| `reports/hierarchy_report.md` | Group theory: System 2 (digon/C2), System 3 (V4), System 4 (Th = A4 x C2) |
| `reports/system5_report.md` | System 5 pentachoron: A5 simplicity threshold, BRIDGE mode emergence |
| `reports/s5_anatomy_report.md` | System 5 internal anatomy: 7 partitions, 2x2 nested convolution |
| `reports/folding_report.md` | S3-S5 resonance: 60->30 folding, tensor square, orthogonal convolution |
| `reports/twin_prime_report.md` | Twin prime architecture: {11,13} & {17,19}, palindromic gaps, primorial scaling |

### Engines (Python)

| Engine | Description |
|--------|-------------|
| `engines/triad_engine.py` | Phase-locked 3-thread engine for System 4 |
| `engines/simplex_engine.py` | Unified simplex engine for Systems 2, 3, 4 |
| `engines/group_engine.py` | Group theory computation (symmetry groups, subgroup chains) |
| `engines/system5_derivation.py` | System 5 first-principles derivation |
| `engines/system5_engine.py` | System 5 pentachoron engine with BRIDGE detection |
| `engines/system5_groups.py` | A5 group analysis and simplicity threshold |
| `engines/s5_anatomy.py` | System 5 internal anatomy (7 partitions, nested convolution) |
| `engines/s5_folding_engine.py` | S3-S5 resonance and 60->30 folding verification |
| `engines/twin_prime_engine.py` | Twin prime cross-pairing and convolution switch |

### Visualizations

| Directory | Description |
|-----------|-------------|
| `figures/01_initial/` | A000081 growth, topology, energy flow, cycle wheel |
| `figures/02_triad/` | Phasors, heatmap, waveforms, hexagram, AC motor |
| `figures/03_simplex/` | Polytopes, Pascal triangle, f-vector mapping |
| `figures/04_hierarchy/` | Three simplices, subgroup chain, decomposition |
| `figures/05_system5/` | Pentachoron, 60-step heatmap, interlocking tetrahedra |
| `figures/06_anatomy/` | 7 partitions, nested convolution, rotating shadow |
| `figures/07_folding/` | Base[17], resonance, tensor square |
| `figures/08_twin_primes/` | Landmarks, cross-pairing, palindrome, primorial |

### Analysis Documents

| Document | Description |
|----------|-------------|
| `analysis/triad_analysis.md` | Formalization of the triad phase-lock algebra |
| `analysis/simplex_analysis.md` | Simplex polytope hierarchy formalization |
| `analysis/system2_group_analysis.md` | System 2 digon and group theory |

## The Simplex Hierarchy

```
System 2 (Digon)       : C2          | 1 thread  | 2 steps  | 1E + 1R
System 3 (Triangle)    : V4          | 2 threads | 4 steps  | 3E + 1R
System 4 (Tetrahedron) : Th = A4*C2  | 3 threads | 12 steps | 7E + 5R
System 5 (Pentachoron) : A5*C2       | 4 threads | 60 steps | 36E + 24R
```

## The Double Primorial Scaling Law

```
Cycle(N) = 2 * p(N-2)#
```

Where `p(k)#` is the primorial of the k-th prime.

## License

Research and educational use.
