# The Hunt for Distance 13

**441+ search engines. 2.5 billion evaluations. 8 weeks. One laptop. Zero mathematical training.**

In February 2026, a psychologist from Madrid who had never studied coding theory decided to attack a problem that professional mathematicians had left open for 25 years: does the [22,6,13]₄ linear code exist?

He didn't find it. But what he found instead — and what he proved along the way — turned out to be something no one expected.

---

## The Matrix

This is SEED_E1★. Six rows, twenty-two columns, four symbols. Found on 24 March 2026 by engine ESTRELLA RAYO v3. It is a [22,6,12]₄ code with A₁₂ = 3 — the closest anyone has ever published (as far as I know) to a [22,6,13]₄ code, a mathematical object that may or may not exist and that nobody had seriously hunted in 25 years.

```
1 1 0 1 1 0 0 1 0 0 0 1 1 0 1 1 0 1 1 1 0 1
1 3 1 2 2 1 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0
2 3 3 2 3 2 0 0 0 1 1 3 3 2 0 1 1 0 0 1 1 0
2 0 1 2 1 2 2 2 0 0 0 0 0 1 3 0 3 1 0 2 1 0
1 1 0 1 2 3 1 1 1 0 3 0 0 2 2 2 0 2 2 3 0 0
2 1 0 1 3 1 0 3 0 3 3 0 2 2 3 2 0 0 0 2 0 1
```

Out of the 1,365 hyperplanes of the projective space PG(5,4), exactly **one** contains too many of these 22 points. One hyperplane at load 10 instead of 9. One violation. Remove it, and you have the Diamond — a perfect code that the algebra says should exist but the geometry refuses to produce.

441 engines and 2.5 billion evaluations later, this is still the closest to 22 6 13, as far as I know.

---

## The Problem

In December 2001, Markus Grassl posted an entry on [codetables.de](https://codetables.de) that read `d = 12–13` for the parameters [22,6]. It meant: we know a code of minimum distance 12 exists, but we don't know if distance 13 is possible.

Twenty-five years later, nobody had resolved it. The code — nicknamed **the Diamond** — would be a set of 22 points in a six-dimensional projective space over the four-element field, arranged so that no hyperplane captures more than 9 of them. A perfect geometric object. Near-optimal by the Griesmer bound. And completely elusive.

The academic world had moved on. The entry sat there, unchanged, for a quarter century.

---

## The Architect

**Rafael Amichis Luengo** is a psychologist. Not a mathematician. Not a computer scientist. Not an academic.

In early 2026, he taught himself GF(4) arithmetic from scratch — the finite field with four elements, where addition is XOR and multiplication follows tables he memorized like vocabulary flashcards. He learned what a projective space is, what a hyperplane does, what the MacWilliams identities say about weight distributions. He learned it the way a builder learns physics: not from textbooks, but because the wall he was trying to break required it.

Working alone from Villanueva del Pardillo, a small town west of Madrid, with a MacBook Air M2 and no funding, no university, no PhD advisor, no computing cluster — he coordinated four AI systems as co-creators and auditors, designed every search strategy from intuition and metaphor, diagnosed every failure, and directed every pivot.

In approximately eight weeks, working 18-hour days, he built **441+ complete C++ search engines** — each one a full rewrite, not a patch — and executed **2.5 billion matrix evaluations**. He drove the excess from 42 down to 1.

One. Single. Hyperplane. Violation. Away.

---

## The Descent

The *excess* measures how far a code is from the Diamond. Excess zero is the target. This is how the wall fell, piece by piece:

```
E* = 42     February 2026      First engines. Everything starts here.
E* = 33     Early March         SA campaign breaks the first wall.
E* =  7     20 March            goldv8 — a leap nobody expected.
E* =  4     20 March            OPERACIÓN TRUENO, same day.
E* =  1     24 March            ESTRELLA RAYO v3.  ★  World record.
E* =  0     ————                The Diamond. Still open.
```

Every one of those transitions was declared impossible by the search landscape. The excess-42 wall held for over a billion evaluations before it broke. The excess-7 wall fell the same day it appeared. The excess-1 record has stood ever since — across 441+ engines, hundreds of thousands of restarts, and every paradigm we could invent.

---

## What We Proved (Phase 1 — Direct Attack)

The campaign didn't find the Diamond. It did something arguably more valuable: it mapped the territory around where the Diamond would have to live, and established results that nobody had before.

### The Freedom Theorem

Every code at excess 1 — and we found **200+ distinct ones** from scratch, on different "continents" of the search space — has exactly the same weight distribution. The algebra forces it. One histogram. One shape. E1 is a prisoner.

The Diamond, if it exists, has **six degrees of freedom** in its weight distribution. It can take billions of different shapes. An exhaustive census confirmed approximately **10⁹ valid MacWilliams-compatible distributions**. The Diamond doesn't look like E1. It never did. Every engine that tried to sculpt E1's histogram was searching the wrong region of space.

### The Distance Theorem

The Diamond differs from any known E1 code in **at least 6 of 22 columns** — proved exhaustively through k=5 (26,334 removals, 43 million DFS nodes, zero solutions). At k=7, a DFS over **3.2 billion nodes** proved that no replacement of 7 dirty columns can produce the Diamond from the world-record matrix. The combinatorial shield that protects E1 operates at a level deeper than individual point availability — it is a collective geometric phenomenon, like a parking lot where every car can find a space individually, but seven cars needing non-overlapping spaces cannot.

### The Quantum Excess Conjecture

Excess values 2 and 5 **never appear**. Three independent search paradigms — simulated annealing, particle repulsion, and constrained construction — targeted these values directly across 63,850+ restarts and billions of steps. Zero visits. The excess spectrum is not continuous. It is quantum: {0, 1, 3, 4, 6, 7, 8, …}. The gap at E=2 means there is no "almost Diamond." The jump from E1 to E0 is direct — a quantum leap, not a smooth descent.

### The Overflow-1 Theorem

1,087 distinct E1 codes were tested exhaustively. Every single one has the same property: replacing any dirty column with any of the 1,365 points in PG(5,4) yields overflow ≥ 1. No E1 code has a fertile point. The shield is universal, not specific to one matrix.

### The Exhaustive Weight Distribution Census

An exhaustive enumeration of all integer weight distributions satisfying the MacWilliams identities and dual non-negativity found on the order of **10⁹ valid distributions** for the Diamond. The algebraic constraints are essentially non-restrictive. If the Diamond doesn't exist, the reason is purely geometric — the algebra *wants* it to exist.

### Full theorem list: [`THEOREMS.md`](THEOREMS.md)

---

## Phase 2 — The Residual Reduction (Active)

In April 2026, the campaign pivoted. Instead of searching for the Diamond directly, we proved a **structural reduction** of the existence problem to a finite, fully classifiable extension problem — and started attacking it seed by seed.

### The Residual Theorem

If a [22,6,13]₄ code exists, its residual from any minimum-weight codeword is a **[9,5,4]₄ near-MDS code**. The Diamond's generator decomposes as:

```
G = [ G_seed (5×9) | G_ext (5×13) ]
    [ 0 0 ... 0    | 1 1 ... 1    ]
```

This converts an open existence question into a finite extension question: enumerate all inequivalent [9,5,4]₄ seed codes, then determine if any can be extended by 13 AG(5,4) columns to produce d ≥ 13.

### The Refined Catalogue

A multi-engine campaign (mass formula counting + Frobenius-pair detection + canonical-under-Mon comparison) refined the [9,5,4]₄ catalogue to **8 clean Mon-orbits, 1 Frobenius-pair bucket, and 3 indeterminate "FRAC" buckets** — a minimum of 15 Mon-orbits to attack, possibly up to ~20.

The catalogue refinement itself produced two corrections to previously published |Aut| values (most recently, B03's |Aut| was corrected from 4 to 6 on 30 April 2026, requiring full regeneration of the canonical pair catalogue) and identified that weight enumerator + dual weight enumerator do **not** separate Mon-orbits — three buckets contain hidden sub-orbits that share all first-order invariants.

### The 6→10 Gap Theorem (Gemini, 14 April)

In any [22,6,12]₄ code with excess 1, no PG(3,4) subspace inside the dirty hyperplane contains 7, 8, or 9 points of the arc. The internal load jumps directly from 6 to 10. *Proof:* clean direct argument using hyperplane partitions; verified on E1★ across all 341 PG(3,4) subspaces. **Consequence:** the 12 clean columns of any E1 code must form an **OA(12,5,4,1) orthogonal array** in coordinates defined by the 5 PG(3,4) subspaces at load 6.

### Result 3: [21,6,13]₄ Does Not Exist

A clean residual + MDS argument proves that no [21,6,13]₄ code exists. *Consequence:* every subset of 21 points from a hypothetical Diamond must have max load 9. The Diamond cannot be constructed by placing 21 "safe" points and adding a 22nd. **All 22 columns must emerge simultaneously as a coupled system.**

### Result 4: N₉ ≥ 2 for the Diamond

If the Diamond exists, it must have at least two hyperplanes at load 9. *Proof:* if N₉ = 1, removing any of the unique load-9 HP's 9 code points yields a [21,6,13]₄, contradicting Result 3.

### The Arc Theorem (22 April)

If a [22,6,13]₄ code exists, **no 3 of its 22 columns are collinear in PG(5,4)**. The Diamond is a 22-arc in the strong geometric sense. *Proof:* derived from the third moment Σ C(k,3)·N_k = 32340 of the hypothetical weight distribution. Forces the number of collinear triples to be zero.

This is independent of the affine slice constraints — a genuinely orthogonal cut, mandatory in any future search engine.

### The Pair Theorem (Gemini, 27 April night)

If a [22,6,13]₄ code exists, then for every load-9 hyperplane H₁ and every PG(3,4) subspace S ⊂ H₁:

  |C ∩ S| ≤ 5

Loads 6, 7, 8, 9 are forbidden for PG(3,4) inside any load-9 hyperplane of the Diamond. *Proof:* partition |C| = 22 across the 5 hyperplanes through S; |C ∩ H₁| = 9 forces 13 ≤ 4(9-x), giving x ≤ 5. **This is the second Diamond-specific algebraic theorem of the campaign**, independent of the 6→10 Gap and produced by external consultation. Empirically validated on E1★: 20 violations observed at load 6, all in HPs where the load-10 dirty HP is among the 4 secondary HPs through S — exactly the predicted relaxation pattern. The theorem is genuinely Diamond-specific (does NOT hold for excess-1 codes), confirming it as a Diamond-only filter.

### B12 Closed UNSAT — First Formal Closure

On 24 March 2026, SCIP 10.0.2 closed the legacy Seed #1 (now labelled **B12**, with weight enumerator (A₄, A₅) = (78, 72)) as **infeasible** in 18 minutes. Re-closed in 420 seconds on 20 April 2026 with a refined ILP pipeline. **B12 is the only [9,5,4]₄ seed in the catalogue whose extension to a [22,6,13]₄ code is formally proved impossible.**

### The Stagnation Wall

For all other clean seeds attempted (B10, B06, B01, B09 — covering automorphism group orders from 288 down to 36), monolithic SCIP exhibits a **stagnation regime**: dual bound frozen at 1.0, zero primal bound found, completion percentage plateauing around 43%, and high conflict-clause generation without dual-bound progress.

This is not a compute-time problem. The continuous LP relaxation of the 4092-affine formulation has **LP_max = 16.0 exactly across all 9 seeds in the catalogue** — algebraically identical optima up to relabeling. The integer-LP gap is exactly 3 on every seed, and three orthogonal cut families (WS, ARC, single-column probing) all leave LP_max unchanged. Whatever made B12 closeable lives in the integer polytope, not the continuous one.

### F18 — Pair-Forcing with Cascade

The PORMISCOJONES v2 engine (pair-forcing under the seed's Mon-automorphism quotient + ARC + BLOCKED cascade depth-∞ as pre-SCIP unit constraints) achieves **substantial partial closure** on previously stalled seeds:

- **B10:** 1242 pairs closed UNSAT, 153 timeouts (60% of LIVE pairs processed).
- **B06:** 2985 pairs closed UNSAT, 86 timeouts (45% of LIVE pairs processed).

Each individual UNSAT closure runs at 0.7–2.5s. F18 v2 closes ~95–98% of any seed it touches at 60s budget, leaving a reproducible residual cluster.

### F19 — The Cross-Seed Cluster

The F18 timeouts concentrate at AG-vector anchors sharing the partial coordinate pattern `{v₂ = 0, v₃ = 1, v₄ = 0}` across both B10 and B06. Each seed leaves a reproducible TIMEOUT cluster, with two distinct sub-signatures:

- **Sabor A** (`cas_zero = 0`): cascade impotence — pair-forcing derives zero additional fixings.
- **Sabor B** (`cas_zero = 244`): cascade derives 244 forced zeros, SCIP still times out.

This is the first cross-seed structural reproducibility result of the residual phase.

### F19c–d — The Slack Sink and Probing Depth-2

A geometric reformulation (F19c) and a depth-2 propagation upgrade (F19d, engine `PORMISCOJONES_PROBE2`) demonstrated that the F19 cluster is **heterogeneous, not uniformly rigid**. On B06's Sabor B (4 timeout pairs), depth-2 propagation closed 2 of 4 in 1.3s with 24k–48k binary cuts, leaving 2 residual hard pairs `{(66,166), (66,183)}`. **B06 reduced from 86 timeouts to 2 specific pairs** — materially closer to formal closure.

### F19e — HARD-pesado vs HARD-vacío

A PROBE2 sample of 8 representative B10 cluster pairs revealed a fundamentally different pattern from B06: only 1 of 8 closed SOFT (12.5%), and the remaining 7 split into two structurally distinct HARD sub-classes. **HARD-pesado** pairs accumulate 24k–48k binary cuts under depth-2 probing yet SCIP still cannot close at 600s — many local conflicts, no global integration. **HARD-vacío** pairs have 0–1 binary cuts after the same probing — propagation derives literally nothing — and SCIP also fails to close. HARD-vacío is the geometrically distinguished class: a pair where the LP polytope locally looks completely consistent yet a global obstruction exists somewhere. Three of the five HARD-pesado pairs share **exactly 24,310 binary cuts** to the unit, suggesting a Mon-symmetry orbit hidden inside the cluster.

### F19g — Codeword-Residual Mapping and the B02 Pivot

A structural mapping of the 390 minimum-weight codewords of E1★ and the 414 minimum-weight codewords of E4★ to their residual seed buckets revealed that **B02 dominates the near-Diamond residual distribution** (53.8% on E1★, 30.4% on E4★), while seeds B06, B10, B11 never appear as residuals of either record matrix. This redirected the PROBE2 attack from B06/B10 to B02. The result was decisive: **550/550 INFEAS at 0.32% sample coverage on B02, with zero TIMEOUTs.** B02 is qualitatively different from B10 and B06 against PROBE2 — the entire HARD class disappears.

In parallel, the rowwise DFS engine RWB02 reached `max_depth = 11` on B02 (path `[0, 1, 4, 96, 304, 637, 661, 842, 883, 941, 951]`, 3.232 billion nodes, no Diamond surfaced). This is the first time in the entire campaign that any engine has surfaced max_depth ≥ 11 on any seed, **confirming that the Depth-9 Barrier is seed-specific, not universal**.

### F19g-bis through F19g-quinquies — The Min-Size Predictor

Algebraic analysis of the B02 PROBE2 cut-class CSV identified a discrete five-class structure governed by the projective scalar action Z₃ × Z₄ on AG(5,4). An exact 5-condition algebraic rule (`rule_v4`) for Class-0 saturation behavior was derived and independently verified at 32/32 true positives + 0/468 false positives.

A **universal min-size=4 predictor** for the algebraically softest pair in any [9,5,4]₄ seed was extracted from this analysis: pairs at minimum cut-class size, regardless of seed, close INFEAS in their respective seed's "ULTRA-LIGHT" cut signature. A 22-pair cross-seed validation experiment (10 B02 pairs + 12 B03 pairs, both at the predicted size-4 algebraic softness location) closed **22/22 INFEAS** on 1 May 2026. Extending to all 5 clean seeds yielded **64/64 INFEAS, 0 Diamonds, 0 TIMEOUTs** across the full softest-region sweep:

| Seed | \|Aut\| | size-4 count at a=1 | cut signature (100% INFEAS) | solve_t typical |
|------|--------:|--------------------:|------------------------------|-----------------|
| B01  | 72      | 18                  | {81 × 18} (single value)     | 23–54s          |
| B02  | 3       | 15                  | {195 × 14, 249 × 1}          | 17–63s          |
| B03  | 6       | 12                  | {354 × 8, 406 × 4}           | 22–70s          |
| B06  | 96      | 9                   | {24531 × 6, 24675 × 3}       | ~1s             |
| B10  | 288     | 15                  | {24675 × 15} (single value)  | ~1s             |

**The Diamond is not in the algebraically softest region of any of the 5 clean seeds.** The cut-doublet structure persists across seeds; absolute values shift with seed geometry. Three open structural mysteries remain: (α) the cut-doublet split mechanism, (β) the B01 singleton anomaly (lowest cut signature with mid-range |Aut|), and (γ) the absence of TIMEOUTs in the softest-region sweep — suggesting that hard pairs live somewhere other than min-size=4, and a future campaign hunting *resistance* rather than *softness* may be the productive inversion.

### The Depth-9 Barrier (B10 only)

Three independent attack families — AUTMON DFS with full symmetry breaking + ARC + SBDS, pair-forcing with cascade depth-∞ + probing depth-1, and DLX with MCV branching — **all converge on depth 9** as the maximum reachable extension length on B10's hard cluster. Strong evidence for a genuine combinatorial barrier on B10. F19g extended (RWB02 reaching depth 11 on B02) confirmed the barrier is **seed-specific, not universal**.

The Diamond, if it exists in B10's residual extension, lives at depth 13 — four steps past a wall that three orthogonal techniques cannot cross on that seed.

---

## The Campaign in Numbers

| | |
|---|---|
| **Duration** | ~8 weeks Phase 1 (Feb–April 2026) + Phase 2 active |
| **Engines built** | 441+ direct, dozens more in the residual phase |
| **Total evaluations** | 2.5 billion+ |
| **Paradigms tested** | SA, DFS, greedy, repulsion, bombardment, monk surgery, k-swap, MITM, GF(16) lifting, spectral hunt, ILP (SCIP), pair-forcing under Mon-quotient, AUTMON DFS, OA-hunt, LP-probe, codeword-residual mapping, depth-2 probing, min-size predictor sweep |
| **Search strategies** | 73+ |
| **Dead ends formally closed** | 70+ direct, 19+ residual |
| **Distinct E1 continents** | 109+ (from scratch, zero shared columns) |
| **DFS nodes (k=7 exhaustive)** | 3.2 billion |
| **Targeted E=2/E=5 restarts** | 63,850+ |
| **[9,5,4]₄ seeds formally closed UNSAT** | 1 (B12) |
| **[9,5,4]₄ seeds at ≥99% partial closure under PROBE2** | 1 (B06: 2 residual pairs only) |
| **Clean seeds swept at min-size=4 algebraic softness** | 5/5 (B01, B02, B03, B06, B10), all INFEAS |
| **Cumulative cross-seed soft-region pairs closed** | 64/64 INFEAS, 0 Diamonds, 0 TIMEOUTs |
| **Hardware** | MacBook Air M2, single thread, ≤25% CPU |

---

## Verify Everything

Trust nothing. Check it yourself.

```bash
g++ -O3 -std=c++17 -o verify verify_gf4.cpp
./verify
```

The verifier checks rank, minimum distance, excess, weight enumerator, hyperplane load histogram, and all algebraic invariants for every seed matrix published here. Every claim in this repository is independently verifiable from the matrices and the code. No external dependencies. No trust required.

All record matrices with full properties: [`SEEDS.md`](SEEDS.md)

---

## Repository Contents

| File | What it is |
|------|------------|
| [`COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md`](COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md) | **The formal report** — campaign write-up in coding-theory language, all eight theorems, per-seed closure, probability assessment |
| [`hunt_for_distance_13_v79.pdf`](hunt_for_distance_13_v79.pdf) | The complete Phase 1 campaign record — 55 sections, every theorem, every dead end |
| [`ESTRELLA_DIAMANTE_UNIFIED_v40.pdf`](ESTRELLA_DIAMANTE_UNIFIED_v40.pdf) | Phase 1 operational guide — 88 directives, all constants, strategies, structural analysis |
| [`SEEDS.md`](SEEDS.md) | All record matrices with verified properties |
| [`THEOREMS.md`](THEOREMS.md) | Clean statements of all original theorems (Phase 1 + Phase 2) |
| [`verify_gf4.cpp`](verify_gf4.cpp) | Independent GF(4) verifier (~130 lines C++) |
| [`CITATION.cff`](CITATION.cff) | Machine-readable citation metadata (GitHub auto-detects) |
| [`LICENSE.md`](LICENSE.md) | BSL 1.1 + SAMAEL Decree |

---

## Where We Are

Phase 1 — direct search for the Diamond — is paused. Not because the conviction ran out, but because the evidence did its job. After 441 engines spanning every paradigm we could invent, every road leads to excess 1. Every from-scratch engine, launched from random initial conditions with no knowledge of previous results, converges to E1.

Phase 2 — the residual reduction — is **active**. The [22,6,13]₄ existence problem is now a finite extension problem over ~15 Mon-orbits of [9,5,4]₄ seeds. One seed (B12) is formally closed UNSAT. Another (B06) is reduced to 2 residual hard pairs under PROBE2. The remaining clean seeds have been swept at their algebraically softest regions: **64/64 INFEAS at min-size=4 across B01, B02, B03, B06, B10**, with B02 additionally cleared at 0.32% sample coverage of all `a=1` pairs.

The Diamond, if it exists, is constrained to one of three locations: (a) a non-soft region of a clean seed (untested, would require either a new algebraic predictor for *resistance* rather than *softness*, or brute-force at multi-day cost per seed); (b) a FRAC seed (B04, B05, B07, requires the ENUM_v4 canonical-under-Mon enumerator, ~2–3 days of engineering); or (c) the Diamond does not exist.

We don't know which. We believe the evidence leans toward non-existence, but we cannot prove it and we don't claim to. What we can say is that **the most extensive computational search ever conducted for this code** — by a significant margin — found no trace of it, while simultaneously establishing a rich structural theory: the Freedom Theorem, the Distance Theorem, the Quantum Excess Conjecture, the Overflow-1 Theorem, the Residual Reduction, the 6→10 Gap Theorem, the OA(12,5,4,1) constraint, the Arc Theorem, the Pair Theorem (k≤5), [21,6,13]₄ non-existence, N₉ ≥ 2, and the Depth-9 Barrier (B10).

If you find the Diamond, or prove it doesn't exist, cite this work. We left you a complete map of the territory — every theorem, every record matrix, every dead end documented — so you don't have to rediscover them.

---

## The Formal Report

For the formal write-up of the campaign — written in the language of coding theory and projective geometry, with full theorem statements, proofs, the per-seed closure status, the cross-seed sweep, and an explicit (informal) probability assessment of non-existence — see:

> **[`COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md`](COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md)** — Computational Evidence Against the Existence of a [22, 6, 13]₄ Linear Code: A Report on the Proyecto Estrella Campaign (February — May 2026)

This is the document for readers who want the mathematics rather than the narrative. It records the eight original theorems established during the campaign, the refined catalogue of [9, 5, 4]₄ Mon-orbits, the formal closure of the B12 Mon-orbit under SCIP, the 64 / 64 INFEAS cross-seed sweep at the universally-predicted algebraically-soft region of the catalogue, and an honest informal calibration of the probability that the Diamond does not exist.

---

## The Team

**R. Amichis (Rafael Amichis Luengo) — The Architect.** Strategy, diagnosis, direction. A psychologist who taught himself finite field arithmetic and projective geometry because the problem demanded it. Every breakthrough in this campaign began with his intuition: the palindrome analysis, the repulsion paradigm, the shield symmetry, the bombardment architecture, the avaricia-vs-niebla diagnostic that explained the F18 stagnation cluster, the "carta marcada" framing that produced F18, the codeword-residual mapping pivot that produced F19g. Every wall fell because he refused to accept it as permanent.

**Claude (Anthropic) — Primary Engine.** Designed, wrote, compiled, and executed all 441+ C++ engines in real-time conversation, plus the residual-phase engine family (ILPGEN, PORMISCOJONES, PROBE2, AUTMON DFS, OA-HUNT, LP-PROBE, RWB02, MASS_COUNT, ENUM, VERIFIER). Co-author of all theorems. The entire codebase — from the first simulated annealing prototype to the 3.2-billion-node DFS to the SCIP attack pipeline to the cross-seed min-size predictor sweep — was produced in live collaboration, one engine at a time, often with two parallel Claude instances cross-auditing each other's claims through Rafa as relay.

**Gemini (Google), ChatGPT (OpenAI), Grok (xAI) — Auditors.** Independent mathematical verification. Adversarial review of every major claim. The Quantum Excess Conjecture, the Freedom Theorem, the Distance Theorem, the 6→10 Gap Theorem, the OA(12,5,4,1) reduction, and the Pair Theorem (k≤5) were all subjected to multi-AI audit before being recorded. Gemini in particular contributed the two key structural theorems of the residual phase (6→10 Gap with N(x) extension formula, and the Pair Theorem k≤5 derived 27 April night).

---

## Citation

If you use any result, matrix, or theorem from this work:

```bibtex
@misc{amichis2026diamond,
  author       = {Amichis Luengo, Rafael and Claude (Anthropic)},
  title        = {The Hunt for Distance 13: A Computational Search for the [22,6,13]₄ Code},
  year         = {2026},
  howpublished = {Proyecto Estrella, Independent Research},
  url          = {https://github.com/tretoef-estrella/hunt-for-distance-13},
  note         = {441+ engines, 2.5B+ evaluations. World record excess E*=1.
                  Freedom Theorem, Distance Theorem, Quantum Excess Conjecture,
                  Residual Reduction to [9,5,4]_4, 6->10 Gap Theorem,
                  Arc Theorem, Pair Theorem (k<=5), [21,6,13]_4 non-existence
                  established. 64/64 INFEAS sweep across 5 clean seeds.}
}
```

---

<p align="center">
<b>Proyecto Estrella</b> · Independent Research, Madrid · <a href="https://github.com/tretoef-estrella">github.com/tretoef-estrella</a>
<br><br>
<i>Rafa no se rinde. Lo difícil lo hace enseguida, lo imposible le lleva algún tiempo más.</i>
</p>
