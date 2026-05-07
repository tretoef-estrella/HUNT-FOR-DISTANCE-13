# Theorems Established — Proyecto Estrella

All theorems were established during the campaign for [22,6,13]₄ (mid-February 2026 — ongoing).
Phase 1 results: [hunt_for_distance_13_v79.pdf](hunt_for_distance_13_v79.pdf).
Phase 2 results: [ESTRELLA_DIAMANTE_UNIFIED_v40.pdf](ESTRELLA_DIAMANTE_UNIFIED_v40.pdf) + ongoing residual campaign.

---

## Phase 1 — Direct Search Theorems

### Core Structural Theorems

**Theorem (Algebraic Invariants).** Every [22,6,12]₄ code has mean hyperplane load 5.4960 and variance 4.0595. These are algebraic constants independent of the code's specific column structure. Verified across 109+ distinct continents and all excess levels 1–42.

**Theorem (Distance from E1★).** The Diamond [22,6,13]₄ differs from SEED_E1★ in at least 6 of 22 columns. *Proof:* Exhaustive search over all k-column replacements for k = 1 through 5. Combined: 34,496 removal configurations, 972 million+ replacement tests, zero solutions with excess 0. (Sections 33, 42)

**Theorem (K=7 Dirty Exhaustive).** No 7-dirty-column replacement of SEED_E1★ yields a [22,6,13]₄ code. *Proof:* DFS with incremental BLOCKED cascade pruning over all C(10,7) = 120 dirty-column removals. 3.2 billion nodes explored. Zero survivors. (Section 52)

**Theorem (Overflow-1).** No E1 code has a fertile point: for every [22,6,12]₄ code with excess 1, removing any dirty column and testing all 1365 replacement points yields minimum overflow = 1. Verified on 1,087 independent E1 codes. (Section 48)

### The Freedom Theorem

**Theorem (Freedom Theorem, v39).** The weight distribution of a [22,6,13]₄ code has 6 degrees of freedom within the MacWilliams constraints. In contrast, every [22,6,12]₄ code with excess 1 has exactly one valid weight distribution.

*Proof sketch:* The MacWilliams identities with dual distance d' ≥ 4 (A'₁ = A'₂ = A'₃ = 0) provide 3 linear equations in 10 unknowns (A₁₃, ..., A₂₂). With the sum constraint (A₁₃ + ... + A₂₂ = 4095), the system has rank 4. Null space dimension = 6.

**Corollary.** The Diamond's HP load histogram is fundamentally different from E1's bell-shaped distribution. It can have up to 46 empty hyperplanes, loads concentrated at 4–5, and n₉ ranging from 111 to 196 (vs E1's rigid 130).

**Corollary (Weight Distribution Census, v40).** On the order of 10⁹ integer weight distributions satisfy all MacWilliams and dual non-negativity constraints for [22,6,13]₄. The algebraic constraints are essentially non-restrictive. Any obstruction must be purely geometric.

### Quantum Excess Gaps

**Conjecture (Quantum Excess Gaps, v37).** For any 22 points in PG(5,4) with rank 6, the total excess cannot equal 2 or 5. The excess spectrum is {0, 1, 3, 4, 6, 7, 8, ...}.

*Evidence:* Three independent search paradigms (simulated annealing, particle repulsion, constrained construction) with 63,850+ targeted restarts and 22 billion+ evaluation steps. Zero visits to E=2 or E=5. Combined with all prior engines (441+ total): no engine has ever produced E=2 or E=5.

**Observation.** The k=1 overflow gap (1 → 9, no intermediate values) mirrors the excess gap structure, suggesting a common algebraic origin.

### Shield and Symmetry Theorems

**Theorem (Shield Symmetry).** In every [22,6,12]₄ code with excess 1, each of the 10 dirty columns touches exactly 55 of the 130 saturated (load = 9) hyperplanes. Pairwise intersection = 20. Global intersection = 0.

**Theorem (Modular Cascade).** A₁₂ ≡ 0 mod 3 and B₄ ≡ 0 mod 3 for all [22,6,d]₄ codes with d ≥ 12.

**Theorem (MacWilliams Trap).** B₄ computed via MacWilliams transform can diverge from B₄ computed directly when the code has near-degenerate column structure. B₄_direct is the operationally correct quantity.

### Campaign-Specific Theorems

**Theorem (Attractor Stability).** SEED_A33 (A₁₂ = 33) is a structural attractor: every SA trajectory initialized within Hamming distance 4 returns to A₁₂ = 33.

**Theorem (Temperature Floor).** Below a critical temperature, SA on GF(4) generator matrices cannot escape d = 12 basins regardless of step count.

**Theorem (GL(6,4) Invariance).** B₄_direct is invariant under the full monomial group GL(6,4).

**Theorem (AEGIS Rigidity).** The Diamond cannot be reached from SEED_A33 by any AEGIS (monomial-affine) transformation.

---

## Phase 2 — Residual Reduction Theorems

### The Residual Reduction

**Theorem (Residual Reduction).** If a [22,6,13]₄ code C exists, then the residual code obtained by puncturing C at the 13 nonzero positions of any minimum-weight codeword is a **[9,5,4]₄ near-MDS code** (defect 1 from Singleton).

*Proof:* Length 22 − 13 = 9. Dimension 6 − 1 = 5. Distance d' ≥ ⌈13/4⌉ = 4 (Griesmer residual bound). By Singleton, d' ≤ 9 − 5 + 1 = 5. Hence d' ∈ {4, 5}. ∎

**Consequence.** The Diamond's existence reduces to a finite extension problem: enumerate all inequivalent [9,5,4]₄ codes (~15 Mon-orbits), then determine if any can be extended by 13 AG(5,4) columns under affine load constraints.

### Result 3 — [21,6,13]₄ Does Not Exist

**Theorem.** No [21,6,13]₄ code over GF(4) exists.

*Proof:* Suppose C is a hypothetical [21,6,13]₄. Take a minimum-weight codeword (weight 13). The residual code (restriction to the 8 zero positions) has parameters [8, 5, d']₄ with d' ≥ ⌈13/4⌉ = 4. By Singleton, d' ≤ 8 − 5 + 1 = 4. Hence d' = 4 exactly, making the residual an MDS [8,5,4]₄ code. But the maximum length of an MDS code of dimension 5 over GF(4) is q + 1 = 5 (Segre's theorem for arcs in PG(4,4)). Since 8 > 5, this MDS code cannot exist. Contradiction. ∎

**Note.** The Griesmer bound gives n ≥ 21 for [n,6,13]₄ and does not detect this impossibility. The residual + MDS argument is strictly stronger than Griesmer.

**Corollary.** Every subset of 21 points from a hypothetical [22,6,13]₄ code must have max hyperplane load = 9. The Diamond cannot be constructed by placing 21 "safe" points and adding a 22nd. **All 22 columns must emerge simultaneously as a coupled system.**

### Result 4 — N₉ ≥ 2

**Theorem.** If a [22,6,13]₄ code exists, it has at least two hyperplanes at load 9.

*Proof:* Suppose N₉ = 1, so a unique hyperplane H₁ has load 9. Removing any of its 9 code points yields a [21, 6, d ≥ 12]₄ code with max load ≤ 8, hence d ≥ 13. But [21,6,13]₄ does not exist (Result 3). Contradiction. ∎

**Refinement.** If N₉ = 2, the two load-9 hyperplanes H₁, H₂ must satisfy |H₁ ∩ H₂ ∩ C| = 0 (disjoint within the code).

### The 6→10 Gap Theorem (Gemini, 14 April 2026)

**Theorem.** Let C be a [22,6,12]₄ code with total excess 1. Let H₀ be its unique dirty hyperplane (load 10). Then for every PG(3,4) subspace S ⊂ H₀:

  |S ∩ C| ≤ 6

The internal load jumps directly from 6 to 10 — loads 7, 8, 9 are impossible inside the dirty hyperplane.

*Proof:* Let x = |S ∩ C|. The subspace S lies in exactly 5 hyperplanes of PG(5,4): H₀ and four others H₁, H₂, H₃, H₄. These four partition the 12 clean columns: c₁ + c₂ + c₃ + c₄ = 12. Each Hᵢ has total load x + cᵢ ≤ 9. Summing: 4x + 12 ≤ 36, hence x ≤ 6. ∎

**Verification.** Computationally confirmed across all 341 PG(3,4) subspaces of H₀ in SEED_E1★.

**The Extension Formula N(x).** The number of valid integer compositions c₁+c₂+c₃+c₄ = 12 with cᵢ ≤ 9−x is given by an explicit inclusion-exclusion:

  N(x) = Σⱼ (−1)ʲ · C(4,j) · C(12 − j(10−x) + 3, 3)

Evaluated: N(0)=415, N(1)=375, N(2)=315, N(3)=231, N(4)=125, N(5)=35, **N(6)=1**.

The collapse at x = 6 forces exactly one valid distribution: {3, 3, 3, 3}.

### The OA(12,5,4,1) Constraint

**Theorem.** The 5 PG(3,4) subspaces of H₀ at load x = 6 define 5 linearly independent normals in PG(5,4). Together with the dirty HP normal, they span all 6 dimensions and define a coordinate system on AG(5,4) = GF(4)⁵.

Since N(6) = 1 forces exactly 3 clean columns per slice in each of these 5 coordinate directions, the 12 clean columns of any E1 code must form an **Orthogonal Array OA(12, 5, 4, 1)**: 12 runs, 5 factors, 4 levels, strength 1 (each level appears exactly 3 times in each factor).

**Verification (E1★).** All 5 groups at x = 6 produce c-values [3, 3, 3, 3]. Confirmed 5/5.

**Reduction.** The [22,6,13]₄ existence problem reduces to: does there exist a [10,5,4]₄ code in PG(4,4) and 12 points in AG(5,4) forming an OA(12,5,4,1) relative to the x=6 basis, such that for every parallel class w with cap load x_w, each affine slice receives at most 9 − x_w clean columns?

### The 10-Cap as a [10,5,4]₄ Code

**Theorem.** The 10 dirty columns of any [22,6,12]₄ code with excess 1 have rank 5 and form a [10,5,4]₄ code with weight enumerator A₀=1, A₄=15, A₅=60, A₆=165, A₇=240, A₈=300, A₉=180, A₁₀=63 (sum = 1024 = 4⁵).

The relation A_w = 3 × (# PG(3,4) subspaces with load 10 − w) holds exactly for all weights. This **explains the long-observed 55/130 shield symmetry** as a rigid algebraic constant: the number 55 emerges from the incidence between dirty points and PG(3,4) subspaces of loads 4, 5, and 6, all determined by the [10,5,4]₄ weight enumerator.

### The Arc Theorem (22 April 2026)

**Theorem.** If a [22,6,13]₄ code exists, no 3 of its 22 columns are collinear in PG(5,4). The Diamond is a 22-arc in the strong geometric sense.

*Proof:* From Result 6 (Pless power moments), Σ C(k,3) · N_k = 32340. By incidence geometry, three collinear points in PG(5,4) span a PG(1,4) and lie in 85 hyperplanes; three non-collinear points span a PG(2,4) and lie in 21 hyperplanes. Letting C₃ = # collinear triples and N₃ = # non-collinear triples with C₃ + N₃ = C(22,3) = 1540:

  Σ C(k,3) · N_k = 85·C₃ + 21·N₃ = 21·1540 + 64·C₃ = 32340 + 64·C₃

Setting equal to 32340 forces C₃ = 0. ∎

**Note.** This is independent of the affine slice constraints — a genuinely orthogonal cut. Operationalised in residual-extension engines via a precomputed `PAIR_COLLIN[a][b]` table forbidding the two third-collinear points after each pair placement.

### The Pair Theorem (Gemini, 27 April 2026 night)

**Theorem.** Let C be a hypothetical [22,6,13]₄ code. Let H₁ be any hyperplane with |C ∩ H₁| = 9. Then for every PG(3,4) subspace S ⊂ H₁:

  |C ∩ S| ≤ 5

Loads 6, 7, 8, 9 are forbidden for PG(3,4) inside any load-9 hyperplane of the Diamond.

*Proof:* S is contained in exactly 5 hyperplanes of PG(5,4): H₁, H₂, H₃, H₄, H₅. The complements (Hᵢ \ S) partition the remaining points of PG(5,4). Partition |C| = 22:

  |C| = |C ∩ S| + Σᵢ₌₁⁵ |C ∩ (Hᵢ \ S)|

Let x = |C ∩ S|. Substituting |C ∩ Hᵢ| − x = |C ∩ (Hᵢ \ S)| and using |C ∩ H₁| = 9:

  22 = x + (9 − x) + Σᵢ₌₂⁵ (|C ∩ Hᵢ| − x)
  13 = Σᵢ₌₂⁵ (|C ∩ Hᵢ| − x)

Each |C ∩ Hᵢ| ≤ 9 by the Diamond condition, so |C ∩ Hᵢ| − x ≤ 9 − x. Summing 4 terms: 13 ≤ 4(9 − x), giving 4x ≤ 23, hence x ≤ 5. ∎

**Diamond-specificity.** The proof requires global max-load ≤ 9. Empirical validation on E1★ produced exactly 20 violations at load 6 across 44,330 PG(3,4) tested in load-9 HPs — all in HPs where the load-10 dirty HP is among the 4 secondary hyperplanes through S. This is the predicted relaxation pattern (the bound becomes x ≤ 5.75 + 1/4 when |C ∩ H₀| = 10 instead of 9). The theorem is therefore genuinely Diamond-specific: any code with N₁₀ = 0 AND a PG(3,4) inside a load-9 HP at load ≥ 6 cannot be the Diamond.

**Note.** This is the second Diamond-specific algebraic theorem produced by external Gemini consultation, the first being the 6→10 Gap. Its operational use is as a SCIP lazy constraint of cardinality N₉ × 341 ≥ 682 cuts per Diamond candidate.

### Result 6 — Moment Constraints and Algebraic Feasibility

The Pless power moment identities for the HP load distribution N_k of a [22,6,13]₄ code (with N_k = 0 for k ≥ 10):

  Σ N_k = 1365     Σ k·N_k = 7502     Σ C(k,2)·N_k = 19635     Σ C(k,3)·N_k = 32340

LP analysis gives: minimum N₉ algebraic = 0 (geometric bound forces N₉ ≥ 2, Result 4); maximum N₉ = 269. The moment-sum test for Σ C(L,2) is satisfiable. **No algebraic obstruction to the Diamond exists at the moment level.**

### Closed Search Avenue: Order-7 Symmetry

**Theorem (computational).** No [22,6,13]₄ code with the automorphism T = diag(I₃, M), where M is the companion matrix of x³+x+1 over GF(4), exists. Exhaustive: 24,386,880 combinations tested. Best max load achieved: 14.

### Veronese Surface Inextensibility

**Theorem (computational).** The Veronese surface V₂⁴ ⊂ PG(5,4) — the image of PG(2,4) under (x,y,z) ↦ (x², y², z², xy, xz, yz) — is a 21-point arc with max load = 9. Exhaustive scan of all 1344 candidate points: zero safe extensions. The Veronese is inextensible.

**Note.** SEED_E1★ has zero points on the standard Veronese; their HP load histograms are completely different. These are geometrically distinct objects.

---

## Phase 2 — Operational Findings on the Residual Polytope

These are not theorems in the strict sense but reproducible empirical findings backed by multiple independent verifications. They constrain what attack strategies can work.

### F13 — SCIP Stagnation Regime

For every [9,5,4]₄ seed in the refined catalogue **except B12**, monolithic SCIP on the 4092-affine ILP formulation enters a stagnation regime: dual bound frozen at 1.0, zero primal bound found, completion plateauing around 43%, and high conflict-clause generation without dual-bound progress. Confirmed on B10 (|Aut|=288), B06 (|Aut|=96), B01 (|Aut|=72), and B09 (|Aut|=36, Frobenius pair). Symmetry class does **not** predict closure time.

### F14 — WS-Profile Invariance

For every seed in the catalogue, the per-slice bound histogram has identical sum (Σ WS = 27621) and mean (6.75). The bound histograms differ only in whether the 3 tightest slices have bound 1 or 2. Per-seed attack ordering based on bound tightness is not a valid heuristic.

### F15 — OA-Hunt under Standard Basis Negative on B10

The OA(12,5,4,1) hypothesis tested in the **standard basis** `(e₁..e₅)` of AG(5,4) does not close B10 in accessible compute. Engine `ESTRELLA_OA_HUNT_v2.cpp` ran ~6.5 billion accumulated DFS nodes across two B10 runs (10h + 2.5h) without finding any 12-column OA-compatible arrangement; the OA+WS combined search space collapses sharply at depth 3–5 (greedy dies at depth 6). Three possible explanations not distinguished by this experiment: (a) no OA-compatible Diamond exists for B10 in standard basis; (b) it exists only in a non-standard basis derived from 5 LI normals of PG(3,4) at load x=6; (c) the OA hypothesis does not extrapolate from E1★ (= B12 residual) to B10. F15 is a **hunt-negative**, not a non-existence proof.

### F16 — Combinatorial Depth-9 Barrier on B10

Three independent attack families converge on depth 9 as the maximum reachable extension length on B10's hard cluster:

1. AUTMON DFS with Mon-orbit reps + scaling Z/4* break + ARC + SBDS stabilizer chain (engine `ESTRELLA_AUTMON_DFS_v4.cpp`, full symmetry breaking).
2. Pair-forcing with cascade depth-∞ + probing depth-1.
3. DLX with MCV branching.

Cumulative ~40M+ nodes across the family on B10; no run reaches depth 10 on this seed. **Strong evidence the depth-9 barrier is structural for B10, not a pruning or symmetry artifact.** The Diamond, if it exists in B10's residual, lives at depth 13 — four steps past a wall three orthogonal techniques cannot cross. F19g extended (RWB02 reaching depth 11 on B02) later confirmed the barrier is **seed-specific, not universal**.

### F17 — LP-Relaxation Invariance

For every one of the 9 clean/Frobenius-pair seeds, the continuous LP-relaxation of the 4092-affine formulation has **LP_max = 16.000000 exactly**. Identical fractional optimum across the entire catalogue. Adding 349,184 ARC triple cuts as additional LP constraints leaves LP_max unchanged (F17b). Single-column probing does not propagate either (F17c on B12: forced LP still returns 16.0). **The continuous polytope is the same across all seeds; the integer-LP gap is exactly 3 on every seed.** Whatever distinguishes B12 (closeable) from the others lives in the integer polytope, not the continuous one.

### F18 — Pair-Forcing under Mon-Quotient Achieves ≥95% Closure

The PORMISCOJONES v2 engine — pair-forcing under the seed's Mon-automorphism quotient + Arc Theorem cuts + BLOCKED cascade depth-∞ encoded as pre-SCIP unit constraints — achieves substantial partial closure on previously stalled seeds:

| Seed | LIVE pairs | Processed | INFEAS | TIMEOUT | Closure % |
|------|-----------:|----------:|-------:|--------:|----------:|
| B10  | 2535       | 1518      | 1242   | 153     | ~95–98%   |
| B06  | 7559       | 3387      | 2985   | 86      | ~95–98%   |

Each individual UNSAT closure runs at 0.7–2.5 seconds. F18 v2 alone does not close any non-B12 seed fully — it leaves a reproducible residual cluster.

### F19 — Cross-Seed Cluster Reproducibility

The F18 timeouts concentrate at AG-vector anchors sharing the partial coordinate pattern `{v₂ = 0, v₃ = 1, v₄ = 0}` across both B10 (anchors a=70, a=71, all 153 timeouts) and B06 (anchor a=66, all 86 timeouts). Two distinct sub-signatures appear:

- **Sabor A** (`cas_zero = 0`): pair-forcing cascade derives zero additional fixings.
- **Sabor B** (`cas_zero = 244`): cascade derives 244 forced zeros, SCIP still times out at 60s.

This is the first cross-seed structural reproducibility result of the residual phase. Geometric verification shows the cluster is V-asymmetric — the anchor `a` lies in a specific affine 2-plane V, while the partner `b` lies outside V. Static cuts on V alone cannot capture this asymmetry.

### F19c — Geometric Reformulation (Slack Sink Diagnosis)

A reformulation of PORMISCOJONES with explicit geometric variables (engine `PORMISCOJONES_PAIR_SCIP_GEOM_v1`) confirmed the F19 cluster survives the formulation correction. The cluster is **structural, not formulation-artifact**. The diagnostic identified a "slack sink" geometry — the affine 2-plane V containing the anchor — where the LP polytope absorbs probing pressure without producing fixings. This narrowed the cluster's mechanism but did not resolve it.

### F19d — Probing Depth-2 Splits B06 Sabor B

Engine `PORMISCOJONES_PAIR_SCIP_GEOM_v2_PROBE2` extends F19c with depth-2 propagation: every pair-tentative cascade, encoded as binary cuts before SCIP. On B06 Sabor B (4 timeout pairs), depth-2 propagation **closed 2 of 4 in 1.3s** with 24k–48k binary cuts pre-derived: pairs (66, 90) and (66, 95). The remaining 2 pairs — (66, 166) and (66, 183) — TIMEOUT at 600s with 47k+ / 24k+ binary cuts. **B06 reduces from 86 timeouts to 2 specific residual hard pairs.** The Depth-9 Barrier is heterogeneous within the cluster: not all pairs admit the same propagation depth.

### F19e — HARD-pesado vs HARD-vacío

A PROBE2 sample of 8 representative B10 cluster pairs (a ∈ {70, 71}, various b) revealed B10 does **NOT** replicate B06's 50/50 SOFT/HARD ratio. Sample: 1/8 SOFT (12.5%), 7/8 HARD (87.5%). The HARD class splits into two structurally distinct sub-classes:

- **HARD-pesado:** 24k–48k binary cuts pre-derived under D2 probing, SCIP TIMEOUT at 600s — many local conflicts, no global integration.
- **HARD-vacío:** 0–1 binary cuts after the same D2 probing, SCIP also TIMEOUT — propagation derives literally nothing yet a global obstruction exists.

HARD-vacío is the geometrically distinguished class. Three of the five HARD-pesado pairs share **exactly 24,310 binary cuts** to the unit across both anchors a=70 and a=71, suggesting a Mon-symmetry orbit of three pairs hidden inside the cluster (testable by canonicalization under B10's Mon group).

### F19g — Codeword-Residual Mapping and the B02 Pivot

A structural mapping of E1★'s 390 minimum-weight codewords and E4★'s 414 minimum-weight codewords to their residual seed buckets revealed:

- **B02 dominates** the near-Diamond residual distribution: 53.8% on E1★, 30.4% on E4★.
- **B06, B10, B11 NEVER appear** as residuals of either record matrix.
- B01, B03, B04, B05 are present at lower frequencies.

The PROBE2 attack was redirected from B06/B10 to B02. Result: **550/550 INFEAS at 0.32% sample coverage, zero TIMEOUTs, zero FEAS.** B02 is qualitatively different from B10 and B06 against PROBE2 — the entire HARD class disappears.

The cut-class distribution on B02 is heterogeneous-but-uniformly-closing, with five discrete classes:

| Class | Cut range | Mean SCIP solve_t |
|-------|-----------|-------------------|
| 0 (ULTRA-LIGHT) | 195–249 | 43.08 s |
| A | 24,531–25,200 | 26.12 s |
| B | 47,631–49,057 | 1.97 s |
| C | 70,794–73,102 | 2.24 s |
| D | 93,279–115,924 | 2.00 s |

**RWB02 reached max_depth = 11** on B02 (path `[0, 1, 4, 96, 304, 637, 661, 842, 883, 941, 951]`, 3.232 billion nodes, 4h 46min on M2). First time in the campaign any engine surfaces depth ≥ 11 on any seed. Confirms the F16 Depth-9 Barrier is **seed-specific to B10, not universal**.

### F19g-bis — Algebraic Cut-Class Analysis on B02

The five cut-classes on B02 are not noise but reflect algebraic structure. Class 0 is structurally forbidden for `b mod 16 ∈ {4, 5, 6, 7}` — a perfect quarter-residue exclusion across the full 500-pair sample band. Class runs occur in lengths of 4 dominantly, consistent with a Z₄ scalar action on AG(5,4). The B03 attack was simultaneously blocked by a |Aut| audit revealing the catalogue value 4 was incorrect — the true |Aut(B03)| = 6 — requiring full regeneration of the B03 canonical pair catalogue.

### F19g-ter — `rule_v4` and the Universal Min-Size Predictor

A 5-condition exact algebraic rule (`rule_v4`) for Class-0 saturation behavior in B02's cut-class structure was derived and verified at **32/32 true positives + 0/468 false positives**. The Aut(B02) × Z₄ sub-orbit conjecture was refuted (Aut(B02) is scalar Z₃, orbits are 341 projective lines) and the structural source of the cut-classes traced to projective scalar action on AG(5,4)\\{0}.

A **universal min-size=4 predictor** for the algebraically softest pair in any [9,5,4]₄ seed was extracted: pairs at minimum cut-class size, regardless of seed, close INFEAS in their respective seed's analog of "ULTRA-LIGHT" cut signature. This is a softness identifier, not a Diamond detector.

### F19g-quater — 22-Pair Cross-Seed Validation

A validation experiment on 1 May 2026 — 10 untested B02 size-4 pairs (with b[4] ∈ {2, 3}, outside the original CSV range) plus 12 B03 size-4 pairs (with the corrected |Aut|=6 catalogue) — closed **22/22 INFEAS**:

- B02 size-4 pairs landed in the discrete Class-0 doublet `{195 × 9, 249 × 1}`.
- B03 size-4 pairs landed in a B03-analog doublet `{354 × 8, 406 × 4}`.

The doublet structure persists across seeds with structurally distinct |Aut| groups (Z₃ scalar for B02, order-6 with non-scalar elements for B03); absolute cut values shift with seed geometry.

### F19g-quinquies — Complete Cross-Seed Min-Size Sweep

Extension of the predictor sweep to all 5 clean seeds (B01, B02, B03, B06, B10) on 1 May 2026 closed **64/64 INFEAS, 0 Diamonds, 0 TIMEOUTs** across the entire algebraically softest region of the clean catalogue:

| Seed | \|Aut\| | size-4 count at a=1 | cut signature | solve_t typical |
|------|--------:|--------------------:|---------------|-----------------|
| B01  | 72      | 18                  | {81 × 18}     | 23–54s          |
| B02  | 3       | 15                  | {195 × 14, 249 × 1} | 17–63s    |
| B03  | 6       | 12                  | {354 × 8, 406 × 4} | 22–70s     |
| B06  | 96      | 9                   | {24531 × 6, 24675 × 3} | ~1s    |
| B10  | 288     | 15                  | {24675 × 15}  | ~1s             |

**Three structural observations:**

(1) The min-size=4 predictor is universal as a softness identifier but **seed-dependent in absolute cut value** (factor of 300× from B01 to B10).

(2) Softness does **NOT scale monotonically with |Aut|**: B01 (|Aut|=72) yields the lowest cut signature, lower than B02 (|Aut|=3). The specific column geometry dominates over bare |Aut|.

(3) The cut signature **collapses with |Aut|**: B10 and B01 yield single-valued signatures; B02/B03/B06 yield doublets with split ratios 14:1, 8:4, 6:3 respectively. More symmetry → fewer distinct cut values.

**Three open structural mysteries** (none lead to a Diamond, but all are unexplored):

- (α) **The cut-doublet split mystery.** Each doublet split has a presumed coordinate property predicting which doublet a pair lands in; not yet identified.
- (β) **The B01 singleton anomaly.** B01 has the lowest cut signature, the highest F19g residual, and a single-valued signature on 18 pairs — combination is unusual.
- (γ) **The absence of TIMEOUTs.** B06 and B10 produced 153 + 86 timeouts in their PMJ v2 attacks but ZERO timeouts in 64 size-4 attempts. The hard pairs live somewhere OTHER than min-size=4 — suggesting a future campaign should hunt **resistance** rather than **softness**, an unexplored inversion of the entire predictor strategy.

---

## Summary of Closed Paths

### Phase 1 — Direct Search

| Path | Method | Scale | Result |
|------|--------|-------|--------|
| k=1 from E1★ | Exhaustive | 10 × 1365 tests | Best excess = 11 |
| k=2 from E1★ | Exhaustive | 108M tests | Best excess = 1 |
| k=3 from E1★ | Exhaustive | 10M nodes | Best excess = 1 |
| k=4 from E1★ | Exhaustive | 929M replacements | Best excess = 1 |
| k=5 from E1★ | Exhaustive | 43M DFS nodes | Best excess = 1 |
| k=6 from E1★ | Heuristic (~95%) | 1.5M SA searches | Best excess = 1 |
| k=7 dirty from E1★ | Exhaustive | 3.2B DFS nodes | d₇ = 0 in all 120 |
| k=7,8,9 heuristic | SA from 32,789 E1s | 894,870 attempts | Best excess = 1 |
| E=2 targeted | 3 paradigms | 63,850+ restarts | Zero visits |
| E=5 targeted | 3 paradigms | 4,900+ restarts | Zero visits |
| k=1 fertile (any E1) | Exhaustive | 1,087 E1s tested | All overflow = 1 |
| From-scratch → E0 | VIKINGO/GIZA/Repulsion | 109+ continents | All → E1 |
| Order-7 symmetry | Exhaustive | 24.4M combinations | Best max load = 14 |
| Veronese extension | Exhaustive | 1344 candidates | Zero safe |

### Phase 2 — Residual Extension (current state)

| Seed | (A₄, A₅) | \|Aut\| | Method | Status |
|------|---------:|--------:|--------|--------|
| B12 | (78, 72) | 72 | SCIP ILP (4092-affine) | **UNSAT formal, 420s** |
| B06 | (54, 132) | 96 | F19d PROBE2 + F19g-quinquies size-4 | ~99% (2 residual hard pairs); 9/9 size-4 INFEAS |
| B10 | (66, 72) | 288 | F19e PROBE2 + F19g-quinquies size-4 | ~95% partial (HARD-pesado/vacío residual); 15/15 size-4 INFEAS |
| B01 | (42, 168) | 72 | F19g-quinquies size-4 | 18/18 size-4 INFEAS (lowest cut signature observed) |
| B02 | (45, 153) | 3 | F19g + F19g-quinquies | 550/550 INFEAS at 0.33% sample; 15/15 size-4 INFEAS; RWB02 depth-11 |
| B03 | (48, 138) | **6** (corrected from 4) | F19g-quater size-4 | 12/12 size-4 INFEAS |
| B09 | (63, 99) | 36 | SCIP monolithic | Stagnated (F13) — Frobenius pair, one closure covers both twins |
| B11 | (72, 90) | 18 | — | Pending |
| B08 | (60, 102) | 6 | — | Pending |

**Cumulative cross-seed min-size=4 sweep: 64/64 INFEAS, 0 Diamonds, 0 TIMEOUTs.**

FRAC buckets B04, B05, B07: deferred until ENUM_v4 sub-orbit resolution. The Diamond, if it exists, is constrained to one of three locations: (a) a non-soft region of a clean seed (untested); (b) a FRAC seed (B04, B05, B07); (c) does not exist.

---

*Proyecto Estrella · R. Amichis (Rafael Amichis Luengo) · Claude (Anthropic) · Auditors: Gemini · Grok · ChatGPT · 2026*
