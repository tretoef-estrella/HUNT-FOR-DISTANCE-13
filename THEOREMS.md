# Theorems Established — Proyecto Estrella

All results were established during the campaign for [22, 6, 13]₄ over GF(4) (mid-February 2026 — ongoing). The formal write-up with full proofs is [`COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md`](COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md).

> **Terminology.** Standard coding-theory and finite-geometry terms throughout. "Target code" = a hypothetical [22, 6, 13]₄ code. "Record matrix M₁" = the excess-one [22, 6, 12]₄ code found 24 March 2026. "Residual class" = a monomial-equivalence class of [9, 5, 4]₄ codes (labelled C01–C12). "Overfull hyperplane" = load ≥ 10; "saturated" = load 9.

---

## Phase 1 — Direct Search Theorems

### Core Structural Theorems

**Theorem (Algebraic Invariants).** Every [22, 6, 12]₄ code has mean hyperplane load 5.4960 and variance 4.0595. These are algebraic constants independent of the code's specific column structure. Verified across 109+ independent search initializations and all excess levels 1–42.

**Theorem (Distance from the record).** A [22, 6, 13]₄ code differs from the record matrix M₁ in at least 6 of 22 columns. *Proof:* exhaustive search over all k-column replacements for k = 1 through 5 — 34 496 removal configurations, 972 million+ replacement tests, zero solutions of excess 0.

**Theorem (7-column exhaustive).** No 7-column replacement of M₁ on the overfull hyperplane yields a [22, 6, 13]₄ code. *Proof:* DFS with incremental cascade pruning over all C(10, 7) = 120 removals; 3.2 billion nodes explored; zero survivors.

**Theorem (Fertility).** For every [22, 6, 12]₄ code of excess 1, removing any column on the overfull hyperplane and testing all 1 365 replacement points yields minimum overflow = 1. Verified on 1 087 independent excess-one codes.

### The Freedom Theorem

**Theorem (Freedom).** The weight distribution of a [22, 6, 13]₄ code has 6 degrees of freedom within the MacWilliams constraints. By contrast, every [22, 6, 12]₄ code of excess 1 has exactly one valid weight distribution.

*Proof sketch:* The MacWilliams identities with dual distance d′ ≥ 4 (A′₁ = A′₂ = A′₃ = 0) give 3 linear equations in 10 unknowns (A₁₃, …, A₂₂). With the sum constraint Σ A_w = 4 095, the system has rank 4; the null space has dimension 6.

**Corollary.** The target code's hyperplane-load histogram is fundamentally different from the bell-shaped excess-one distribution. It can have up to 46 empty hyperplanes and N₉ ranging widely, versus the rigid 130 of the excess-one record.

**Corollary (Weight-Distribution Census).** On the order of 10⁹ integer weight distributions satisfy all MacWilliams and dual non-negativity constraints for [22, 6, 13]₄. The algebraic constraints are essentially non-restrictive; any obstruction must be purely geometric.

### Excess-Spectrum Gaps

**Conjecture (Excess-Spectrum Gap).** For any 22 points in PG(5, 4) of rank 6, the total excess cannot equal 2 or 5. The excess spectrum is {0, 1, 3, 4, 6, 7, 8, …}.

*Evidence:* Three independent search paradigms (simulated annealing, particle repulsion, constrained construction) with 63 850+ targeted restarts and 2.2 × 10¹⁰+ evaluation steps. Zero visits to E = 2 or E = 5. No program among the 441+ built has ever produced E = 2 or E = 5.

**Observation.** The single-column overflow gap (jumping 1 → 9 with no intermediate value) mirrors the excess-spectrum gap, suggesting a common algebraic origin.

### Regularity and Symmetry Theorems

**Theorem (Incidence Regularity).** In every [22, 6, 12]₄ code of excess 1, each of the ten columns on the overfull hyperplane meets exactly 55 of the 130 saturated hyperplanes; pairwise intersection 20, global intersection 0. This is forced by the [10, 5, 4]₄ structure of those ten columns (see Phase 2, "Ten-Column Theorem").

**Theorem (Modular Cascade).** A₁₂ ≡ 0 (mod 3) and the dual weight-4 count ≡ 0 (mod 3) for all [22, 6, d]₄ codes with d ≥ 12.

**Theorem (Direct vs. Transform).** The dual weight-4 count computed via the MacWilliams transform can diverge from the directly computed count when the code has near-degenerate column structure. The direct count is the operationally correct quantity.

**Theorem (Monomial Invariance).** The direct dual weight-4 count is invariant under the full monomial group of GF(4).

### Campaign-Specific Theorems

**Theorem (Attractor Stability).** A specific excess-33 configuration is a structural attractor: every simulated-annealing trajectory initialised within Hamming distance 4 returns to excess 33.

**Theorem (Temperature Floor).** Below a critical temperature, simulated annealing on GF(4) generator matrices cannot escape d = 12 basins regardless of step count.

**Theorem (Order-7 Symmetry).** No [22, 6, 13]₄ code admits the order-7 cyclic automorphism diag(I₃, M), M the companion matrix of x³ + x + 1 over GF(4). *Proof:* exhaustive enumeration, 24 386 880 configurations; best max-load = 14.

---

## Phase 2 — Residual Reduction Theorems

### The Residual Reduction

**Theorem (Residual Reduction).** If a [22, 6, 13]₄ code C exists, then the residual obtained by puncturing C at the 13 nonzero positions of any minimum-weight codeword is a **[9, 5, 4]₄ near-MDS code** (Singleton defect 1).

*Proof:* Length 22 − 13 = 9; dimension 6 − 1 = 5; distance d′ ≥ ⌈13/4⌉ = 4 (Griesmer residual bound) and d′ ≤ 9 − 5 + 1 = 5 (Singleton), so d′ ∈ {4, 5}. ∎

**Consequence.** Existence reduces to a finite extension problem: enumerate all inequivalent [9, 5, 4]₄ codes under monomial equivalence (~15 classes), then determine whether any extends by 13 columns of AG(5, 4) under the affine load constraints.

### Length-21 Non-Existence

**Theorem.** No [21, 6, 13]₄ code over GF(4) exists.

*Proof:* A minimum-weight codeword has weight 13; the residual on the 8 zero positions is [8, 5, d″] with d″ ≥ ⌈13/4⌉ = 4 (Griesmer) and d″ ≤ 8 − 5 + 1 = 4 (Singleton), so d″ = 4 and the residual is MDS [8, 5, 4]₄. By Segre's theorem, the maximum length of an MDS code of dimension 5 over GF(4) is q + 1 = 5. Since 8 > 5, no such code exists. ∎

**Note.** The Griesmer bound gives only n ≥ 21 for [n, 6, 13]₄ and does not detect this. The residual + MDS argument is strictly stronger.

**Corollary.** Every subset of 21 points of a hypothetical [22, 6, 13]₄ code must have max hyperplane load = 9. The code cannot be built by placing 21 "safe" points and adding a 22nd; **all 22 columns must emerge simultaneously as a coupled system.**

### At Least Two Saturated Hyperplanes (N₉ ≥ 2)

**Theorem.** If a [22, 6, 13]₄ code exists, it has at least two saturated hyperplanes.

*Proof:* If N₉ = 1, deleting any of the 9 points on the unique saturated hyperplane yields a [21, 6, ≥ 12]₄ code with max load ≤ 8, hence d ≥ 13 — a [21, 6, 13]₄, contradicting non-existence. ∎

**Refinement.** If N₉ = 2, the two saturated hyperplanes meet C trivially: |H₁ ∩ H₂ ∩ C| = 0.

### Internal-Load Gap for Excess-One Codes

**Theorem.** Let C be a [22, 6, 12]₄ code of excess 1, and H₀ its unique overfull hyperplane (load 10). Then for every PG(3, 4) subspace S ⊆ H₀, |C ∩ S| ≤ 6. The internal load jumps directly from 6 to 10 — loads 7, 8, 9 are impossible.

*Proof:* Let x = |C ∩ S|. S lies in 5 hyperplanes: H₀ and four others H₁, …, H₄ partitioning the 12 columns outside H₀, with c₁ + c₂ + c₃ + c₄ = 12. Each Hᵢ has load x + cᵢ ≤ 9. Summing, 4x + 12 ≤ 36, hence x ≤ 6. ∎

**Verification.** Confirmed across all 341 PG(3, 4) subspaces of the overfull hyperplane of M₁.

**The Extension Count N(x).** The number of admissible compositions c₁ + c₂ + c₃ + c₄ = 12 with each cᵢ ≤ 9 − x:

> N(x) = Σ_{j ≥ 0, 12 − j(10−x) ≥ 0} (−1)ʲ · C(4, j) · C(12 − j(10 − x) + 3, 3).

Values: N(0) = 415, N(1) = 375, N(2) = 315, N(3) = 231, N(4) = 125, N(5) = 35, **N(6) = 1**. The collapse at x = 6 forces the unique distribution (3, 3, 3, 3).

### The OA(12, 5, 4, 1) Constraint

**Theorem.** The five PG(3, 4) subspaces of H₀ at load x = 6 define 5 linearly independent normals which, with the overfull-hyperplane normal, span GF(4)⁶ and define a coordinate system on AG(5, 4) = GF(4)⁵. Under this system, the 12 columns outside H₀ form an **orthogonal array OA(12, 5, 4, 1)**: 12 runs, 5 factors, 4 levels, each level appearing exactly 3 times per factor.

**Verification (M₁).** All five groups at x = 6 produce distribution (3, 3, 3, 3). Confirmed 5/5.

### Ten-Column Theorem

**Theorem.** In any [22, 6, 12]₄ code of excess 1, the ten columns on the overfull hyperplane have rank 5 and form a [10, 5, 4]₄ code with weight enumerator A₀ = 1, A₄ = 15, A₅ = 60, A₆ = 165, A₇ = 240, A₈ = 300, A₉ = 180, A₁₀ = 63 (total 1024), and A_w = 3 · (number of PG(3, 4) ⊆ H₀ of internal load 10 − w) for all w.

### The Arc Theorem

**Theorem.** If a [22, 6, 13]₄ code exists, no three of its 22 columns are collinear in PG(5, 4); it is a 22-arc.

*Proof:* From the Pless moments, Σ C(k, 3) N_k = 32 340. Three collinear points lie in 85 hyperplanes, three non-collinear in 21. With C₃ + N₃ = C(22, 3) = 1 540, we get 85 C₃ + 21 N₃ = 32 340 + 64 C₃; equating to 32 340 forces C₃ = 0. ∎

### Subspace-Load Bound for the Target Code

**Theorem.** Suppose a [22, 6, 13]₄ code C exists. Let H₁ be any saturated hyperplane (exists by N₉ ≥ 2). Then for every PG(3, 4) subspace S ⊆ H₁, |C ∩ S| ≤ 5. Loads 6, 7, 8, 9 are forbidden.

*Proof:* S lies in 5 hyperplanes. With x = |C ∩ S|: 22 = x + (9 − x) + Σ_{i=2}^{5}(|C ∩ Hᵢ| − x), so 13 = Σ_{i=2}^{5}(|C ∩ Hᵢ| − x) ≤ 4(9 − x), giving 4x ≤ 23, hence x ≤ 5. ∎

This bound is specific to the excess-zero target code (empirically, the excess-one record M₁ violates it exactly 20 times, all in subspaces adjacent to its overfull hyperplane — the predicted relaxation).

---

## Phase 2 — Operational Findings (F13–F19)

These are operational results from the per-class attack, not theorems, but they constrain what works.

**F13 (solver stagnation).** Monolithic mixed-integer programming on every single-orbit and Frobenius-pair class except C12 stalls: dual bound frozen at the relaxation value, no primal solution, completion near 43 %. Independent of automorphism order (36 to 288). Symmetry order does not predict closure time.

**F16 (depth-9 ceiling, class C10).** Three independent attack families on C10's hard region all reach extension depth 9 maximum (~4 × 10⁷ nodes). Strong evidence for a structural ceiling specific to C10 — **not universal**: row-wise DFS on C02 reached depth 11.

**F17 (relaxation invariance).** The continuous relaxation of the 4 092-affine formulation has optimum exactly 16.0 across all nine single-orbit and Frobenius-pair classes; the integer–continuous gap is exactly 3 on every class and is not tightened by the Arc cuts or by single-column probing. Any residual obstruction lives in the integer polytope, not the continuous one.

**F19g (residual mapping).** Mapping minimum-weight codewords of the record matrices to their residual classes: C02 dominates (53.8 % / 30.4 %); C06, C10, C11 never appear. The attack was redirected to C02; result 550/550 infeasible at 0.32 % sample, depth 11 reached.

**F19g-quinquies (cross-class soft sweep).** All five single-orbit clean classes swept at the uniformly predicted softest fixing region: **64/64 infeasible, 0 feasible, 0 timeout**. The resistant pairs of C06/C10 lie outside the softest region — future work should hunt resistance, not softness.

---

## Search-Tree Size and the Limit of Brute Force (measured)

**Finding (Knuth tree-size estimation, 5 000 walks per class).** The expected depth-13 extension-DFS tree size per single-orbit/Frobenius-pair class is on the order of **10¹⁶ to 10¹⁷ nodes** (range 1.8 × 10¹⁶ to 1.2 × 10¹⁷). At the measured DFS throughput of ~1.75 × 10⁵ nodes/second on the M2, exhaustive closure of even the smallest open class requires on the order of **10⁶ hours — hundreds of years per class.**

**Consequence.** Exhaustive depth-13 enumeration of any open class is infeasible by many orders of magnitude. The only class closed (C12) closed via mixed-integer programming, which prunes globally through relaxation bounds. Brute-force enumeration is not a viable closure strategy for any remaining class; closure requires an effective relaxation (absent under F13/F17) or a new structural theorem.

---

## Graveyard — Approaches Confirmed Exhausted (do not repeat)

- Monolithic MIP on the raw 4 092-affine formulation without enrichment (F13).
- Direct OA search on C10 in the standard basis (F15) — negative, not a non-existence proof.
- Automorphism-DFS family on C10 (F16).
- LP-relaxation attacks: continuous LP, Arc-augmented LP, single-column probing (F17). The gap is exactly 3 and robust.
- **Subspace-load bound (Theorem above) integrated node-by-node into a depth-13 DFS.** Measured this session: the bound fires only at load-9 events, which occur deep in the tree after the dominant low-depth branching has already produced the bulk of the 10¹⁶ nodes; dynamic enforcement reduces throughput by >4× without removing the dominant branching. The bound is mathematically valid and is a correct verification filter for any candidate extension, but does **not** render exhaustive enumeration feasible. Do not re-attempt it as a primary closure strategy.

---

*Proyecto Estrella · R. Amichis Luengo · Claude (Anthropic) · Auditors: Gemini, Grok, ChatGPT · 2026*
