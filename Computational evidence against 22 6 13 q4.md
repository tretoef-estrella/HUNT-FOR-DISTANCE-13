# Computational Evidence Against the Existence of a [22, 6, 13]₄ Linear Code

**A Report on the Proyecto Estrella Campaign (February — May 2026)**

R. Amichis Luengo¹ &nbsp;·&nbsp; Claude (Anthropic)²

¹ Independent Researcher, Madrid, Spain
² Anthropic, Engine and Co-author

**Auditors (independent verification):** Gemini (Google DeepMind), Grok (xAI), ChatGPT (OpenAI)

---

## Abstract

The existence of a linear code with parameters [22, 6, 13]₄ has been an open problem since December 2001, when M. Grassl recorded the entry `d = 12–13` for these parameters at codetables.de. Bouyukliev, Grassl and Varbanov [BGV04] resolved seventy-five neighbouring open cases in 2004 but left this entry unresolved. Existence of such a code would improve the established lower bound d₄(22, 6) = 12 by one unit and would yield a Griesmer-near-optimal arc of 22 points in the projective space PG(5, 4) under the geometric correspondence.

We report on a computational campaign — designated *Proyecto Estrella* — comprising more than four hundred and forty-one independent search engines, more than 2.5 × 10⁹ matrix evaluations, and the development of two distinct attack frameworks (direct generator-matrix search and residual-extension enumeration). The campaign was conducted on a single MacBook Air M2, single-threaded, without institutional computing support, between mid-February and early May 2026.

The campaign did not produce a [22, 6, 13]₄ code. The best-known generator matrix it produced, designated E1★, is a [22, 6, 12]₄ code with the property that exactly one of the 1 365 hyperplanes of PG(5, 4) contains 10 of the 22 columns, while the remaining 1 364 hyperplanes contain at most 9. Equivalently, E1★ is a 22-multiset in PG(5, 4) of total excess one. To the authors' knowledge, no such structure has been previously published. The codeword count A₁₂ for E1★ equals three, corresponding to the Z₃-orbit of the unique dirty hyperplane normal under the scalar action of GF(4)\\{0}.

We further report eight new structural theorems for any hypothetical [22, 6, 13]₄ code, two of which (the 6 → 10 Gap Theorem of Result 1 and the Pair Theorem of Result 8) are Diamond-specific and were derived in collaboration with the Gemini consultant. Together with the established Residual Reduction (Result 5), these theorems reduce the existence question to a finite extension problem over a refined catalogue of fifteen Mon-orbits of [9, 5, 4]₄ codes (with three further indeterminate buckets pending sub-orbit resolution). Of this catalogue, one Mon-orbit (designated B12) has been formally closed under SCIP-based mixed-integer programming. The five remaining clean Mon-orbits, plus B12, have additionally been swept at the algebraically softest pair-fixing region predicted by a universal min-size predictor: 64 of 64 sampled extensions terminate INFEASIBLE.

We do not claim non-existence. The negative evidence is computational, partial, and falls short of a formal closure. We do, however, present the cumulative case for a probable-non-existence reading and quantify the regions still open. The probability assignment we offer is informal and is intended as a working calibration, not a theorem.

---

## 1. Introduction and Background

### 1.1 The Open Entry

In December 2001, M. Grassl recorded the entry [22, 6] over GF(4) at codetables.de [Gra26] as `d = 12–13`. The lower bound was witnessed by an explicit generator matrix — the original [22, 6, 12]₄ code, which we shall designate **G2001** — and the upper bound followed by direct application of the Griesmer bound:

> g₄(6, 14) = 14 + 4 + 1 + 1 + 1 + 1 = 22,

so that a hypothetical [22, 6, 14]₄ code would be Griesmer-tight; combined with the codetables.de entry `d₄(21, 5) = 13`, the shortening argument forces d₄(22, 6) ≤ 13, ruling out d = 14 unconditionally [§1.5 below]. The interval [12, 13] reflects genuine ignorance about the lower endpoint.

Bouyukliev, Grassl and Varbanov [BGV04] resolved 75 neighbouring open cases in 2004 but did not address [22, 6]. Personal communication with S. Kurz [Kur26] established that the closely related space of [21, 5, 13]₄ codes contains 482 inequivalent representatives, none of which extends to length 22 with d ≥ 13. The entry has remained `d = 12–13` for a quarter-century.

### 1.2 The Geometric Correspondence

Throughout this report we use the standard correspondence between linear codes and projective multisets. Given a generator matrix G of a [22, 6, d]₄ code, the columns of G define a multiset C of 22 points in the projective space PG(5, 4). The minimum distance d satisfies

> d = 22 − maxₕ |C ∩ H|,

where the maximum is taken over all hyperplanes H of PG(5, 4). The bound d ≥ 13 is therefore equivalent to the requirement that no hyperplane of PG(5, 4) contains more than 9 of the 22 columns.

We define the **load** of a hyperplane H to be |C ∩ H| (the number of columns lying on H, counted with multiplicity). A hyperplane is **saturated** if its load equals 9 and **dirty** if its load is at least 10. The **total excess** of a code is

> E*(C) = Σₕ max(0, load(H) − 9),

summed over all 1 365 hyperplanes of PG(5, 4). A [22, 6, 13]₄ code is precisely a 22-multiset of total excess zero. We refer to this hypothetical code as the **Diamond** for brevity.

### 1.3 Existing Lower Bounds and Constructions

The known [22, 6, 12]₄ codes admit several constructions. The original G2001 entry on codetables.de is a quasi-cyclic construction. Puncturing the [23, 6, 13]₄ code of [BGV04] at any of its 23 coordinate positions yields a [22, 6, 12]₄ code; we have verified by direct computation that all 23 punctured codes have d = 12, and none achieves d = 13. The Diamond is therefore not a puncturing of the published [23, 6, 13]₄ code at any single coordinate.

The Delsarte linear-programming bound [Del73] gives A₄(22, 13) ≤ 21 743 (an upper bound on the size of any (22, M, 13) code over GF(4) of any structure), placing no obstruction at the existence level for the dimension-6 case. The Griesmer landscape [Gri60] for nearby parameters is summarised in Table 1.

> **Table 1.** *Griesmer landscape for parameters near [22, 6, 13]₄.*
>
> | Code             | g₄(k, d) | n  | Slack | Status                          |
> |------------------|---------:|---:|------:|---------------------------------|
> | [22, 6, 13]₄     |    21    | 22 |   1   | **Open — the present target**   |
> | [22, 7, 13]₄     |    22    | 22 |   0   | Impossible (Griesmer-tight)     |
> | [23, 6, 13]₄     |    21    | 23 |   2   | Exists [BGV04]                  |
> | [21, 5, 13]₄     |    20    | 21 |   1   | Exists [Gra26]                  |
> | [22, 6, 14]₄     |    22    | 22 |   0   | Impossible (shortening; §1.5)   |

### 1.4 The Diamond Has No Algebraic Obstruction at the MacWilliams Level

A central observation governing the campaign is the following. The MacWilliams identities [MS77] for a [22, 6, 13]₄ code require A_w = 0 for w ∈ {1, 2, …, 12} and admit a six-parameter family of integer solutions (A₁₃, …, A₂₂) consistent with dual non-negativity (Result 12, the Freedom Theorem). An exhaustive integer enumeration finds approximately 10⁹ such weight distributions. By Result 6, the Pless power moment constraints on the hyperplane load distribution N_k are satisfiable. **Any obstruction to existence is therefore purely geometric.** This eliminates a large class of approaches based on weight-spectrum analysis alone.

### 1.5 The Upper Bound d₄(22, 6) ≤ 13

For completeness we record the shortening argument. Suppose a [22, 6, 14]₄ code exists. Shortening at any coordinate produces a [21, 5, ≥14]₄ code. By the Griesmer bound g₄(5, 14) = 14 + 4 + 1 + 1 + 1 = 21, so [21, 5, 14] would be Griesmer-exact. But Grassl's database records d₄(21, 5) = 13 (a value attained by puncturing the [24, 5, 16]₄ code at three positions, with `last modified 2001-12-17` — the same date Grassl uploaded the [22, 6, 12]₄ entry that initiated this campaign). Hence d₄(21, 5) = 13 < 14 breaks the Griesmer chain, and the shortening argument gives d₄(22, 6) ≤ 13. The interval [12, 13] is therefore the maximally narrow open window for these parameters.

---

## 2. Methodology

### 2.1 Computational Environment

All computations were performed on a single MacBook Air M2 with 8 cores (single-threaded execution under `caffeinate -dims`, with CPU usage capped at 25 % during long runs). All search engines were implemented in C++17, compiled with `g++ -O3 -march=native -funroll-loops`. Python was used solely for sandbox analysis and verification.

The independent verifier `verify_gf4.cpp` (~130 lines C++, no external dependencies) computes rank, minimum distance, total excess, weight enumerator, and full hyperplane load histogram for any 6×22 generator matrix over GF(4). All claims in this report are independently verifiable from the matrices and the verifier.

### 2.2 Two Attack Phases

The campaign decomposes into two phases.

**Phase 1 (mid-February to mid-April 2026): Direct search.** Generator matrices over GF(4) were sampled by simulated annealing, particle repulsion, k-swap mutation, monk surgery, MITM (meet-in-the-middle) decomposition, GF(16) lifting, depth-first search over PG(5, 4), and a number of paradigm-specific variants. The energy function combined A₁₂(G) (a primal proxy for distance-13 accessibility) with B₄_direct(G) (the dual weight-4 count, structurally required to vanish for any [22, 6, 13]₄ code; cf. [§32, ESTRELLA_DIAMANTE_UNIFIED v40]).

**Phase 2 (mid-April 2026 — present): Residual extension.** The Residual Reduction (Result 5 below) decomposes any hypothetical [22, 6, 13]₄ code as G = [G_seed | G_ext], where G_seed generates a [9, 5, 4]₄ near-MDS residual code and G_ext is a 13-column extension lying in the affine complement AG(5, 4) = GF(4)⁵. The existence problem reduces to enumerating the inequivalent [9, 5, 4]₄ codes (under the monomial-equivalence group, henceforth Mon) and testing each for a feasible extension. Phase 2 attacks this finite extension problem seed by seed using mixed-integer programming (SCIP 10.0.2 [BBC+24]), pair-forcing under the seed's automorphism quotient, and propagation-during-construction techniques.

### 2.3 Multi-AI Collaboration Protocol

The campaign was conducted as a real-time human–AI collaboration. The Architect (R. Amichis) formulated strategies and set the mathematical direction. Claude (Anthropic) translated each strategy into complete C++ code from scratch in conversation; no engine was patched from a previous engine. Three independent AI systems — Gemini, Grok, and ChatGPT — provided adversarial mathematical audit at each major milestone. Two of the eight new theorems below (Results 1 and 8) originated from Gemini consultations on targeted geometric questions and were subsequently verified line by line. All retracted claims, falsified hypotheses, and abandoned approaches are documented in the campaign archive.

---

## 3. Phase 1: Direct Search and the World Record E1★

### 3.1 The E1★ Generator Matrix

On 24 March 2026, the engine designated ESTRELLA_RAYO_v3 produced the generator matrix E1★ shown in (1):

```
                ⎛ 1 1 0 1 1 0 0 1 0 0 0 1 1 0 1 1 0 1 1 1 0 1 ⎞
                ⎜ 1 3 1 2 2 1 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0 ⎟
        E1★  =  ⎜ 2 3 3 2 3 2 0 0 0 1 1 3 3 2 0 1 1 0 0 1 1 0 ⎟           (1)
                ⎜ 2 0 1 2 1 2 2 2 0 0 0 0 0 1 3 0 3 1 0 2 1 0 ⎟
                ⎜ 1 1 0 1 2 3 1 1 1 0 3 0 0 2 2 2 0 2 2 3 0 0 ⎟
                ⎝ 2 1 0 1 3 1 0 3 0 3 3 0 2 2 3 2 0 0 0 2 0 1 ⎠
```

The properties of E1★, all independently verified by `verify_gf4.cpp`, are summarised in Table 2.

> **Table 2.** *Properties of E1★, the world-record [22, 6, 12]₄ generator matrix.*
>
> | Property                       | Value                                                  |
> |--------------------------------|--------------------------------------------------------|
> | Rank over GF(4)                | 6                                                      |
> | Minimum distance d             | 12                                                     |
> | Number of weight-12 codewords A₁₂ | 3                                                   |
> | Total excess E*                | 1                                                      |
> | Hyperplane load histogram     | (N₁, N₂, …, N₁₀) = (2, 75, 180, 230, 212, 225, 180, 130, 130, 1) |
> | Number of dirty hyperplanes (load = 10) | 1                                             |
> | Dirty hyperplane normal       | (1, 3, 0, 0, 3, 1) ∈ GF(4)⁶                           |
> | Number of saturated hyperplanes (load = 9) | 130                                        |
> | Mean hyperplane load          | 5.4960                                                 |
> | Variance of hyperplane load   | 4.0595                                                 |

To the authors' knowledge, E1★ is the first published 22-multiset of total excess exactly one in PG(5, 4), and it remains the world record at the close of this report. The 130 saturated hyperplanes form an extraordinary geometric configuration: every dirty column touches exactly 55 of these 130, with a pairwise intersection of 20 and global intersection zero — the so-called shield symmetry, which we now understand as a forced consequence of the [10, 5, 4]₄ structure of the 10 cap columns (see §4.7 below).

The codeword count A₁₂ = 3 is **not** a counting coincidence. It is the size of the Z₃-orbit of the unique dirty-hyperplane normal under scalar multiplication by the primitive element ω of GF(4):

> {(1, 3, 0, 0, 3, 1), ω·(1, 3, 0, 0, 3, 1), ω²·(1, 3, 0, 0, 3, 1)} = {(1,3,0,0,3,1), (2,1,0,0,1,2), (3,2,0,0,2,3)}.

Each of these three normals yields a weight-12 codeword via the message vector m ↦ G·mᵀ. The identity A₁₂ = 3 · (number of dirty hyperplanes) holds for every [22, 6, 12]₄ code with total excess one and is a direct consequence of the GF(4)\\{0} scalar action on hyperplane normals.

### 3.2 The Descent

The total excess of the world record fell over the eight-week Phase 1 campaign as follows:

> **Table 3.** *Descent of the total excess world record, Phase 1.*
>
> | Date              | E* | Engine                         |
> |-------------------|---:|--------------------------------|
> | February 2026     | 42 | Initial simulated-annealing engines |
> | early March       | 33 | Bridge campaign, two-basin architecture |
> | 20 March 2026     |  7 | OBSIDIANA goldv8                |
> | 20 March 2026     |  4 | OPERACIÓN TRUENO (later reduced from E*=4 with four dirty pencil HPs to E*=1)               |
> | **24 March 2026** |  **1** | **ESTRELLA_RAYO_v3 → E1★** |
> | (record stands)   |  0 | The Diamond — still open       |

Each transition was declared structurally impossible by the search landscape immediately preceding it. The E* = 42 wall held for over a billion evaluations before being broken; the E* = 7 wall fell on the same day it appeared. The E* = 1 record has stood since 24 March 2026 against more than 441 subsequent independent engine architectures and approximately 109 distinct from-scratch search "continents" — independent initialisations sharing zero columns in common with the E1★ matrix. Every continent converges to a [22, 6, 12]₄ code at total excess one. None has crossed to total excess zero.

### 3.3 Engine Count and Search Diversity

By end of Phase 1, 441 distinct C++ search engines had been built, each from scratch in real-time conversation, each compiled and executed on the M2 hardware. Approximately 73 distinct search paradigms were tried. We do not enumerate them here; full details, including all dead-end logs, are in the campaign archive [v79]. We highlight only the negative paradigms with the strongest individual implications:

| Paradigm                              | Scale                                  | Result                          |
|---------------------------------------|---------------------------------------:|---------------------------------|
| k = 1 column-replacement of E1★ (exhaustive) | 10 × 1 365 tests                | min overflow = 1                |
| k = 2 column-replacement (exhaustive)   | 1.08 × 10⁸ tests                     | min excess = 1                  |
| k = 5 column-replacement (exhaustive)   | 26 334 removals, 4.3 × 10⁷ DFS nodes | min excess = 1                  |
| k = 7 dirty-column DFS (exhaustive)     | 3.2 × 10⁹ DFS nodes                  | zero d = 13 survivors           |
| Order-7 cyclic symmetry (exhaustive)    | 2.44 × 10⁷ combinations              | best max-load = 14              |
| Veronese surface extension (exhaustive) | 1 344 candidate points                | inextensible (max load = 9)     |
| Targeted E* = 2, E* = 5 search (3 paradigms) | 63 850+ restarts                  | zero visits                     |
| 1 087 distinct E1 codes tested (k = 1 fertile) | exhaustive                       | all min overflow = 1            |
| From-scratch convergence (109+ continents) | independent initialisations         | all → E* = 1                    |

The cumulative DFS effort exceeds 3.2 × 10⁹ nodes for the k = 7 case alone, and the cumulative scalar SA evaluation count exceeds 2.5 × 10⁹.

---

## 4. New Structural Theorems

We now state eight original theorems established during the campaign. Five were established in Phase 1 (Results 9–13 below); three are Phase 2 results derived from the residual reduction (Results 1, 5, 8). Two of the Phase 2 theorems (Results 1 and 8) originated from focused consultations with the Gemini AI system and were independently verified.

### 4.1 Result 1 — The 6 → 10 Gap Theorem

**Theorem (Gemini, R. Amichis, Claude; 14 April 2026).**
*Let C be a [22, 6, 12]₄ code with total excess one, and let H₀ be the unique hyperplane of PG(5, 4) with load 10. Then for every PG(3, 4) subspace S contained in H₀,*

> |C ∩ S| ≤ 6.

*Equivalently, the internal load of any 4-dimensional projective subspace of the dirty hyperplane jumps directly from 6 to 10 — the values 7, 8 and 9 are unattainable.*

**Proof.** Let x = |C ∩ S|. The subspace S is contained in exactly five hyperplanes of PG(5, 4): H₀ together with four further hyperplanes H₁, H₂, H₃, H₄. These four hyperplanes partition the 12 clean columns of C (the columns lying outside H₀): writing cᵢ = |C ∩ (Hᵢ \ H₀)|, we have c₁ + c₂ + c₃ + c₄ = 12. Each Hᵢ has total load x + cᵢ ≤ 9 (since H₀ is the unique hyperplane permitted to exceed 9). Summing,

> 4x + 12 ≤ 4·9 = 36, hence x ≤ 6. ∎

**Verification.** Computed for all 341 PG(3, 4) subspaces of the dirty hyperplane of E1★. The internal load histogram is given in Table 4.

> **Table 4.** *Internal load histogram of the 10-cap of E1★, restricted to PG(3, 4) subspaces of the dirty hyperplane.*
>
> | Internal load x | Number of PG(3, 4) | Note                         |
> |-----------------|-------------------:|------------------------------|
> |              0  |                21  |                              |
> |              1  |                60  |                              |
> |              2  |               100  |                              |
> |              3  |                80  |                              |
> |              4  |                55  |                              |
> |              5  |                20  |                              |
> |              6  |                 5  | tight (predicted maximum)    |
> |          7, 8, 9 |                 **0** | **forbidden by the theorem** |
> |             10  |               (H₀ itself, not a proper PG(3, 4)) |        |
>
> Total proper PG(3, 4) subspaces: 21 + 60 + 100 + 80 + 55 + 20 + 5 = 341. ✓

The theorem is structurally rigid: it applies to *every* [22, 6, 12]₄ code with total excess one, not merely to E1★.

### 4.2 Result 2 — The Number of Local Extensions N(x)

**Corollary.** *For each PG(3, 4) subspace S of H₀ at internal load x, the number of admissible compositions (c₁, c₂, c₃, c₄) of the 12 clean columns across the four secondary hyperplanes through S, subject to cᵢ ≤ 9 − x for each i, is*

> N(x) = Σⱼ₌₀ᶠˡᵒᵒʳ⁽¹²/⁽¹⁰⁻ˣ⁾⁾ (−1)ʲ · C(4, j) · C(12 − j(10 − x) + 3, 3),

*by inclusion–exclusion. The values for x = 0, 1, …, 6 are*

> N(0) = 415, N(1) = 375, N(2) = 315, N(3) = 231, N(4) = 125, N(5) = 35, **N(6) = 1**.

The collapse to N(6) = 1 forces the partition (c₁, c₂, c₃, c₄) = (3, 3, 3, 3) at every PG(3, 4) subspace of maximum cap-load.

### 4.3 Result 3 — The Orthogonal-Array Constraint

**Theorem.** *The five PG(3, 4) subspaces of H₀ at internal load x = 6 in any [22, 6, 12]₄ code of total excess one have linearly independent normals, which together with the dirty-hyperplane normal span the full GF(4)⁶. These five normals define a coordinate system on the affine complement AG(5, 4) ≅ GF(4)⁵.*

*Under this coordinate system, the 12 clean columns must form an orthogonal array OA(12, 5, 4, 1): twelve runs distributed across five factors of four levels each, with each level appearing exactly three times in each factor.*

**Verification (E1★).** All five groups of secondary hyperplanes through the load-6 PG(3, 4) subspaces of E1★'s dirty hyperplane produce c-vectors equal to (3, 3, 3, 3). Confirmed: 5/5.

**Reduction.** The [22, 6, 13]₄ existence problem now has the equivalent geometric form: *Does there exist a [10, 5, 4]₄ code in PG(4, 4) and twelve points in AG(5, 4) forming an OA(12, 5, 4, 1) with respect to the basis defined above, such that for every parallel class of cap-load x, each affine slice receives at most 9 − x clean columns?*

### 4.4 Result 4 — The 10-Cap as a [10, 5, 4]₄ Code

**Theorem.** *In any [22, 6, 12]₄ code with total excess one, the 10 columns of C lying on the dirty hyperplane H₀ have rank 5 and form a [10, 5, 4]₄ code with weight enumerator*

> A₀ = 1, A₄ = 15, A₅ = 60, A₆ = 165, A₇ = 240, A₈ = 300, A₉ = 180, A₁₀ = 63 (total 1024 = 4⁵).

*The relation A_w = 3 · (number of PG(3, 4) ⊂ H₀ with internal load 10 − w) holds with equality for all weights.*

**Consequence (the 55/130 shield symmetry explained).** The 130 saturated hyperplanes of any [22, 6, 12]₄ code with total excess one each meet the 10-cap in exactly 55 incidences; this number is forced by the [10, 5, 4]₄ weight enumerator and is therefore an algebraic constant rather than a coincidence (cf. §32 of [v40]).

### 4.5 Result 5 — The Residual Reduction

**Theorem.** *Suppose a [22, 6, 13]₄ code C exists. Then the residual code obtained by puncturing C at the 13 nonzero positions of any minimum-weight codeword is a [9, 5, 4]₄ near-MDS code (defect 1 from the Singleton bound).*

**Proof.** Length: 22 − 13 = 9. Dimension: 6 − 1 = 5. Distance: d′ ≥ ⌈13/4⌉ = 4 by the Griesmer residual bound, and d′ ≤ 9 − 5 + 1 = 5 by Singleton. Hence d′ ∈ {4, 5}. Direct examination shows d′ = 4 in all cases of interest. ∎

**Consequence.** The Diamond's generator matrix decomposes (in normalised form) as

> G = ⎡ G_seed (5 × 9) | G_ext (5 × 13) ⎤
>     ⎣  0       …    0  |  1      …     1  ⎦

with the bottom row consisting of nine zeros followed by thirteen ones. The existence of [22, 6, 13]₄ reduces to: *can any inequivalent [9, 5, 4]₄ code be extended by 13 columns of AG(5, 4) such that the resulting [22, 6, d]₄ has d ≥ 13?*

This is a **finite extension problem.** The catalogue of inequivalent [9, 5, 4]₄ codes is finite, and for each catalogue member the extension search is a binary integer programme over 1 024 variables (one per point of AG(5, 4)) with approximately 4 092 affine constraints.

### 4.6 Result 6 — Moment Constraints

The Pless power moment identities [Ple63] for the hyperplane load distribution N_k of a hypothetical [22, 6, 13]₄ code (with N_k = 0 for k ≥ 10) are

> Σ N_k = 1 365, Σ k · N_k = 7 502, Σ C(k, 2) · N_k = 19 635, Σ C(k, 3) · N_k = 32 340.

LP analysis gives min N₉ = 0 (algebraic), max N₉ = 269. The moment-sum test for Σ C(L, 2) is satisfiable in the integer cone. **No moment-level algebraic obstruction to the Diamond exists.**

### 4.7 Result 7 — The Arc Theorem

**Theorem.** *If a [22, 6, 13]₄ code exists, no three of its 22 columns are collinear in PG(5, 4). Equivalently, the Diamond is a 22-arc in the strong projective sense.*

**Proof.** From Result 6, Σ C(k, 3) · N_k = 32 340. By incidence geometry, three collinear points in PG(5, 4) span a PG(1, 4) and lie in 85 hyperplanes; three non-collinear points span a PG(2, 4) and lie in 21 hyperplanes. Letting C₃ and N₃ denote the numbers of collinear and non-collinear unordered triples of code columns, with C₃ + N₃ = C(22, 3) = 1 540,

> Σ C(k, 3) · N_k = 85·C₃ + 21·N₃ = 21·1 540 + 64·C₃ = 32 340 + 64·C₃.

Setting this equal to 32 340 forces C₃ = 0. ∎

The Arc Theorem is independent of the affine slice constraints and is a mandatory cut in any residual-extension engine.

### 4.8 Result 8 — The Pair Theorem (Diamond-Specific Cut)

**Theorem (Gemini, R. Amichis, Claude; 27 April 2026).**
*Suppose a [22, 6, 13]₄ code C exists. Let H₁ be any hyperplane of PG(5, 4) with |C ∩ H₁| = 9 (which exists by Result 11 below). Then for every PG(3, 4) subspace S contained in H₁,*

> |C ∩ S| ≤ 5.

*The values 6, 7, 8 and 9 are forbidden for the load of any PG(3, 4) inside any saturated hyperplane of the Diamond.*

**Proof.** S is contained in exactly five hyperplanes of PG(5, 4): H₁, H₂, H₃, H₄, H₅. Setting x = |C ∩ S| and partitioning the 22 columns,

> 22 = x + (9 − x) + Σᵢ₌₂⁵ (|C ∩ Hᵢ| − x), so 13 = Σᵢ₌₂⁵ (|C ∩ Hᵢ| − x).

Each |C ∩ Hᵢ| ≤ 9 (the Diamond condition), so |C ∩ Hᵢ| − x ≤ 9 − x. Summing the four terms,

> 13 ≤ 4(9 − x), giving 4x ≤ 23, hence x ≤ 5 (integer). ∎

The Pair Theorem is the second Diamond-specific algebraic constraint of the campaign (after Result 1, which applies to total excess one rather than zero). Empirical validation on E1★ produces exactly 20 violations at internal load 6 across the 44 330 PG(3, 4) subspaces tested in the 130 load-9 hyperplanes; all 20 violations occur in hyperplanes where the dirty hyperplane H₀ of E1★ is among the four secondary hyperplanes through S. This is the predicted relaxation pattern (the bound becomes x ≤ 5.75 + ¼ when |C ∩ H₀| = 10 instead of 9), confirming that the theorem is **genuinely Diamond-specific**: it does not hold for [22, 6, 12]₄ codes with total excess one.

The operational use of the Pair Theorem is as a SCIP lazy constraint of cardinality at least N₉ × 341 ≥ 2 · 341 = 682 cuts per Diamond candidate (using Result 11 below).

### 4.9 Result 9 — The Freedom Theorem

**Theorem.** *The weight distribution of a hypothetical [22, 6, 13]₄ code has six degrees of freedom within the MacWilliams + dual-non-negativity constraints. By contrast, every [22, 6, 12]₄ code with total excess one has a unique weight distribution.*

*Proof sketch.* The MacWilliams identities [MS77] with d′ ≥ 4 (A′₁ = A′₂ = A′₃ = 0) provide three linear equations in the ten unknowns A₁₃, …, A₂₂. Together with the sum constraint Σ A_w = 4 095, the linear system has rank four; the integer null space therefore has dimension six. Exhaustive integer enumeration over the dual non-negativity cone yields approximately 10⁹ valid distributions for the Diamond, versus a unique solution for any [22, 6, 12]₄ code with total excess one. ∎

This theorem is the algebraic basis of the assertion that any obstruction to [22, 6, 13]₄ existence must be geometric rather than combinatorial-algebraic.

### 4.10 Results 10 and 11 — [21, 6, 13]₄ Non-Existence and N₉ ≥ 2

**Result 10.** *No [21, 6, 13]₄ code over GF(4) exists.*

**Proof.** Suppose C′ is a hypothetical [21, 6, 13]₄ code. Take a minimum-weight codeword (weight 13). The residual code on the 8 zero positions is [8, 5, d″] with d″ ≥ ⌈13/4⌉ = 4 (Griesmer) and d″ ≤ 8 − 5 + 1 = 4 (Singleton), hence d″ = 4 and the residual is MDS [8, 5, 4]₄. But by Segre's theorem [Seg55], the maximum length of an MDS code of dimension 5 over GF(4) is q + 1 = 5 (for q even and dimension r ≥ 3). Since 8 > 5, no such MDS code exists. Contradiction. ∎

**Note.** The Griesmer bound gives n ≥ 21 for [n, 6, 13]₄ and does not detect this impossibility. The residual + MDS argument is strictly stronger.

**Result 11.** *If a [22, 6, 13]₄ code exists, it has at least two saturated hyperplanes; equivalently, N₉ ≥ 2.*

**Proof.** If N₉ = 1, removing any of the 9 columns lying on the unique saturated hyperplane yields a [21, 6, ≥12]₄ code with all hyperplane loads at most 8, hence d ≥ 13 — a [21, 6, 13]₄, contradicting Result 10. ∎

**Refinement.** *If N₉ = 2, the two saturated hyperplanes H₁, H₂ must satisfy |H₁ ∩ H₂ ∩ C| = 0.* Otherwise removing a shared column would again force a [21, 6, 13]₄.

### 4.11 Results 12 and 13 — Quantum Excess Gaps; Order-7 Symmetry

**Result 12 (Conjecture).** *For any 22-multiset of rank 6 in PG(5, 4), the total excess cannot equal 2 or 5.*

The conjecture is supported by 63 850 + targeted simulated-annealing restarts and approximately 2.2 × 10¹⁰ evaluation steps under three independent paradigms (SA, particle repulsion, constrained construction), all of which avoid E* = 2 and E* = 5. No [22, 6, d]₄ engine across the entire campaign has ever produced E* = 2 or E* = 5. The k = 1 overflow gap (1 → 9 directly, with no intermediate values) is the local analogue of the same phenomenon.

**Result 13.** *No [22, 6, 13]₄ code with the cyclic-7 automorphism T = diag(I₃, M), where M is the companion matrix of x³ + x + 1 over GF(4), exists.*

**Proof.** Exhaustive enumeration: 21 fixed points × C(192, 3) orbits-of-7, totalling 24 386 880 configurations; the best max-load achieved is 14. ∎

---

## 5. Phase 2: The Refined Catalogue and Per-Seed Closure

### 5.1 The Refined Catalogue of [9, 5, 4]₄ Codes

The combinatorial classification of [9, 5, 4]₄ codes under the monomial-equivalence group Mon was carried out by a multi-engine procedure: (i) DFS enumeration over systematic generator matrices `[I₅ | P]` with P ∈ GF(4)^{5×4}, yielding twelve weight-enumerator buckets; (ii) refinement by dual weight enumerator (which does not separate Mon-orbits — three buckets contain hidden sub-orbits); (iii) Frobenius-pair detection by canonical-under-Mon comparison of P versus Frobenius(P); (iv) independent Python audit by S₉ permutation enumeration with column-scale greedy normalisation.

Two corrections to previously held |Aut| values were made: the original B12 |Aut| reading was confirmed at 72; the B03 |Aut| value was discovered to be 6 (not 4 as recorded earlier), requiring full regeneration of the canonical pair catalogue on 30 April 2026.

The result is summarised in Table 5.

> **Table 5.** *The refined catalogue of [9, 5, 4]₄ codes under Mon-equivalence.*
>
> | Bucket | (A₄, A₅) | Structure                          | \|Aut_Mon\| per orbit | Mon-orbit count |
> |-------:|---------:|------------------------------------|-----------------------|-----------------|
> | B01    | (42, 168) | clean                             | 72                    | 1               |
> | B02    | (45, 153) | clean                             | 3                     | 1               |
> | B03    | (48, 138) | clean (\|Aut\| corrected to 6)       | 4                     | 1               |
> | B04    | (51, 135) | non-Frobenius split                | sum 7/12              | ≥ 2             |
> | B05    | (54, 120) | partial Frobenius split            | 9 + 9 + residual ≥ 1  | ≥ 3             |
> | B06    | (54, 132) | clean                             | 96                    | 1               |
> | B07    | (57, 117) | non-Frobenius split                | sum 7/18              | ≥ 2             |
> | B08    | (60, 102) | clean                             | 6                     | 1               |
> | B09    | (63, 99) | Frobenius pair                     | 36 each               | 2               |
> | B10    | (66, 72) | clean                             | 288                   | 1               |
> | B11    | (72, 90) | clean                             | 18                    | 1               |
> | B12    | (78, 72) | **clean — closed UNSAT (SCIP)**   | 72                    | 1               |
>
> Total Mon-orbits, minimum: **15**. Maximum estimate (pending sub-orbit resolution of B04, B05, B07): ≈ 20.

### 5.2 The B12 Formal Closure

**Theorem.** *The Mon-orbit B12 — generated by any [9, 5, 4]₄ code with weight enumerator (A₄, A₅) = (78, 72) — admits no extension to a [22, 6, 13]₄ code.*

**Proof.** Encoding the extension problem as a binary ILP over the 1 024 points of AG(5, 4), with a sum-to-13 constraint and 4 092 affine load-bound constraints, and submitting the resulting `.lp` file to SCIP 10.0.2 [BBC+24], the solver returns `problem is solved [infeasible]`. The closure was first achieved on 24 March 2026 in 18 minutes (1 078 s wall time) and reproducibly re-closed on 20 April 2026 in 420 s after pipeline refinement. The proof is a verified UNSAT certificate. ∎

**Significance.** B12 is the only [9, 5, 4]₄ Mon-orbit in the catalogue whose extension to [22, 6, 13]₄ is **formally proved impossible** at the close of this report.

### 5.3 The Stagnation Phenomenon (Findings F13–F17)

Repeated attempts to extend the SCIP closure of B12 to other Mon-orbits revealed a consistent stagnation regime, formalised across five operational findings.

**F13 (SCIP stagnation regime).** For every clean and Frobenius-pair Mon-orbit other than B12 — namely B01, B06, B09, B10 — monolithic SCIP under the 4 092-affine formulation enters a stagnation regime characterised by: dual bound frozen at 1.0; zero primal bound found; completion percentage plateauing at 43 %; and high conflict-clause generation without dual-bound progress. The phenomenon was confirmed across |Aut_Mon| values from 36 to 288 and across multiple SCIP random seeds. **Symmetry class does not predict closure time.**

**F14 (WS-profile invariance).** The per-slice bound histogram (the multiset of WS_m,α values, where WS_m,α counts admissible columns in the affine slice (m, α)) has identical sum 27 621 and mean 6.75 across all nine clean and Frobenius-pair Mon-orbits in the catalogue. The histograms differ only in whether the three tightest slices have bound 1 or bound 2.

**F15 (OA-hunt under standard basis is negative on B10).** A direct DFS search for the [22, 6, 13]₄ extension of B10 under the OA(12, 5, 4, 1) hypothesis (Result 3) in the standard coordinate basis explored approximately 6.5 × 10⁹ DFS nodes across two runs (10 hours plus 2.5 hours) without finding any 12-column OA-compatible arrangement. The OA + WS combined search space collapses sharply at depth 3–5; greedy lex-DFS dies at depth 6. This is a **hunt-negative**, not a non-existence proof: three explanations remain unseparated, including the possibility that the OA-compatible Diamond, if it exists, lives only in a non-standard basis.

**F16 (combinatorial depth-9 barrier on B10).** Three independent attack families on the B10 hard cluster — AUTMON DFS with full Mon-orbit reps + scaling + Arc Theorem + SBDS stabiliser chain; pair-forcing under the seed Mon-quotient with cascade depth-∞ + probing depth-1; DLX with most-constrained-variable branching — all converge on extension depth 9 as the maximum reachable. Cumulative ~4 × 10⁷ DFS nodes; no run reaches depth 10. **Strong evidence for a structural depth-9 barrier on B10.** The result is seed-specific: rowwise DFS on B02 reached depth 11 (3.232 × 10⁹ nodes; see §5.6), confirming that the depth-9 barrier is not universal.

**F17 (LP-relaxation invariance).** For every one of the nine clean and Frobenius-pair Mon-orbits, the continuous LP relaxation of the 4 092-affine formulation has

> LP_max = 16.000000 exactly,

with the same fractional optimum. Adding the 349 184 Arc Theorem triple cuts as additional LP constraints leaves LP_max unchanged (F17b). Single-column probing on B12 leaves LP_max at 16.0 (F17c). **The integer-LP gap is exactly 3 on every seed, and three orthogonal cut families do not tighten it.** Whatever distinguishes B12 (closeable) from B01/B06/B09/B10 (stagnating) lives in the integer polytope, not in the continuous one.

### 5.4 F18 — Pair-Forcing Under the Mon-Quotient Achieves Substantial Partial Closure

The PORMISCOJONES v2 engine combines (i) enumeration of canonical pair representatives under the seed's Mon-automorphism group, (ii) Arc Theorem pre-pruning, and (iii) BLOCKED cascade depth-∞ encoded as pre-SCIP unit constraints. The result on the two largest catalogued seeds:

> **Table 6.** *F18 PORMISCOJONES v2 results on B10 and B06.*
>
> | Seed | LIVE pairs | Pairs processed | INFEASIBLE | TIMEOUT (60 s) | Closure rate    |
> |------|-----------:|----------------:|-----------:|---------------:|-----------------|
> | B10  |      2 535 |           1 518 |      1 242 |            153 | ~95 % – 98 %    |
> | B06  |      7 559 |           3 387 |      2 985 |             86 | ~95 % – 98 %    |

Each individual UNSAT closure runs in 0.7 – 2.5 seconds. F18 v2 alone does not formally close any non-B12 seed but leaves a **reproducible residual cluster** across multiple seeds.

### 5.5 F19 — The Cross-Seed Cluster Phenomenon

The F18 timeouts concentrate at AG(5, 4)-vector anchors sharing the partial coordinate pattern {v₂ = 0, v₃ = 1, v₄ = 0}. Both B10 (anchors a = 70 and a = 71, all 153 timeouts) and B06 (single anchor a = 66, all 86 timeouts) exhibit the same anchor-cluster phenomenon, with two distinct sub-signatures:

- **Sabor A** (cas_zero = 0): pair-forcing cascade derives zero further fixings.
- **Sabor B** (cas_zero = 244): cascade derives 244 forced zeros, but SCIP still times out at the 60-second budget.

Subsequent refinements (F19c, F19d) showed that depth-2 pair-tentative cascade closes 2 of 4 Sabor B pairs on B06 (pairs (66, 90) and (66, 95) close in 1.3 s with 24 000 – 48 000 binary cuts pre-derived), while 2 remain residual ((66, 166) and (66, 183), both TIMEOUT at 600 s with 47 000 + and 24 000 + binary cuts respectively). **B06 reduces from 86 timeouts under F18 to two specific residual pairs under F18 + PROBE2** — materially close to formal closure.

F19e revealed that B10 does **not** replicate B06's 1 : 1 SOFT/HARD ratio. A representative sample of 8 B10 cluster pairs gave 1 SOFT and 7 HARD, with the HARD class splitting into HARD-pesado (24 000 – 48 000 binary cuts pre-derived but SCIP still TIMEOUT) and HARD-vacío (0 – 1 binary cuts after the same probing depth, propagation derives essentially nothing yet a global obstruction exists somewhere). HARD-vacío is the geometrically distinguished class. Three of the five HARD-pesado pairs share **exactly 24 310 binary cuts to the unit** across both anchors a = 70 and a = 71, suggesting a Mon-symmetry orbit of three pairs hidden inside the cluster.

### 5.6 F19g — The Codeword-Residual Mapping and the B02 Pivot

A structural mapping of E1★'s 390 minimum-weight codewords and a separately discovered E1-class record E4★'s 414 minimum-weight codewords to their residual seed buckets revealed the distribution shown in Table 7.

> **Table 7.** *Distribution of minimum-weight codeword residuals across [9, 5, 4]₄ Mon-orbits.*
>
> | Seed | Frequency in E1★ residuals | Frequency in E4★ residuals |
> |------|---------------------------:|---------------------------:|
> | B02  | 53.8 %                     | 30.4 %                     |
> | B01  | nontrivial                 | nontrivial                 |
> | B03, B04, B05 | present (lower)   | present (lower)            |
> | B06  | 0 %                        | 0 %                        |
> | B10  | 0 %                        | 0 %                        |
> | B11  | 0 %                        | 0 %                        |

**B02 dominates the near-Diamond residual distribution; B06, B10 and B11 do not appear at all as residuals of either E1★ or E4★.** This finding redirected the attack from B06/B10 to B02. The result on B02 was qualitatively different: **550/550 INFEASIBLE in the first sample of pair-fixed extensions at coverage 0.32 % of the canonical pair catalogue, with zero TIMEOUTs and zero feasible verdicts.**

In parallel, the rowwise DFS engine RWB02 reached extension depth 11 on B02 (the path [0, 1, 4, 96, 304, 637, 661, 842, 883, 941, 951], 3.232 × 10⁹ nodes, 4 hours 46 minutes on the M2). This is the first extension reaching depth ≥ 11 on any seed in the entire campaign, **confirming that the F16 depth-9 barrier is seed-specific to B10, not a universal feature of [9, 5, 4]₄ → [22, 6, 13]₄ extensions.**

### 5.7 F19g-bis through F19g-quinquies — The Universal Min-Size Predictor

Algebraic analysis of the B02 PROBE2 cut-class distribution identified five discrete cut-count classes. Class 0 (the algebraically softest) is structurally forbidden for `b mod 16 ∈ {4, 5, 6, 7}` — a perfect quarter-residue exclusion. A 5-condition exact algebraic rule (`rule_v4`) for Class-0 saturation behaviour was derived and verified at 32 / 32 true positives + 0 / 468 false positives.

A **universal min-size = 4 predictor** for the algebraically softest pair in any [9, 5, 4]₄ seed was extracted from this analysis and validated cross-seed. Pairs at minimum cut-class size, regardless of seed, close INFEASIBLE in their respective seed's analogue of "ULTRA-LIGHT" cut signature. A 22-pair cross-seed validation (10 untested B02 pairs + 12 B03 pairs) on 1 May 2026 closed 22 / 22 INFEASIBLE. Extension to all five clean Mon-orbits closed **64 / 64 INFEASIBLE, with zero feasible verdicts and zero timeouts**, as summarised in Table 8.

> **Table 8.** *Cross-seed sweep at the universally-predicted min-size = 4 algebraic softness location, 1 May 2026.*
>
> | Seed | \|Aut\| | size-4 count at a = 1 | cut signature              | typical solve_t |
> |------|--------:|----------------------:|----------------------------|-----------------|
> | B01  | 72      | 18                    | {81 × 18}                  | 23 – 54 s       |
> | B02  | 3       | 15                    | {195 × 14, 249 × 1}        | 17 – 63 s       |
> | B03  | 6       | 12                    | {354 × 8, 406 × 4}         | 22 – 70 s       |
> | B06  | 96      | 9                     | {24 531 × 6, 24 675 × 3}   | ≈ 1 s           |
> | B10  | 288     | 15                    | {24 675 × 15}              | ≈ 1 s           |
>
> Cumulative: **64 / 64 INFEASIBLE, 0 Diamonds, 0 TIMEOUTs**.

The three structural observations to be recorded for future work are:

(α) **The cut-doublet split** in B02, B03, B06: each seed splits its size-4 pairs into two cut-count values with split ratios 14 : 1, 8 : 4, and 6 : 3 respectively. A coordinate predictor of which doublet element a given pair lands in has not been identified.

(β) **The B01 singleton anomaly.** B01 has the lowest cut-count signature among all five seeds (cut count 81), the highest residual frequency in the E1★ codeword-residual mapping after B02, and a single-valued cut signature on 18 pairs. The combination is structurally unusual.

(γ) **The absence of TIMEOUTs.** B06 and B10 produced 153 + 86 timeouts in their original PORMISCOJONES v2 attacks but **zero timeouts in 64 size-4 attempts**. The hard pairs of those seeds lie in some region other than min-size = 4; a future campaign hunting **resistance** rather than **softness** may be the productive inversion of the entire predictor strategy used so far.

### 5.8 Summary of Phase 2 Closure Status

> **Table 9.** *Per-seed closure status at the close of this report.*
>
> | Seed | (A₄, A₅) | \|Aut\| | Best method                          | Status                                          |
> |------|---------:|--------:|--------------------------------------|-------------------------------------------------|
> | B12  | (78, 72) |     72  | SCIP ILP (4 092-affine)              | **UNSAT formal, 420 s**                         |
> | B06  | (54, 132) |    96  | F18 PORMISCOJONES + F19d PROBE2      | ~99 %; 2 residual hard pairs (66,166), (66,183) |
> | B10  | (66, 72) |    288  | F18 PORMISCOJONES + F19e PROBE2      | ~95 %; HARD-pesado / HARD-vacío residual         |
> | B01  | (42, 168) |    72  | F19g-quinquies size-4                 | 18 / 18 size-4 INFEAS (lowest cut signature)    |
> | B02  | (45, 153) |     3  | F19g + F19g-quinquies                 | 550 / 550 INFEAS at 0.33 % sample; 15 / 15 size-4 INFEAS; RWB02 depth 11 |
> | B03  | (48, 138) |     6  | F19g-quater size-4                    | 12 / 12 size-4 INFEAS                           |
> | B09  | (63, 99) |    36  | SCIP monolithic                      | Stagnated (F13). Frobenius pair: one closure covers both twins |
> | B11  | (72, 90) |    18  | —                                    | Untouched                                       |
> | B08  | (60, 102) |    6  | —                                    | Untouched                                       |
> | B04  | (51, 135) |   split | —                                    | Pending ENUM_v4 sub-orbit resolution             |
> | B05  | (54, 120) |   split | —                                    | Pending ENUM_v4 sub-orbit resolution             |
> | B07  | (57, 117) |   split | —                                    | Pending ENUM_v4 sub-orbit resolution             |

**Cumulative cross-seed min-size = 4 sweep: 64 / 64 INFEASIBLE, 0 Diamonds, 0 TIMEOUTs.**

---

## 6. The Probable-Non-Existence Reading

This section is informal. The numerical estimates given here are not theorems and are not intended as such. They are an honest working calibration based on the evidence accumulated to date.

### 6.1 What the Evidence Establishes

The campaign has established the following hard, verifiable facts.

(i) The world record for total excess in PG(5, 4) over a 22-multiset of rank 6 is E* = 1, witnessed by the matrix E1★ of (1). The record has stood since 24 March 2026 against more than 441 independent search engines and approximately 109 from-scratch search continents.

(ii) The total excess values 2 and 5 have **never** been observed in any of the 63 850+ targeted search restarts under three independent paradigms, supporting the Quantum Excess Conjecture (Result 12) that the excess spectrum is {0, 1, 3, 4, 6, 7, 8, …}. If the conjecture is correct, the next attainable value below E* = 3 is E* = 1, and below E* = 1 is E* = 0 — there is no "almost Diamond" to find by smooth descent.

(iii) Of the at-least-15 Mon-orbits in the [9, 5, 4]₄ catalogue, one (B12) is formally closed UNSAT, four more (B01, B02, B03, B06, B10) are swept INFEASIBLE at their algebraically softest pair-fixing region (64 / 64 sweep), and B02 is additionally swept INFEASIBLE at 0.33 % sample coverage of all `a = 1` pairs. **No feasible verdict has been returned by any engine in either phase of the campaign.**

(iv) The two Diamond-specific algebraic theorems (Results 1 and 8) impose strong geometric constraints unavailable to any prior published argument. Combined with Results 3, 4, 5, 7, 10, 11, they reduce the existence question to a finite extension problem with at most ~20 Mon-orbit cases.

### 6.2 What the Evidence Does Not Establish

We do **not** claim non-existence. The negative evidence is partial:

- B11 and B08 (clean, single-orbit) have not been attacked.
- The three FRAC Mon-orbit buckets (B04, B05, B07) — possibly comprising 6 or more sub-orbits in total — are blocked pending the ENUM_v4 canonical-under-Mon enumerator, which is a multi-day engineering task not yet completed.
- B10 retains a residual cluster of HARD-pesado and HARD-vacío pairs, and B06 retains 2 residual hard pairs, all formally TIMEOUT rather than INFEASIBLE.
- The min-size = 4 sweep covers only the algebraically softest region of each seed; the phenomenon (γ) — absence of TIMEOUTs in the soft sweep paired with their presence in the larger F18 attacks — suggests that the **structural hard pairs of clean seeds may live elsewhere than in the soft region**, an inversion of the predictor strategy that has not yet been investigated.

### 6.3 An Informal Probability Assessment

We offer the following calibration as a working summary of the evidence, **not as a theorem and not as a claim of fact**:

- The probability that a [22, 6, 13]₄ code exists with residual seed in the **already-swept clean Mon-orbits** (B01, B02, B03, B06, B10), located in the **already-swept algebraically-soft region**, is informally judged at **≪ 1 %** in light of the 64 / 64 + 550 / 550 INFEAS evidence.

- The probability that a [22, 6, 13]₄ code exists with residual seed in those clean Mon-orbits, located in a **non-soft region** of one of those seeds, is **informally judged at perhaps 5–15 %** — the campaign has explicitly not investigated this region (phenomenon γ above), and one cannot rule out a "barely-feasible" structure that resists every constraint without saturating any local cut.

- The probability that a [22, 6, 13]₄ code exists with residual seed in the **untouched clean Mon-orbits** (B08, B11) is **informally judged at perhaps 5–10 %** — both have moderate or low |Aut|, and the F13 stagnation regime is expected to apply to both.

- The probability that a [22, 6, 13]₄ code exists with residual seed in the **FRAC buckets** (B04, B05, B07) is the largest single open region. Given that FRAC sub-orbits arise from algebraically less-constrained code structure, we informally judge this at perhaps **15–25 %** conditional on the Diamond's existence.

- The probability that the [22, 6, 13]₄ code **does not exist** is, in our informal reading, **the modal hypothesis** at this stage of the campaign — perhaps **55–75 %** depending on how heavily one weights phenomenon γ and the FRAC residual.

We emphasise once more that these numbers are working calibrations of a campaign-internal assessment, not theorems. They are presented to make the authors' epistemic state legible to subsequent researchers — including ourselves — without overstatement.

### 6.4 What Would Settle the Question

Three concrete paths remain to a formal resolution.

**Path A — completion of the clean catalogue.** Build PORMISCOJONES + PROBE2 attacks on B08 and B11 (predicted ~18 h and ~6 h Mac wall time respectively, based on F18 extrapolations). Combine with a focused attack on the residual hard pairs of B06 and B10 — depth-3 probing or algebraic enumeration at depth 13 hard cap — to formally close those seeds. If all clean Mon-orbits close UNSAT, the question reduces to the FRAC catalogue alone.

**Path B — FRAC catalogue resolution.** Build the canonical-under-Mon enumerator ENUM_v4 with hash-invariant pre-bucketing (a multi-day engineering task), enumerate all sub-orbits of B04, B05, B07, and attack each with the F18 + PROBE2 + min-size pipeline. If all FRAC sub-orbits close UNSAT, the [22, 6, 13]₄ existence problem is formally resolved as **non-existent**.

**Path C — independent positive construction.** A construction of a [22, 6, 13]₄ code by any method — algebraic, geometric, or computational — would settle the question in the affirmative. Three independent published angles deserve attention: (i) the Krawtchouk-LP-feasible 10⁹ valid weight distributions admit, in principle, a code-theoretic construction; (ii) the OA(12, 5, 4, 1) reduction (Result 3) admits combinatorial-design approaches; (iii) algebraic-geometry constructions over GF(4)-rational curves or surfaces — beyond the Veronese, which is closed (§3.3).

---

## 7. Concluding Remarks

The campaign has not produced a [22, 6, 13]₄ code. It has, however, established that:

1. The world record for the [22, 6, 12]₄ excess profile is one (matrix E1★, (1)), with the codeword-count witness A₁₂ = 3 corresponding to the Z₃-orbit of the unique dirty-hyperplane normal.

2. Eight new structural theorems (Results 1, 3, 5, 7, 8, 9, 10, 11) constrain the geometry of any hypothetical [22, 6, 13]₄ code, with two of these (Results 1 and 8) being Diamond-specific and derived in collaboration with the Gemini consultant.

3. Twelve weight-enumerator buckets and a refined ≥ 15-Mon-orbit catalogue of [9, 5, 4]₄ codes form a finite extension problem to which one Mon-orbit (B12) has been formally proved infeasible.

4. The five remaining clean Mon-orbits and B12 itself have been cross-seed swept INFEASIBLE at their algebraically softest pair-fixing region with **64 / 64 INFEAS + 0 Diamonds + 0 TIMEOUTs**.

5. The continuous LP-relaxation of the 4 092-affine residual extension formulation has identical optimum 16.000000 across all nine clean and Frobenius-pair Mon-orbits, with integer-LP gap exactly 3, indicating that any residual obstruction is in the integer polytope rather than the continuous one.

6. Three structural mysteries (α, β, γ) and three open paths (A, B, C) remain.

At the close of this report, the authors' informal reading of the evidence is that **non-existence is the more probable hypothesis**, but the evidence does not formally establish it and we make no such claim. The matrices, code, theorems, dead ends and full computational record are made publicly available so that subsequent researchers — whether they prove non-existence, construct the Diamond, or pursue a third path — have the complete map of the territory the present campaign has explored.

The original Grassl entry of December 2001 has stood for a quarter-century. We have moved it from `d = 12–13` to `d = 12–13 (probable d = 12, evidence accumulated)`.

We thank Markus Grassl for [Gra26] — the foundational reference of this work — and for the open scientific tradition that allowed an independent, non-academic campaign such as the present one to engage with the question on equal terms. We thank the auditors Gemini, Grok and ChatGPT for adversarial mathematical review at every milestone; the Pair Theorem (Result 8) and the 6 → 10 Gap Theorem (Result 1) in particular originated from focused Gemini consultations. We thank S. Kurz for [Kur26]. The project was conducted entirely on independent resources.

---

## Appendix A. Reproducibility

All matrices, source code, raw logs, retracted hypotheses, and the full theorem record are available at the campaign repository `github.com/tretoef-estrella/hunt-for-distance-13`. All computations are reproducible from the published source on a MacBook Air M2 or equivalent under standard Linux/macOS, using SCIP 10.0.2 [BBC+24], C++17 compiled with `g++ -O3 -march=native`, and Python ≥ 3.10.

The independent verifier `verify_gf4.cpp` (~130 lines, no external dependencies) checks rank, minimum distance, weight enumerator, hyperplane load histogram, and total excess for any 6 × 22 generator matrix over GF(4). The verifier was used to confirm every published result of this report.

## Appendix B. Notation

| Symbol            | Definition                                                                |
|-------------------|---------------------------------------------------------------------------|
| GF(4)             | The finite field {0, 1, ω, ω²} with ω² + ω + 1 = 0.                       |
| [n, k, d]_q       | A linear code of length n, dimension k, minimum distance d over GF(q).    |
| d_q(n, k)         | The maximum minimum distance of any [n, k] linear code over GF(q).        |
| g_q(k, d)         | The Griesmer bound: g_q(k, d) = Σᵢ₌₀ᵏ⁻¹ ⌈d/qⁱ⌉.                          |
| PG(r, q)          | The projective space of dimension r over GF(q).                          |
| AG(r, q)          | The affine space of dimension r over GF(q): AG(r, q) = PG(r, q) \ PG(r−1, q). |
| load(H)           | For C ⊂ PG(r, q) and H ⊂ PG(r, q) a hyperplane, load(H) = |C ∩ H|.     |
| total excess E*   | E*(C) = Σ_H max(0, load(H) − 9) over all hyperplanes H.                  |
| Mon               | The monomial-equivalence group: column permutations × diagonal scalings × Frobenius. |
| OA(N, k, q, t)    | An orthogonal array of N runs, k factors, q levels, strength t.           |
| A_w               | The weight enumerator: A_w = number of codewords of Hamming weight w.    |
| N_k               | The hyperplane-load distribution: N_k = number of hyperplanes of load k. |
| WS_{m,α}          | The slice bound: WS_{m,α} = max number of admissible columns c with ⟨m, c⟩ = α. |
| ω                 | The primitive element of GF(4); the Frobenius automorphism is x ↦ x².      |

## References

[BBC+24] T. Berthold, S. J. Maher, K. Mexi, M. E. Pfetsch, S. Vigerske, and others. *The SCIP Optimization Suite 10.0.* Technical Report, Zuse Institute Berlin, 2024.

[BGV04] I. Bouyukliev, M. Grassl, and Z. Varbanov. New extremal and near-extremal binary codes. *Discrete Mathematics* **281** (2004), 43–66.

[Del73] P. Delsarte. *An algebraic approach to the association schemes of coding theory.* Philips Research Reports Supplement **10**, 1973.

[Gra26] M. Grassl. *Bounds on the minimum distance of linear codes and quantum codes.* Online at [codetables.de](https://codetables.de). Original [22, 6, 12]₄ entry: `last modified 2001-12-17`. Accessed March 2026.

[Gri60] J. H. Griesmer. A bound for error-correcting codes. *IBM Journal of Research and Development* **4** (1960), 532–540.

[Kur26] S. Kurz. *Personal communication: 482 inequivalent [21, 5, 13]₄ codes, none extends to length 22.* Universität Bayreuth, March 2026.

[MS77] F. J. MacWilliams and N. J. A. Sloane. *The Theory of Error-Correcting Codes.* North-Holland, 1977.

[Ple63] V. Pless. Power moment identities on weight distributions in error correcting codes. *Information and Control* **6** (1963), 147–152.

[Seg55] B. Segre. Curve razionali normali e k-archi negli spazi finiti. *Annali di Matematica Pura ed Applicata* **39** (1955), 357–379.

---

*Proyecto Estrella · Independent Research, Madrid · BSL 1.1 + SAMAEL Decree*
*github.com/tretoef-estrella · 2026*
