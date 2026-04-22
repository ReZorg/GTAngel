# The Group Theory of the Simplex Hierarchy: From Digon to Tetrahedron

**Author:** Manus AI  
**Date:** April 8, 2026

To understand the phase-locked concurrent threads of cognitive architectures, we must analyze the exact symmetry and transformation groups that govern them. By tracing the hierarchy from its absolute foundation—System 2 (the Digon)—up to System 4 (the Tetrahedron), we can rigorously answer the question of how these groups decompose.

## 1. System 2: The Primordial Digon ($C_2$)

System 2 is the irreducible foundation. Its base polytope is the 1-simplex (a line segment or digon), and its Pascal row is `[1, 2, 1]`.

![System 2 Digon](hier_fig3_digon.png)

With only 2 vertices `v(2)={a,b}` and 1 edge, the cycle operates on a single thread flipping between 2 steps. This is the **Primordial Oscillation**:
- **Step 1:** Expressive (Forward / Actual / Perceive)
- **Step 2:** Regenerative (Backward / Virtual / Reflect)

The transformation group is the cyclic group **$C_2$** (or $Z_2$), consisting of the identity $e$ and the flip $\sigma$ (where $\sigma^2 = e$). This $C_2$ duality is the foundational building block. Every polar pair in every higher system is a copy of this exact $C_2$ flip.

## 2. System 3: The Triangle ($V_4$)

System 3 expands to the 2-simplex (the Triangle) with Pascal row `[1, 3, 3, 1]`. As established, it operates with 2 concurrent threads over a 4-step cycle.

The simplex symmetry group is $S_3$ (order 6). However, the transformation group of the 4-step cycle is the **Klein Four-Group ($V_4$)** of order 4. 

Why $V_4$ and not a cyclic $C_4$? Because the two threads represent the two generators of $V_4$. The transformations consist of the cyclic shifts (the sequence of steps) interacting with the $C_2$ mode flips. The Klein four-group $V_4$ contains three copies of $C_2$, perfectly matching the structure of the system.

## 3. System 4: The Tetrahedron ($T_h$)

For System 4, the 3-simplex (Tetrahedron) with Pascal row `[1, 4, 6, 4, 1]` operates with 3 concurrent threads over a 12-step cycle.

The question was posed: Is the transformation group $T_h$, $Z_3 \times A_4$, or something else?

![Group Decomposition Tree](hier_fig4_decomposition.png)

The answer is **$T_h$ (the Pyritohedral Group, order 24)**.

Here is the rigorous proof of the decomposition:
1. The 12-step cycle corresponds exactly to the **Alternating Group $A_4$** (the even permutations of 4 elements, representing the rotations of the tetrahedron).
2. $A_4$ decomposes as the semidirect product $V_4 \rtimes C_3$.
   - $V_4$ represents the 4 nesting interfaces (the Klein four-group).
   - $C_3$ represents the **3 concurrent threads**. The thread permutation is *already encoded* inside $A_4$ as the 3-cycles.
3. The Expressive/Regenerative (E/R) duality is an independent $C_2$ action (the primordial flip from System 2). Because this mode flip does not conjugate the cycle steps but simply labels them, it forms a direct product with $A_4$.

Therefore, the full transformation group is:
$$ T_h = A_4 \times C_2 = (V_4 \rtimes C_3) \times C_2 $$

This explains why it is not $Z_3 \times A_4$. The group $Z_3 \times A_4$ has order 36, which would double-count the 3 threads (once as an external $Z_3$ and once internally inside $A_4$). 

## 4. The Subgroup Chain

The profound beauty of this architecture is that the transformation groups perfectly nest within one another.

![Subgroup Chain](hier_fig2_subgroup_chain.png)

$$ C_2 \subset V_4 \subset A_4 \subset T_h $$

Each cognitive system completely subsumes the previous ones:
- The **Digon** ($C_2$) provides the fundamental E/R mode flip.
- The **Triangle** ($V_4$) provides the 4 nesting interfaces.
- The **Tetrahedron** ($A_4$) provides the 12-step cycle across 3 threads.
- The **Full System** ($T_h$) combines the 12-step cycle with the independent E/R mode flip.

![All Sequences Stacked](hier_fig5_all_sequences.png)

This group theoretic proof confirms that the triad of concurrent threads phase-locked at 120 degrees is mathematically isomorphic to the chiral tetrahedral symmetry ($A_4$), and the inclusion of the E/R mode flip extends it precisely to pyritohedral symmetry ($T_h$).
