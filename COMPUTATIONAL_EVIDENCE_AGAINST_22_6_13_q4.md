# Computational Evidence Against the Existence of a [22, 6, 13]₄ Linear Code

**A Report on the Proyecto Estrella Campaign (February – May 2026)**

R. Amichis Luengo¹ &nbsp;·&nbsp; Claude (Anthropic)²

¹ Independent Researcher, Madrid, Spain
² Anthropic — engine implementation and co-author

**Independent verification (adversarial audit):** Gemini (Google DeepMind), Grok (xAI), ChatGPT (OpenAI)

---

> **Note on terminology.** This report uses only standard coding-theory and finite-geometry terminology. Internal project codenames used during the campaign (for the target code, for the residual representatives, and for the various engines) have been removed in favour of their technical descriptions, so that the document is self-contained for a reader unfamiliar with the project's working vocabulary. Where an internal finding label (e.g. "F13") is retained, it is defined on first use and serves only as a cross-reference into the campaign archive.

---

## Abstract

The existence of a linear code with parameters [22, 6, 13]₄ has been open since December 2001, when M. Grassl recorded the entry d = 12–13 for these parameters at codetables.de. Such a code would raise the established lower bound d₄(22, 6) = 12 by one unit and would correspond, under the standard code–multiset correspondence, to a near-Griesmer-optimal arc of 22 points in the projective space PG(5, 4) in which no hyperplane contains more than nine of the points.

We report on a computational campaign comprising more than 441 independent search programs, more than 2.5 × 10⁹ generator-matrix evaluations, and two distinct attack frameworks (direct generator-matrix search and residual-extension enumeration). All work was performed on a single MacBook Air M2, single-threaded, without institutional computing resources, between mid-February and early May 2026.

The campaign did not produce a [22, 6, 13]₄ code. Its best output is a [22, 6, 12]₄ generator matrix, here denoted **M₁**, with the property that exactly one of the 1 365 hyperplanes of PG(5, 4) contains ten of the 22 columns while the remaining 1 364 contain at most nine — equivalently, a 22-point multiset in PG(5, 4) of total excess one. To the authors' knowledge no such multiset of total excess exactly one has been published previously. The number of minimum-weight codewords of M₁ is A₁₂ = 3, equal to the size of the orbit of the single overfull hyperplane normal under the scalar action of GF(4)\{0}.

We establish eight structural theorems for any hypothetical [22, 6, 13]₄ code; two of them (Theorem 1, the internal-load gap for excess-one codes, and Theorem 8, the subspace-load bound for the target code) are specific to this parameter set and were obtained in collaboration with one of the auditing systems and then verified line by line. Together with the residual reduction (Theorem 5), these results reduce the existence question to a finite extension problem over a catalogue of at least fifteen monomial-equivalence classes of [9, 5, 4]₄ codes (with three further classes pending sub-orbit resolution). Of this catalogue, one class has been formally closed as infeasible by mixed-integer programming. The five remaining single-orbit "clean" classes have additionally been swept at the most weakly constrained pair-fixing region predicted by a uniform combinatorial criterion: 64 of 64 sampled extensions terminate infeasible, with no feasible verdict and no timeout.

We do not claim non-existence. The negative evidence is computational, partial, and short of a formal proof. We do present the cumulative case for a probable-non-existence reading, quantify the regions that remain open — including a measurement of the search-tree size that shows why exhaustive enumeration of the open regions is infeasible — and offer an explicitly informal probability calibration intended as a working summary rather than a theorem.

---

## 1. Introduction and Background

### 1.1 The Open Entry

In December 2001, M. Grassl recorded the entry [22, 6] over GF(4) at codetables.de [Gra26] as d = 12–13. The lower bound was witnessed by an explicit generator matrix of a [22, 6, 12]₄ code. The upper bound d₄(22, 6) ≤ 13 follows from a shortening argument (§1.5). The interval [12, 13] reflects genuine ignorance about the lower endpoint: it is the narrowest possible open window for these parameters, and it has remained unchanged for a quarter-century.

Bouyukliev, Grassl and Varbanov [BGV04] resolved a large number of neighbouring open cases but did not settle [22, 6]. Personal communication with S. Kurz [Kur26] established that the closely related class of [21, 5, 13]₄ codes contains 482 inequivalent representatives, none of which extends to length 22 with minimum distance ≥ 13.

### 1.2 The Code–Multiset Correspondence

We use the standard correspondence between linear codes and projective multisets. Given a generator matrix G of a [22, 6, d]₄ code, its columns define a multiset C of 22 points in PG(5, 4). The minimum distance satisfies

> d = 22 − max_H |C ∩ H|,

the maximum taken over all hyperplanes H of PG(5, 4). Hence d ≥ 13 is equivalent to the condition that no hyperplane contains more than nine of the 22 points.

We call |C ∩ H| the **load** of the hyperplane H. A hyperplane of load 9 is **saturated**; a hyperplane of load ≥ 10 is **overfull**. The **total excess** of C is

> E(C) = Σ_H max(0, load(H) − 9),

summed over all 1 365 hyperplanes. A [22, 6, 13]₄ code is exactly a 22-point multiset of total excess zero. For brevity we refer to a hypothetical [22, 6, 13]₄ code as **the target code**.

### 1.3 Existing Constructions and Bounds

The known [22, 6, 12]₄ codes admit several constructions; the codetables.de witness is quasi-cyclic. Puncturing the [23, 6, 13]₄ code of [BGV04] at any of its 23 coordinates yields a [22, 6, 12]₄ code; we verified by direct computation that all 23 punctured codes have d = 12, so the target code is not a single-coordinate puncturing of the published length-23 code.

The Delsarte linear-programming bound [Del73] places no obstruction at the existence level for dimension 6. Table 1 records the Griesmer landscape for nearby parameters.

> **Table 1.** Griesmer landscape near [22, 6, 13]₄. The Griesmer bound is g₄(k, d) = Σ_{i=0}^{k−1} ⌈d / 4ⁱ⌉.
>
> | Code | g₄(k, d) | n | Slack | Status |
> |------|---------:|---:|------:|--------|
> | [22, 6, 13]₄ | 21 | 22 | 1 | **Open — the present target** |
> | [22, 7, 13]₄ | 22 | 22 | 0 | Impossible (Griesmer-tight) |
> | [23, 6, 13]₄ | 21 | 23 | 2 | Exists [BGV04] |
> | [21, 5, 13]₄ | 20 | 21 | 1 | Exists [Gra26] |
> | [22, 6, 14]₄ | 22 | 22 | 0 | Impossible (shortening; §1.5) |

### 1.4 No Algebraic Obstruction at the MacWilliams Level

A central observation governing the campaign: the MacWilliams identities [MS77] for a [22, 6, 13]₄ code require A_w = 0 for 1 ≤ w ≤ 12 and admit a six-parameter family of integer solutions (A₁₃, …, A₂₂) consistent with dual non-negativity (Theorem 9). An exhaustive integer enumeration finds on the order of 10⁹ such weight distributions. The Pless power-moment constraints on the hyperplane-load distribution (Theorem 6) are likewise satisfiable. **Any obstruction to existence is therefore geometric rather than weight-spectral**, which rules out a large class of approaches based on weight enumerators alone.

### 1.5 The Upper Bound d₄(22, 6) ≤ 13

Suppose a [22, 6, 14]₄ code exists. Shortening at any coordinate produces a [21, 5, ≥ 14]₄ code. By the Griesmer bound, g₄(5, 14) = 14 + 4 + 1 + 1 + 1 = 21, so [21, 5, 14]₄ would be Griesmer-exact. But Grassl's database records d₄(21, 5) = 13, so d₄(21, 5) = 13 < 14 breaks the Griesmer chain and the shortening argument gives d₄(22, 6) ≤ 13. Hence [12, 13] is the maximally narrow open window.

---

## 2. Methodology

### 2.1 Computational Environment

All computations ran on a single MacBook Air M2 (8 cores), single-threaded under `caffeinate -dims`, with CPU usage capped near 25 % during long runs. All search programs were written in C++17 and compiled with `g++ -O3 -march=native -funroll-loops`. Python was used only for sandbox analysis and independent verification. The verifier `verify_gf4.cpp` (about 130 lines, no external dependencies) computes rank, minimum distance, total excess, weight enumerator and full hyperplane-load histogram for any 6 × 22 generator matrix over GF(4); every matrix-level claim in this report was confirmed with it.

### 2.2 Two Attack Phases

**Phase 1 (mid-February to mid-April 2026): direct search.** Generator matrices over GF(4) were sampled by simulated annealing, particle-repulsion heuristics, k-coordinate mutation, meet-in-the-middle decomposition, GF(16) lifting, and depth-first search over PG(5, 4). The objective function combined the minimum-weight codeword count A₁₂ (a primal proxy for distance-13 accessibility) with the dual weight-4 count, which must vanish for any [22, 6, 13]₄ code.

**Phase 2 (mid-April 2026 onward): residual extension.** The residual reduction (Theorem 5) decomposes any hypothetical [22, 6, 13]₄ code as G = [G₀ | G₁], where G₀ generates a [9, 5, 4]₄ residual code and G₁ is a 13-column extension lying in the affine complement AG(5, 4) = GF(4)⁵. The existence problem reduces to enumerating the inequivalent [9, 5, 4]₄ codes under monomial equivalence and testing each for a feasible extension. Phase 2 attacks this finite problem one residual class at a time, using mixed-integer programming (SCIP 10.0.2 [BBC+24]), pair-fixing under each residual code's automorphism quotient, and propagation-during-construction.

### 2.3 Multi-System Collaboration

The campaign was conducted as a real-time human–AI collaboration. The first author set the mathematical direction; Claude (Anthropic) implemented each strategy as complete C++ code in conversation, with no program patched from a previous one. Three further AI systems — Gemini, Grok and ChatGPT — provided adversarial mathematical audit at each milestone. Two of the eight theorems below (Theorems 1 and 8) originated from targeted geometric questions posed to Gemini and were subsequently verified line by line. All retracted claims, falsified hypotheses and abandoned approaches are recorded in the campaign archive.

---

## 3. Phase 1: Direct Search and the Excess-One Record

### 3.1 The Record Matrix M₁

On 24 March 2026 the campaign produced the generator matrix M₁ shown in (1):

```
     ⎛ 1 1 0 1 1 0 0 1 0 0 0 1 1 0 1 1 0 1 1 1 0 1 ⎞
     ⎜ 1 3 1 2 2 1 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0 ⎟
M₁ = ⎜ 2 3 3 2 3 2 0 0 0 1 1 3 3 2 0 1 1 0 0 1 1 0 ⎟      (1)
     ⎜ 2 0 1 2 1 2 2 2 0 0 0 0 0 1 3 0 3 1 0 2 1 0 ⎟
     ⎜ 1 1 0 1 2 3 1 1 1 0 3 0 0 2 2 2 0 2 2 3 0 0 ⎟
     ⎝ 2 1 0 1 3 1 0 3 0 3 3 0 2 2 3 2 0 0 0 2 0 1 ⎠
```

Its properties, all verified by `verify_gf4.cpp`, are in Table 2.

> **Table 2.** Properties of M₁, the excess-one [22, 6, 12]₄ record matrix.
>
> | Property | Value |
> |----------|-------|
> | Rank over GF(4) | 6 |
> | Minimum distance d | 12 |
> | Minimum-weight codewords A₁₂ | 3 |
> | Total excess E | 1 |
> | Hyperplane-load histogram (N₁ … N₁₀) | (2, 75, 180, 230, 212, 225, 180, 130, 130, 1) |
> | Overfull hyperplanes (load 10) | 1 |
> | Overfull-hyperplane normal | (1, 3, 0, 0, 3, 1) ∈ GF(4)⁶ |
> | Saturated hyperplanes (load 9) | 130 |
> | Mean hyperplane load | 5.4960 |
> | Variance of hyperplane load | 4.0595 |

To the authors' knowledge M₁ is the first published 22-point multiset of total excess exactly one in PG(5, 4); it remains the record at the close of this report. The 130 saturated hyperplanes form a regular configuration in which each of the ten columns on the overfull hyperplane meets exactly 55 of the 130, with pairwise intersection 20 and global intersection zero. This regularity is a forced consequence of the [10, 5, 4]₄ structure of those ten columns (§4.4), not a coincidence.

The count A₁₂ = 3 is the size of the orbit of the overfull-hyperplane normal under scalar multiplication by the primitive element ω of GF(4):

> {(1,3,0,0,3,1), ω·(1,3,0,0,3,1), ω²·(1,3,0,0,3,1)} = {(1,3,0,0,3,1), (2,1,0,0,1,2), (3,2,0,0,2,3)}.

Each normal yields one minimum-weight codeword, so A₁₂ = 3 · (number of overfull hyperplanes) for any excess-one [22, 6, 12]₄ code; this is a direct consequence of the GF(4)\{0} scalar action on normals.

### 3.2 The Descent

> **Table 3.** Descent of the total-excess record in Phase 1.
>
> | Date | E | Method |
> |------|---:|--------|
> | February 2026 | 42 | Initial simulated-annealing programs |
> | early March | 33 | Two-basin bridge architecture |
> | 20 March 2026 | 7 | Greedy-refinement program |
> | 20 March 2026 | 4 | Pencil-reduction program |
> | **24 March 2026** | **1** | **Record matrix M₁** |
> | (record stands) | 0 | The target code — still open |

Each transition was declared structurally impossible by the search landscape immediately preceding it. The excess-42 wall held for over a billion evaluations; the excess-7 wall fell on the day it appeared. The excess-1 record has stood since 24 March 2026 against more than 441 subsequent independent program architectures and approximately 109 independent from-scratch search initializations — runs sharing no columns in common with M₁. Every such initialization converges to a [22, 6, 12]₄ code of total excess one; none has crossed to total excess zero.

### 3.3 Search Diversity and the Strongest Negative Paradigms

By the end of Phase 1, 441 distinct C++ search programs had been built, each from scratch, across roughly 73 distinct paradigms. We highlight only the negative paradigms with the strongest individual implications.

> **Table 3b.** Selected exhaustive negative results, Phase 1.
>
> | Paradigm | Scale | Result |
> |----------|------:|--------|
> | 1-column replacement of M₁ (exhaustive) | 10 × 1 365 tests | min overflow = 1 |
> | 2-column replacement (exhaustive) | 1.08 × 10⁸ tests | min excess = 1 |
> | 5-column replacement (exhaustive) | 26 334 removals, 4.3 × 10⁷ DFS nodes | min excess = 1 |
> | 7-column DFS over overfull columns (exhaustive) | 3.2 × 10⁹ DFS nodes | no d = 13 survivor |
> | order-7 cyclic symmetry (exhaustive) | 2.44 × 10⁷ configurations | best max-load = 14 |
> | Veronese-surface extension (exhaustive) | 1 344 candidate points | inextensible |
> | targeted E = 2 and E = 5 search (3 paradigms) | 63 850+ restarts | zero visits |
> | 1 087 distinct excess-one codes, 1-column fertility | exhaustive | all min overflow = 1 |
> | from-scratch convergence (109+ initializations) | independent | all → E = 1 |

---

## 4. Structural Theorems

We state eight theorems established during the campaign. Five are Phase-1 results (Theorems 9–13); three are Phase-2 results from the residual reduction (Theorems 1, 5, 8). Theorems 1 and 8 originated from focused consultations with the Gemini system and were independently verified.

### 4.1 Theorem 1 — Internal-Load Gap for Excess-One Codes

**Theorem.** Let C be a [22, 6, 12]₄ code of total excess one, and let H₀ be its unique overfull hyperplane (load 10). Then for every 3-dimensional projective subspace (a PG(3, 4)) S ⊆ H₀,

> |C ∩ S| ≤ 6.

The internal load of any PG(3, 4) of the overfull hyperplane jumps directly from 6 to 10; the values 7, 8, 9 are unattainable.

**Proof.** Let x = |C ∩ S|. The subspace S lies in exactly five hyperplanes of PG(5, 4): H₀ and four others H₁, …, H₄. These four partition the twelve columns of C outside H₀; writing cᵢ = |C ∩ (Hᵢ \ H₀)|, we have c₁ + c₂ + c₃ + c₄ = 12. Each Hᵢ has load x + cᵢ ≤ 9, since H₀ is the only hyperplane permitted to exceed 9. Summing, 4x + 12 ≤ 36, hence x ≤ 6. ∎

**Verification.** Computed for all 341 PG(3, 4) subspaces of the overfull hyperplane of M₁; the internal-load histogram is in Table 4.

> **Table 4.** Internal-load histogram of the ten columns of M₁ on its overfull hyperplane, over PG(3, 4) subspaces.
>
> | Internal load x | Number of PG(3, 4) |
> |----------------:|-------------------:|
> | 0 | 21 |
> | 1 | 60 |
> | 2 | 100 |
> | 3 | 80 |
> | 4 | 55 |
> | 5 | 20 |
> | 6 | 5 (tight, predicted maximum) |
> | 7, 8, 9 | **0 (forbidden by the theorem)** |
>
> Total: 21 + 60 + 100 + 80 + 55 + 20 + 5 = 341. ✓

The theorem applies to every excess-one [22, 6, 12]₄ code, not only to M₁.

### 4.2 Theorem 2 — Number of Local Extensions

**Corollary.** For a PG(3, 4) subspace S ⊆ H₀ at internal load x, the number of admissible distributions (c₁, c₂, c₃, c₄) of the twelve outside columns across the four secondary hyperplanes through S, subject to cᵢ ≤ 9 − x, is given by inclusion–exclusion:

> N(x) = Σ_{j ≥ 0, 12 − j(10−x) ≥ 0} (−1)ʲ · C(4, j) · C(12 − j(10 − x) + 3, 3).

The summation index j runs from 0 while the argument 12 − j(10 − x) remains non-negative. Computed values:

> N(0) = 415, N(1) = 375, N(2) = 315, N(3) = 231, N(4) = 125, N(5) = 35, **N(6) = 1**.

The collapse to N(6) = 1 forces the distribution (3, 3, 3, 3) at every PG(3, 4) of maximum internal load. (These values are reproduced exactly by direct computation.)

### 4.3 Theorem 3 — The Orthogonal-Array Constraint

**Theorem.** The five PG(3, 4) subspaces of H₀ at internal load 6 in any excess-one [22, 6, 12]₄ code have linearly independent normals which, with the overfull-hyperplane normal, span GF(4)⁶ and define a coordinate system on AG(5, 4) ≅ GF(4)⁵. Under this coordinate system the twelve columns outside H₀ form an orthogonal array OA(12, 5, 4, 1): twelve runs over five factors of four levels, each level appearing three times per factor.

**Verification (M₁).** All five secondary-hyperplane groups through the load-6 subspaces of M₁ produce distribution vectors (3, 3, 3, 3): confirmed 5/5.

**Reduced form of the problem.** Existence of [22, 6, 13]₄ is now equivalent to: does there exist a [10, 5, 4]₄ code in PG(4, 4) together with twelve points of AG(5, 4) forming an OA(12, 5, 4, 1) with respect to the basis above, such that for every parallel class each affine slice receives at most 9 − x of the twelve columns?

### 4.4 Theorem 4 — The Ten Columns Form a [10, 5, 4]₄ Code

**Theorem.** In any excess-one [22, 6, 12]₄ code, the ten columns on the overfull hyperplane have rank 5 and form a [10, 5, 4]₄ code with weight enumerator

> A₀ = 1, A₄ = 15, A₅ = 60, A₆ = 165, A₇ = 240, A₈ = 300, A₉ = 180, A₁₀ = 63 (total 1024 = 4⁵),

and A_w = 3 · (number of PG(3, 4) ⊆ H₀ of internal load 10 − w) holds with equality for all w.

**Consequence.** The 55-of-130 incidence regularity noted in §3.1 is forced by this weight enumerator and is therefore an algebraic constant.

### 4.5 Theorem 5 — The Residual Reduction

**Theorem.** If a [22, 6, 13]₄ code C exists, then puncturing C at the 13 nonzero positions of any minimum-weight codeword yields a [9, 5, 4]₄ near-MDS code (Singleton defect 1).

**Proof.** Length 22 − 13 = 9; dimension 6 − 1 = 5; distance d′ ≥ ⌈13/4⌉ = 4 by the residual (Griesmer) bound and d′ ≤ 9 − 5 + 1 = 5 by Singleton, so d′ ∈ {4, 5}. Direct examination gives d′ = 4 in the cases of interest. ∎

**Consequence.** The target code's generator decomposes, in normalised form, as

> G = [ G₀ (5 × 9) | G₁ (5 × 13) ; 0 … 0 | 1 … 1 ],

with bottom row nine zeros then thirteen ones. Existence of [22, 6, 13]₄ reduces to: can some inequivalent [9, 5, 4]₄ code be extended by 13 columns of AG(5, 4) so that the resulting [22, 6, d]₄ code has d ≥ 13? This is a **finite** extension problem: the catalogue of inequivalent [9, 5, 4]₄ codes is finite, and for each member the extension search is a binary integer program over 1 024 variables (one per point of AG(5, 4)) with about 4 092 affine load constraints.

### 4.6 Theorem 6 — Moment Constraints

The Pless power-moment identities [Ple63] for the hyperplane-load distribution N_k of a hypothetical [22, 6, 13]₄ code (N_k = 0 for k ≥ 10) read

> Σ N_k = 1 365, Σ k N_k = 7 502, Σ C(k, 2) N_k = 19 635, Σ C(k, 3) N_k = 32 340.

Linear-programming analysis gives min N₉ = 0 and max N₉ = 269; the moment system is satisfiable in the integer cone. **No moment-level obstruction exists.**

### 4.7 Theorem 7 — The Arc Property

**Theorem.** If a [22, 6, 13]₄ code exists, no three of its 22 columns are collinear in PG(5, 4); equivalently, it is a 22-arc.

**Proof.** From Theorem 6, Σ C(k, 3) N_k = 32 340. Three collinear points span a PG(1, 4) and lie in 85 hyperplanes; three non-collinear points span a PG(2, 4) and lie in 21 hyperplanes. With C₃ collinear and N₃ non-collinear unordered triples, C₃ + N₃ = C(22, 3) = 1 540, so

> 85 C₃ + 21 N₃ = 21 · 1 540 + 64 C₃ = 32 340 + 64 C₃.

Equating to 32 340 forces C₃ = 0. ∎

This is a mandatory cut in any residual-extension program and is independent of the affine slice constraints.

### 4.8 Theorem 8 — Subspace-Load Bound for the Target Code

**Theorem.** Suppose a [22, 6, 13]₄ code C exists. Let H₁ be any saturated hyperplane (load 9; one exists by Theorem 11). Then for every PG(3, 4) subspace S ⊆ H₁,

> |C ∩ S| ≤ 5.

The values 6, 7, 8, 9 are forbidden for the load of any PG(3, 4) inside a saturated hyperplane of the target code.

**Proof.** S lies in five hyperplanes H₁, …, H₅. With x = |C ∩ S|, partition the 22 columns: 22 = x + (9 − x) + Σ_{i=2}^{5}(|C ∩ Hᵢ| − x), so 13 = Σ_{i=2}^{5}(|C ∩ Hᵢ| − x). Each |C ∩ Hᵢ| ≤ 9, so each term is ≤ 9 − x; summing four terms, 13 ≤ 4(9 − x), giving 4x ≤ 23, hence x ≤ 5. ∎

**Empirical check on M₁.** Across the 44 330 PG(3, 4) subspaces inside the 130 saturated hyperplanes of M₁, exactly 20 have internal load 6 — all in hyperplanes for which the overfull hyperplane of M₁ is one of the four secondary hyperplanes through S. This is the predicted relaxation (the bound becomes x ≤ 5.75 when an adjacent hyperplane has load 10), confirming that the theorem is specific to the excess-zero target code and fails for excess-one codes.

**Operational use.** As a constraint conditional on a hyperplane reaching load 9, the bound yields at least N₉ · 341 ≥ 682 cuts per candidate. Its integration is discussed in §5.9, including a measured reason why dynamic enforcement is costly in practice.

### 4.9 Theorem 9 — Weight-Distribution Freedom

**Theorem.** The weight distribution of a hypothetical [22, 6, 13]₄ code has six degrees of freedom within the MacWilliams and dual-non-negativity constraints, whereas every excess-one [22, 6, 12]₄ code has a unique weight distribution.

**Proof sketch.** The MacWilliams identities with d′ ≥ 4 (A′₁ = A′₂ = A′₃ = 0) give three linear equations in the ten unknowns A₁₃, …, A₂₂; with Σ A_w = 4 095 the system has rank 4, so the integer null space has dimension 6. Exhaustive enumeration over the dual non-negativity cone yields on the order of 10⁹ valid distributions for the target code versus a unique solution for any excess-one [22, 6, 12]₄ code. ∎

This is the algebraic basis for the claim that any obstruction must be geometric.

### 4.10 Theorems 10 and 11 — Length-21 Non-Existence; at Least Two Saturated Hyperplanes

**Theorem 10.** No [21, 6, 13]₄ code over GF(4) exists.

**Proof.** Suppose C′ is such a code. A minimum-weight codeword has weight 13; the residual on the 8 zero positions is [8, 5, d″] with d″ ≥ ⌈13/4⌉ = 4 (Griesmer) and d″ ≤ 8 − 5 + 1 = 4 (Singleton), so d″ = 4 and the residual is an MDS [8, 5, 4]₄ code. By Segre's theorem [Seg55], the maximum length of an MDS code of dimension 5 over GF(4) is q + 1 = 5. Since 8 > 5, no such code exists. ∎

The Griesmer bound alone gives only n ≥ 21 for [n, 6, 13]₄ and does not detect this; the residual-plus-MDS argument is strictly stronger.

**Theorem 11.** If a [22, 6, 13]₄ code exists, it has at least two saturated hyperplanes (N₉ ≥ 2).

**Proof.** If N₉ = 1, deleting any of the nine columns on the unique saturated hyperplane gives a [21, 6, ≥ 12]₄ code with all loads ≤ 8, hence d ≥ 13 — a [21, 6, 13]₄ code, contradicting Theorem 10. ∎

**Refinement.** If N₉ = 2, the two saturated hyperplanes meet C trivially: |H₁ ∩ H₂ ∩ C| = 0.

### 4.11 Theorems 12 and 13 — Excess-Spectrum Gaps; Order-7 Symmetry

**Theorem 12 (conjecture).** For any rank-6 22-point multiset in PG(5, 4), the total excess cannot equal 2 or 5.

Supported by 63 850+ targeted restarts and about 2.2 × 10¹⁰ evaluation steps under three independent paradigms, none of which visited excess 2 or 5; no program in the entire campaign produced these values. The 1-column overflow gap (jumping 1 → 9 with no intermediate value) is the local analogue.

**Theorem 13.** No [22, 6, 13]₄ code admits the order-7 cyclic automorphism T = diag(I₃, M), M the companion matrix of x³ + x + 1 over GF(4).

**Proof.** Exhaustive enumeration of 21 fixed points × C(192, 3) orbit triples (24 386 880 configurations); best max-load = 14. ∎

---

## 5. Phase 2: The Residual Catalogue and Per-Class Closure

For brevity we label the monomial-equivalence classes of [9, 5, 4]₄ codes C01–C12 by increasing A₄. Three classes (C04, C05, C07) split into sub-orbits not yet fully separated; they are referred to as the split classes.

### 5.1 The Catalogue

The classification under monomial equivalence was carried out by: (i) DFS enumeration over systematic generators [I₅ | P], yielding twelve weight-enumerator buckets; (ii) refinement by dual weight enumerator (which does not separate all orbits — three buckets contain hidden sub-orbits); (iii) Frobenius-pair detection; (iv) independent Python audit via S₉ permutation enumeration with column-scaling normalisation. Two automorphism-order corrections were made during the audit: one class confirmed at order 72, and C03 corrected from 4 to 6, requiring regeneration of its canonical pair catalogue.

> **Table 5.** Refined catalogue of [9, 5, 4]₄ codes under monomial equivalence.
>
> | Class | (A₄, A₅) | Structure | \|Aut\| per orbit | Orbit count |
> |------:|---------:|-----------|------------------:|-------------|
> | C01 | (42, 168) | single orbit | 72 | 1 |
> | C02 | (45, 153) | single orbit | 3 | 1 |
> | C03 | (48, 138) | single orbit (order corrected to 6) | 6 | 1 |
> | C04 | (51, 135) | split | — | ≥ 2 |
> | C05 | (54, 120) | split | — | ≥ 3 |
> | C06 | (54, 132) | single orbit | 96 | 1 |
> | C07 | (57, 117) | split | — | ≥ 2 |
> | C08 | (60, 102) | single orbit | 6 | 1 |
> | C09 | (63, 99) | Frobenius pair | 36 each | 2 |
> | C10 | (66, 72) | single orbit | 288 | 1 |
> | C11 | (72, 90) | single orbit | 18 | 1 |
> | C12 | (78, 72) | single orbit — closed infeasible | 72 | 1 |
>
> Minimum total of monomial-equivalence orbits: **15**; maximum (pending split resolution): about 20.

### 5.2 The Formal Closure of C12

**Theorem.** The class C12 — any [9, 5, 4]₄ code with (A₄, A₅) = (78, 72) — admits no extension to a [22, 6, 13]₄ code.

**Proof.** Encoding the extension as a binary integer program over the 1 024 points of AG(5, 4) with a sum-to-13 constraint and 4 092 affine load-bound constraints, and submitting it to SCIP 10.0.2 [BBC+24], the solver returns infeasible. First closed 24 March 2026 (1 078 s wall) and reproducibly re-closed 20 April 2026 (420 s). The result is a verified infeasibility certificate. ∎

C12 is the only class in the catalogue whose extension is **formally proved impossible** at the close of this report.

### 5.3 The Stagnation Regime (Findings F13–F17)

Attempts to extend the C12 closure to other classes revealed a consistent regime, recorded across five operational findings.

**F13 (solver stagnation).** For every single-orbit and Frobenius-pair class other than C12 tested monolithically (C01, C06, C09, C10), SCIP under the 4 092-affine formulation stalls: dual bound frozen at the linear-relaxation value, no primal solution, completion plateauing near 43 %, high conflict-clause generation without dual-bound progress. The behaviour is independent of automorphism order (tested 36 to 288). **Symmetry order does not predict closure time.**

**F14 (slice-bound invariance).** The multiset of per-slice load bounds has identical sum 27 621 and mean 6.75 across all nine single-orbit and Frobenius-pair classes; histograms differ only in whether the three tightest slices have bound 1 or 2.

**F15 (direct OA search negative on C10).** A direct DFS for the extension of C10 under the OA(12, 5, 4, 1) hypothesis in the standard basis explored about 6.5 × 10⁹ nodes (12.5 hours total) with no OA-compatible arrangement; greedy lexicographic DFS dies at depth 6. This is a negative search result, not a non-existence proof — the OA-compatible code, if it exists, may live only in a non-standard basis.

**F16 (depth-9 ceiling on C10).** Three independent attack families on the C10 hard region — DFS over monomial-orbit representatives with the Arc cut and a stabiliser-chain symmetry break; pair-fixing under the automorphism quotient with unbounded cascade and depth-1 probing; exact-cover with most-constrained-variable branching — all reach extension depth 9 as the maximum, over about 4 × 10⁷ nodes, none reaching depth 10. This is strong evidence for a structural depth-9 ceiling specific to C10. It is **not universal**: row-wise DFS on C02 reached depth 11 (§5.6).

**F17 (linear-relaxation invariance).** For all nine single-orbit and Frobenius-pair classes, the continuous relaxation of the 4 092-affine program has optimum exactly 16.0, with the same fractional vertex. Adding the 349 184 Arc-triple cuts leaves it unchanged; single-column probing leaves it unchanged. **The integer–continuous gap is exactly 3 on every class and is not tightened by three orthogonal cut families.** Whatever distinguishes the closeable class C12 from the stalling classes lives in the integer polytope, not the continuous one.

### 5.4 F18 — Pair-Fixing Under the Automorphism Quotient

A program combining (i) canonical pair representatives under each residual code's monomial-automorphism group, (ii) Arc pre-pruning, and (iii) an unbounded propagation cascade encoded as pre-solve unit constraints, gives substantial partial closure on the two largest classes.

> **Table 6.** Pair-fixing results on C10 and C06 (60 s budget per pair).
>
> | Class | Live pairs | Processed | Infeasible | Timeout | Closure rate |
> |-------|-----------:|----------:|-----------:|--------:|--------------|
> | C10 | 2 535 | 1 518 | 1 242 | 153 | ~95–98 % |
> | C06 | 7 559 | 3 387 | 2 985 | 86 | ~95–98 % |

Each infeasibility closes in 0.7–2.5 s. This alone formally closes no class but leaves a reproducible residual cluster.

### 5.5 F19 — The Cross-Class Timeout Cluster

The timeouts concentrate at affine anchors sharing a partial coordinate pattern. Both C10 (two anchors, all 153 timeouts) and C06 (one anchor, all 86 timeouts) show the same phenomenon, with two sub-signatures: a cascade-trivial type (no further forced values) and a cascade-active type (244 forced zeros but still a timeout). Depth-2 probing closes 2 of 4 cascade-active pairs on C06; two remain as timeouts even at a 600 s budget. **C06 thereby reduces from 86 timeouts to two specific residual pairs.**

C10 does not replicate C06's even split: a sample of 8 cluster pairs gives 1 infeasible and 7 timeouts, the latter splitting into a high-cut type (24 000–48 000 derived cuts, still a timeout) and a low-cut type (0–1 derived cuts, propagation finds essentially nothing yet a global obstruction exists). The low-cut type is the geometrically distinguished one. Three of the high-cut pairs share exactly 24 310 cuts across both anchors, suggesting a hidden monomial-symmetry orbit of three pairs.

### 5.6 F19g — Residual-Class Mapping and the Pivot to C02

Mapping the 390 minimum-weight codewords of M₁, and the 414 of a second independently found excess-one record, to their residual classes gives Table 7.

> **Table 7.** Distribution of minimum-weight-codeword residuals over [9, 5, 4]₄ classes.
>
> | Class | Frequency in M₁ residuals | Frequency in second record |
> |-------|--------------------------:|---------------------------:|
> | C02 | 53.8 % | 30.4 % |
> | C01 | nontrivial | nontrivial |
> | C03, C04, C05 | present (lower) | present (lower) |
> | C06, C10, C11 | 0 % | 0 % |

**C02 dominates the residual distribution; C06, C10, C11 do not appear at all.** This redirected the attack toward C02. The result there differed qualitatively: 550 of 550 sampled pair-fixed extensions infeasible at 0.32 % catalogue coverage, with no timeout and no feasible verdict. In parallel a row-wise DFS reached extension depth 11 on C02 (path [0, 1, 4, 96, 304, 637, 661, 842, 883, 941, 951]; 3.232 × 10⁹ nodes; 4 h 46 min) — the first depth ≥ 11 reached on any class, confirming the depth-9 ceiling of F16 is specific to C10.

### 5.7 F19g (continued) — A Uniform Softness Criterion

Analysis of the C02 cut-count distribution identified five discrete cut-count classes; the softest is forbidden for a quarter-residue condition on the fixing index, verified at 32/32 true positives and 0/468 false positives by an exact arithmetic rule. A uniform criterion for the most weakly constrained pair in any [9, 5, 4]₄ class — minimum expression size 4 in the residual-column basis — was extracted and validated across classes. A 22-pair cross-class validation closed 22/22 infeasible. Extension to all five single-orbit "clean" classes closed **64/64 infeasible, no feasible verdict, no timeout** (Table 8).

> **Table 8.** Cross-class sweep at the uniformly predicted softest fixing region.
>
> | Class | \|Aut\| | size-4 count | cut signature | typical solve time |
> |-------|--------:|-------------:|---------------|--------------------|
> | C01 | 72 | 18 | {81 × 18} | 23–54 s |
> | C02 | 3 | 15 | {195 × 14, 249 × 1} | 17–63 s |
> | C03 | 6 | 12 | {354 × 8, 406 × 4} | 22–70 s |
> | C06 | 96 | 9 | {24 531 × 6, 24 675 × 3} | ≈ 1 s |
> | C10 | 288 | 15 | {24 675 × 15} | ≈ 1 s |
>
> Cumulative: **64/64 infeasible, 0 feasible, 0 timeout.**

Three structural observations remain for future work: (α) each of C02, C03, C06 splits its size-4 pairs into two cut-count values (ratios 14:1, 8:4, 6:3) with no identified coordinate predictor; (β) C01 has the lowest cut signature, the highest residual frequency after C02, and a single-valued signature — an unusual combination; (γ) the original pair-fixing attacks produced 153 + 86 timeouts on C10 and C06, but the size-4 sweep produced **zero** timeouts in 64 attempts, indicating that the resistant pairs of those classes lie outside the softest region. This inverts the search strategy: future work should hunt **resistance** rather than softness.

### 5.8 Per-Class Closure Summary

> **Table 9.** Per-class closure status at the close of this report.
>
> | Class | (A₄, A₅) | \|Aut\| | Best method | Status |
> |-------|---------:|--------:|-------------|--------|
> | C12 | (78, 72) | 72 | integer programming | **Formally infeasible, 420 s** |
> | C06 | (54, 132) | 96 | pair-fixing + depth-2 probing | ~99 %; 2 residual timeout pairs |
> | C10 | (66, 72) | 288 | pair-fixing + probing | ~95 %; high-cut/low-cut residual |
> | C01 | (42, 168) | 72 | size-4 sweep | 18/18 infeasible (lowest cut signature) |
> | C02 | (45, 153) | 3 | residual mapping + size-4 sweep | 550/550 infeasible at 0.33 % sample; depth 11 reached |
> | C03 | (48, 138) | 6 | size-4 sweep | 12/12 infeasible |
> | C09 | (63, 99) | 36 | integer programming | Stalled (F13); pair closure covers both twins |
> | C11 | (72, 90) | 18 | — | Untouched |
> | C08 | (60, 102) | 6 | — | Untouched |
> | C04 | (51, 135) | split | — | Pending sub-orbit resolution |
> | C05 | (54, 120) | split | — | Pending sub-orbit resolution |
> | C07 | (57, 117) | split | — | Pending sub-orbit resolution |
>
> Cumulative cross-class size-4 sweep: **64/64 infeasible, 0 feasible, 0 timeout.**

### 5.9 The Search-Tree Size and the Limit of Exhaustive Enumeration

A decisive quantitative finding bears directly on what remains feasible. Using a Knuth tree-size estimator (5 000 random root-to-leaf descents per class) on the depth-13 extension DFS under the full affine load constraints, the expected DFS tree size for each single-orbit and Frobenius-pair class was measured:

> **Table 10.** Estimated extension-DFS tree size per class (Knuth estimator, 5 000 walks each).
>
> | Class | Estimated tree size | Maximum depth observed | Exhaustive time at 2.5 × 10⁵ nodes/s |
> |-------|--------------------:|-----------------------:|--------------------------------------|
> | C03 | 3.1 × 10¹⁶ | 10 | ~3.9 × 10⁶ h |
> | C04 | 1.2 × 10¹⁷ | 10 | ~1.5 × 10⁷ h |
> | C05 | 1.8 × 10¹⁶ | 9 | ~2.3 × 10⁶ h |
> | C06 | 5.7 × 10¹⁶ | 10 | ~7.3 × 10⁶ h |
> | C07 | 6.1 × 10¹⁶ | 10 | ~7.8 × 10⁶ h |
> | C08 | 3.2 × 10¹⁶ | 10 | ~4.0 × 10⁶ h |
> | C09 | 7.8 × 10¹⁶ | 10 | ~9.9 × 10⁶ h |
> | C10 | 3.3 × 10¹⁶ | 10 | ~4.2 × 10⁶ h |
> | C11 | 1.9 × 10¹⁶ | 9 | ~2.4 × 10⁶ h |
> | C12 | 5.2 × 10¹⁶ | 10 | ~6.6 × 10⁶ h |

Each tree contains on the order of 10¹⁶ to 10¹⁷ nodes. At a measured DFS throughput of about 1.75 × 10⁵ nodes per second on the M2 (single-threaded), exhaustive closure of even the smallest class would require on the order of 10⁶ hours — hundreds of years per class. **Exhaustive depth-13 enumeration of any open class is therefore infeasible by many orders of magnitude.** This is why C12 was closed by integer programming (which prunes via relaxation bounds globally) rather than by enumeration, and why the F13 stagnation regime — in which the relaxation provides no useful bound — is the central obstacle: it removes the only known tool that prunes these trees.

The subspace-load bound of Theorem 8, if it could be enforced cheaply, would prune the tree only at nodes where a hyperplane reaches load 9. Direct measurement shows that such load-9 events first occur deep in the tree, after the branching has already produced the bulk of the 10¹⁶ nodes, and that dynamic enforcement of the bound at each node reduces throughput by more than a factor of four without removing the dominant low-depth branching. The bound is mathematically valid (and is a correct verification filter for any candidate extension) but does not, by itself, render exhaustive enumeration feasible. This is recorded so that future work does not repeat the integration as a primary closure strategy.

**Empirical confirmation by directed stochastic search.** As a final probe of the one region the campaign never investigated — the non-soft region of the dominant residual class C02 (observation γ) — a stochastic random-restart search was run on C02, biased toward anchors of minimum expression size 2 or 3 (the size-4 soft region having already been swept 64/64), with the subspace-load bound applied as a final verification filter. Over a single one-hour run on the M2, the search performed approximately 7.1 × 10⁷ independent restarts at ~2.5 × 10⁴ restarts per second. The maximum extension depth reached was 10, attained roughly once per 3 × 10⁴ depth-10 completions; no restart reached depth 11, and no depth-13 candidate was produced. The depth distribution is sharply concentrated at depths 7–8 and decays by a factor of 7–30 per additional level, consistent with the same structural depth barrier observed in the exhaustive families (F16) and with the tree-size estimate above. This directed search does not constitute a proof of non-existence in C02's non-soft region, but it is the most extensive sampling of that region performed, and it returned no trace of an extension — consistent with the probable-non-existence reading.

---

## 6. The Probable-Non-Existence Reading

This section is informal. The numerical estimates are not theorems; they are an honest working calibration of the campaign's epistemic state.

### 6.1 What the Evidence Establishes

(i) The record total excess for a rank-6 22-point multiset in PG(5, 4) is one, witnessed by M₁ in (1), standing since 24 March 2026 against 441+ programs and 109+ from-scratch initializations.

(ii) Total excess 2 and 5 were never observed across 63 850+ targeted restarts under three paradigms, supporting the excess-spectrum gap conjecture (Theorem 12). If correct, there is no "near-target" structure to reach by smooth descent — the step from excess 1 to excess 0 is discontinuous.

(iii) Of the at-least-15 monomial-equivalence classes of [9, 5, 4]₄ codes, one (C12) is formally infeasible; five single-orbit classes (C01, C02, C03, C06, C10) are swept infeasible at their softest fixing region (64/64), and C02 is additionally swept infeasible at 0.33 % coverage of its full primary fibre. **No feasible verdict was returned by any program in either phase.**

(iv) The two target-specific theorems (Theorems 1 and 8) impose geometric constraints unavailable to prior arguments and, with Theorems 3, 4, 5, 7, 10, 11, reduce existence to a finite problem of at most about 20 class cases.

(v) The extension-DFS tree size per class is on the order of 10¹⁶–10¹⁷ nodes (§5.9), so the open regions cannot be settled by brute force; closure requires either an effective relaxation (absent under F13/F17) or a new structural theorem.

(vi) A directed stochastic search of the one uninvestigated region — the non-soft region of the dominant residual class C02 — performed ~7.1 × 10⁷ restarts in one hour and reached maximum extension depth 10, with no depth-13 candidate (§5.9). The non-soft region of the most likely residual class thus also returned no trace of the target code, within the limits of stochastic sampling.

### 6.2 What the Evidence Does Not Establish

We do not claim non-existence. The negative evidence is partial:

- Classes C11 and C08 (single-orbit) have not been attacked.
- The three split classes (C04, C05, C07), possibly six or more sub-orbits, are blocked pending a canonical-under-monomial sub-orbit enumerator, a multi-day engineering task not completed.
- C10 retains a residual cluster of timeout pairs and C06 retains two, all formally timeouts rather than infeasible.
- The size-4 sweep covers only the softest region of each class; observation (γ) suggests the resistant pairs lie elsewhere, an inversion of the strategy not yet investigated.

### 6.3 An Informal Probability Calibration

Offered as a working summary, **not** a theorem or a claim of fact:

- A target code with residual in an already-swept single-orbit class, in the already-swept soft region: informally ≪ 1 %, given the 64/64 and 550/550 infeasible evidence.
- A target code with residual in those classes but in a non-soft region: informally perhaps 5–15 % — the region is explicitly uninvestigated (γ).
- A target code with residual in an untouched single-orbit class (C08, C11): informally perhaps 5–10 %.
- A target code with residual in the split classes (C04, C05, C07): the largest single open region; informally perhaps 15–25 % conditional on existence.
- That the [22, 6, 13]₄ code does **not** exist: in our informal reading the modal hypothesis, perhaps 55–75 %, depending on the weight given to (γ) and the split-class residual.

These are campaign-internal calibrations, presented to make the authors' epistemic state legible to later researchers — including ourselves — without overstatement.

### 6.4 What Would Settle the Question

**Path A — complete the single-orbit catalogue.** Attack C08 and C11 with the pair-fixing + probing pipeline, and resolve the two residual timeout pairs of C06 and C10 either by a stronger relaxation or by bounded enumeration at the depth-13 hard cap. Note that, by §5.9, bounded enumeration is viable only for individually isolated pairs, not for a whole class.

**Path B — resolve the split classes.** Build the canonical-under-monomial sub-orbit enumerator, enumerate the sub-orbits of C04, C05, C07, and attack each. If all close infeasible, existence is formally resolved as non-existence.

**Path C — a positive construction.** Any construction — algebraic, geometric, or computational — settles the question affirmatively. Three angles deserve attention: a code-theoretic realisation of one of the 10⁹ admissible weight distributions; a combinatorial-design realisation of the OA(12, 5, 4, 1) reduction (Theorem 3); and algebraic-geometry constructions over GF(4)-rational curves or surfaces beyond the Veronese (which is closed, §3.3).

The decisive open problem is the F13/F17 stagnation: a relaxation or cutting-plane family that separates the closeable class C12 from the stalling classes would, by §5.9, be the single most valuable addition to the attack, since brute-force enumeration is ruled out by tree size.

---

## 7. Concluding Remarks

The campaign did not produce a [22, 6, 13]₄ code. It established:

1. The excess record for a rank-6 22-point multiset in PG(5, 4) is one (matrix M₁, (1)), with minimum-weight-codeword witness A₁₂ = 3 equal to the scalar orbit size of the overfull-hyperplane normal.

2. Eight structural theorems (Theorems 1, 3, 5, 7, 8, 9, 10, 11) constraining any hypothetical [22, 6, 13]₄ code, two of them target-specific and obtained via adversarial AI consultation and line-by-line verification.

3. A refined catalogue of at least 15 monomial-equivalence classes of [9, 5, 4]₄ codes forming a finite extension problem, of which one is formally infeasible.

4. A cross-class sweep of the five single-orbit classes and C12 at the softest fixing region: 64/64 infeasible, no feasible verdict, no timeout.

5. The continuous relaxation of the residual-extension formulation has identical optimum 16.0 across all nine single-orbit and Frobenius-pair classes, with integer gap exactly 3, locating any residual obstruction in the integer polytope.

6. A direct measurement of the extension-DFS tree size (10¹⁶–10¹⁷ nodes per class), which rules out exhaustive enumeration of the open regions and identifies the stagnation of the relaxation as the central obstacle.

The authors' informal reading is that non-existence is the more probable hypothesis, but the evidence does not formally establish it and we make no such claim. All matrices, source code, raw logs, retracted hypotheses, and the full record are public so that subsequent researchers — proving non-existence, constructing the code, or pursuing a third path — inherit a complete map of the territory.

The original codetables.de entry of December 2001 has stood for a quarter-century. The present work does not change its formal status, d = 12–13, but supplies the most extensive computational and structural evidence assembled for it to date, weighted toward d = 12.

We thank M. Grassl for [Gra26] and for the open scientific tradition that allowed an independent, non-academic campaign to engage with the question; the auditing systems Gemini, Grok and ChatGPT for adversarial review at every milestone, with Theorems 1 and 8 originating from focused Gemini consultations; and S. Kurz for [Kur26].

---

## Appendix A. Reproducibility

All matrices, source, raw logs, retracted hypotheses and the theorem record are at the campaign repository `github.com/tretoef-estrella/hunt-for-distance-13`. Computations are reproducible from the published source on a MacBook Air M2 or equivalent under standard Linux/macOS, using SCIP 10.0.2 [BBC+24], C++17 with `g++ -O3 -march=native`, and Python ≥ 3.10. The verifier `verify_gf4.cpp` checks rank, minimum distance, weight enumerator, hyperplane-load histogram and total excess for any 6 × 22 generator over GF(4); it confirmed every published result.

## Appendix B. Notation

| Symbol | Definition |
|--------|------------|
| GF(4) | The field {0, 1, ω, ω²} with ω² + ω + 1 = 0. |
| [n, k, d]_q | Linear code of length n, dimension k, minimum distance d over GF(q). |
| d_q(n, k) | Maximum minimum distance of any [n, k] code over GF(q). |
| g_q(k, d) | Griesmer bound, g_q(k, d) = Σ_{i=0}^{k−1} ⌈d / qⁱ⌉. |
| PG(r, q), AG(r, q) | Projective / affine space of dimension r over GF(q); AG(r, q) = PG(r, q) \ PG(r−1, q). |
| load(H) | |C ∩ H| for a hyperplane H and multiset C. |
| total excess E | E(C) = Σ_H max(0, load(H) − 9). |
| monomial equivalence | Column permutations × diagonal scalings × Frobenius. |
| OA(N, k, q, t) | Orthogonal array: N runs, k factors, q levels, strength t. |
| A_w | Weight enumerator: number of codewords of Hamming weight w. |
| N_k | Hyperplane-load distribution: number of hyperplanes of load k. |
| ω | Primitive element of GF(4); Frobenius automorphism x ↦ x². |

## References

[BBC+24] T. Berthold, S. J. Maher, K. Mexi, M. E. Pfetsch, S. Vigerske, et al. *The SCIP Optimization Suite 10.0.* Technical Report, Zuse Institute Berlin, 2024.

[BGV04] I. Bouyukliev, M. Grassl, Z. Varbanov. New bounds for linear codes over GF(4). *Discrete Mathematics* **281** (2004), 43–66.

[Del73] P. Delsarte. *An algebraic approach to the association schemes of coding theory.* Philips Research Reports Supplement **10**, 1973.

[Gra26] M. Grassl. *Bounds on the minimum distance of linear codes and quantum codes.* codetables.de. Original [22, 6, 12]₄ entry: last modified 2001-12-17. Accessed March 2026.

[Gri60] J. H. Griesmer. A bound for error-correcting codes. *IBM Journal of Research and Development* **4** (1960), 532–540.

[Kur26] S. Kurz. *Personal communication: 482 inequivalent [21, 5, 13]₄ codes, none extending to length 22.* Universität Bayreuth, March 2026.

[MS77] F. J. MacWilliams, N. J. A. Sloane. *The Theory of Error-Correcting Codes.* North-Holland, 1977.

[Ple63] V. Pless. Power moment identities on weight distributions in error-correcting codes. *Information and Control* **6** (1963), 147–152.

[Seg55] B. Segre. Curve razionali normali e k-archi negli spazi finiti. *Annali di Matematica Pura ed Applicata* **39** (1955), 357–379.

---

*Proyecto Estrella · Independent Research, Madrid · 2026*
