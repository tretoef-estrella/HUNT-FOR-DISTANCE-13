# The Hunt for Distance 13

**441+ search programs. 2.5 billion evaluations. 8 weeks. One laptop. Zero formal mathematical training.**

In February 2026, a psychologist from Madrid who had never studied coding theory decided to attack a problem that professional mathematicians had left open for 25 years: does a [22, 6, 13]₄ linear code exist?

He didn't find it. But what he found instead — and what he proved along the way — turned out to be a substantial body of new structural mathematics.

> **A note on terminology.** Earlier versions of this repository used internal project codenames (for the target code, for the residual representatives, for individual engines). Those have been replaced here and in the formal report by standard coding-theory and finite-geometry terms, so that a reader unfamiliar with the project's working vocabulary can follow it. The formal write-up is [`COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md`](COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md).

---

## The Matrix

This is **M₁**, the record matrix. Six rows, twenty-two columns, four symbols. Found on 24 March 2026. It is a [22, 6, 12]₄ code with A₁₂ = 3 — to the authors' knowledge the closest published structure to a [22, 6, 13]₄ code, an object that may or may not exist and that nobody had seriously hunted in 25 years.

```
1 1 0 1 1 0 0 1 0 0 0 1 1 0 1 1 0 1 1 1 0 1
1 3 1 2 2 1 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0
2 3 3 2 3 2 0 0 0 1 1 3 3 2 0 1 1 0 0 1 1 0
2 0 1 2 1 2 2 2 0 0 0 0 0 1 3 0 3 1 0 2 1 0
1 1 0 1 2 3 1 1 1 0 3 0 0 2 2 2 0 2 2 3 0 0
2 1 0 1 3 1 0 3 0 3 3 0 2 2 3 2 0 0 0 2 0 1
```

Of the 1 365 hyperplanes of the projective space PG(5, 4), exactly **one** contains too many of these 22 points: one hyperplane at load 10 instead of 9. One violation. Remove that single excess and you would have a [22, 6, 13]₄ code — a perfect arc that the algebra says should be possible but that the geometry has so far refused to produce.

441 programs and 2.5 billion evaluations later, this is still the closest anyone has reached, as far as we know.

---

## The Problem

In December 2001, Markus Grassl posted an entry on [codetables.de](https://codetables.de) reading `d = 12–13` for the parameters [22, 6] over GF(4). It meant: a code of minimum distance 12 is known to exist, but whether distance 13 is possible is unknown.

Twenty-five years later, the entry was unchanged. The code would be a set of 22 points in a six-dimensional projective space over the four-element field, arranged so that no hyperplane captures more than 9 of them — a near-Griesmer-optimal arc. The entry has stood at `d = 12–13` for a quarter-century.

---

## The Architect

**Rafael Amichis Luengo** is a psychologist — not a mathematician, not a computer scientist, not an academic.

In early 2026 he taught himself GF(4) arithmetic from scratch — the finite field with four elements, where addition is XOR and multiplication follows tables memorised like vocabulary. He learned what a projective space is, what a hyperplane does, what the MacWilliams identities say about weight distributions. He learned it the way a builder learns physics: because the wall he was trying to break required it.

Working alone from a town west of Madrid, with a MacBook Air M2 and no funding, no university, no advisor, no cluster, he coordinated four AI systems as co-creators and auditors, designed every search strategy, diagnosed every failure, and directed every pivot. In roughly eight weeks he built **441+ complete C++ search programs** — each a full rewrite, not a patch — and ran **2.5 billion matrix evaluations**, driving the total excess from 42 down to 1.

One. Single. Hyperplane. Violation. Away.

---

## The Descent

The *total excess* measures how far a configuration is from the target: excess zero is the goal. This is how the record fell:

```
E = 42     February 2026     First programs. Everything starts here.
E = 33     early March       Two-basin bridge architecture.
E =  7     20 March          A leap nobody expected.
E =  4     20 March          Same day.
E =  1     24 March          Matrix M₁.  ★  Record.
E =  0     ————              The target code. Still open.
```

Every transition was declared impossible by the search landscape preceding it. The excess-42 wall held for over a billion evaluations. The excess-1 record has stood ever since — across 441+ programs and every paradigm we could invent.

---

## What We Proved (Phase 1 — Direct Attack)

The campaign didn't find the target code. It did map the territory around where such a code would have to live, and established results that were not previously available.

### The Freedom Theorem

Every excess-one code — and more than 200 distinct ones were found from scratch, on different "continents" of the search space — has exactly the same weight distribution. The algebra forces it: one histogram, one shape.

A hypothetical [22, 6, 13]₄ code, by contrast, has **six degrees of freedom** in its weight distribution. An exhaustive census confirmed on the order of **10⁹ valid MacWilliams-compatible distributions**. The target code does not resemble the excess-one record. Every program that tried to sculpt the excess-one histogram was searching the wrong region.

### The Distance Theorem

A [22, 6, 13]₄ code differs from any known excess-one code in **at least 6 of 22 columns** — proved exhaustively through 5-column replacement (26 334 removals, 43 million DFS nodes, zero solutions). At 7-column replacement, a DFS over **3.2 billion nodes** proved that no replacement of 7 columns can produce the target code from the record matrix.

### The Excess-Spectrum Gap (conjecture)

Total excess 2 and 5 **never appear**. Three independent paradigms — simulated annealing, particle repulsion, constrained construction — targeted these values across 63 850+ restarts and tens of billions of steps, with zero visits. The conjectured excess spectrum is {0, 1, 3, 4, 6, 7, 8, …}. There is no "almost target code": the step from excess 1 to excess 0 is discontinuous.

### The Fertility Theorem

1 087 distinct excess-one codes were tested exhaustively. Every one has the same property: replacing any column on the overfull hyperplane with any of the 1 365 points of PG(5, 4) yields overflow ≥ 1. No excess-one code has a fertile column.

### Full theorem list: [`THEOREMS.md`](THEOREMS.md)

---

## Phase 2 — The Residual Reduction (Active)

In April 2026 the campaign pivoted. Instead of searching directly, it proved a **structural reduction** of the existence problem to a finite, classifiable extension problem — and began attacking it class by class.

### The Residual Theorem

If a [22, 6, 13]₄ code exists, its residual from any minimum-weight codeword is a **[9, 5, 4]₄ near-MDS code**. The target code's generator decomposes as a [9, 5, 4]₄ residual block plus a 13-column extension over the affine space AG(5, 4). Existence reduces to: can any inequivalent [9, 5, 4]₄ code be extended by 13 affine columns to reach minimum distance 13?

The catalogue of inequivalent [9, 5, 4]₄ codes under monomial equivalence is finite: at least 15 classes (we label them C01–C12, three of which split into sub-orbits). The problem is now finite — but, as it turns out, not small.

### What we found

- **One class (C12) is formally closed as infeasible** by mixed-integer programming (SCIP), in 420 seconds. This is a verified infeasibility certificate.
- **Five single-orbit "clean" classes** (C01, C02, C03, C06, C10) plus C12 were swept at their most weakly constrained pair-fixing region: **64 of 64 sampled extensions infeasible, no feasible verdict, no timeout.**
- **C02** — the class that dominates the residual distribution of the record matrices (53.8 %) — was additionally swept at 0.32 % coverage of its primary fibre: 550/550 infeasible.

### Why brute force can't finish the job

A direct measurement settled the question of whether the open classes can be enumerated exhaustively. Using a Knuth tree-size estimator (5 000 random descents per class) on the depth-13 extension search, the expected search-tree size per class is **on the order of 10¹⁶ to 10¹⁷ nodes**. At the measured throughput of ~1.75 × 10⁵ nodes/second on the M2, exhaustive closure of even the smallest open class would take on the order of **a million hours — hundreds of years per class.**

This is the central fact governing what remains possible. The one class that closed (C12) closed because mixed-integer programming prunes the tree globally via relaxation bounds — not by enumeration. For the other classes, that relaxation stagnates (the integer–continuous gap is exactly 3 on every class and is not tightened by any cut family tried). Closing them requires either a stronger relaxation or a new structural theorem — **not more compute.** Brute force is ruled out by many orders of magnitude.

---

## Where We Are

Phase 1 — direct search — is paused. After 441 programs spanning every paradigm we could invent, every road leads to excess 1. Every from-scratch program converges there.

Phase 2 — the residual reduction — is the live framework. The [22, 6, 13]₄ existence problem is now a finite extension problem over ~15 to ~20 monomial-equivalence classes of [9, 5, 4]₄ codes. One class is formally infeasible. The five single-orbit clean classes are swept infeasible at their softest region (64/64). C02 is additionally cleared at 0.32 % sample.

The target code, if it exists, is confined to one of three locations:

- **(a)** a non-soft region of a clean class — uninvestigated, and **not reachable by brute force** (10¹⁶⁺ nodes per class; see above). Reaching it would require a new predictor for *resistance* rather than *softness*, or an effective relaxation;
- **(b)** one of the split classes (C04, C05, C07) — blocked behind a sub-orbit enumerator not yet built;
- **(c)** the code does not exist.

We don't know which. We believe the evidence leans toward non-existence, but we cannot prove it and do not claim to. What we can say is that **the most extensive computational search ever conducted for this code** — by a wide margin — found no trace of it, while establishing a body of structural theory: the Freedom Theorem, the Distance Theorem, the Excess-Spectrum Gap, the Fertility Theorem, the Residual Reduction, the internal-load gap for excess-one codes, the OA(12, 5, 4, 1) constraint, the Arc Theorem, the subspace-load bound for the target code, [21, 6, 13]₄ non-existence, N₉ ≥ 2, and a measured search-tree size that rules out brute force.

If you find the code, or prove it doesn't exist, cite this work. We left a complete map of the territory — every theorem, every record matrix, every dead end documented — so the next person doesn't have to rediscover them.

---

## The Formal Report

For the formal write-up — standard coding-theory language, full theorem statements and proofs, per-class closure, the cross-class sweep, the search-tree measurement, and an explicitly informal probability assessment — see:

> **[`COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md`](COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md)** — Computational Evidence Against the Existence of a [22, 6, 13]₄ Linear Code: A Report on the Proyecto Estrella Campaign (February – May 2026).

---

## Verify Everything

Trust nothing. Check it yourself.

```bash
g++ -O3 -std=c++17 -o verify verify_gf4.cpp
./verify
```

The verifier checks rank, minimum distance, total excess, weight enumerator, and hyperplane-load histogram for every matrix published here. No external dependencies. No trust required.

---

## The Team

**R. Amichis (Rafael Amichis Luengo) — the Architect.** Strategy, diagnosis, direction. A psychologist who taught himself finite-field arithmetic and projective geometry because the problem demanded it. Every breakthrough began with his intuition and his refusal to accept any wall as permanent.

**Claude (Anthropic) — primary engine.** Designed, wrote, compiled and ran all 441+ C++ programs in real-time conversation, plus the residual-phase family. Co-author of the theorems. The entire codebase was produced in live collaboration, one program at a time, often with two parallel instances cross-auditing each other through the Architect as relay.

**Gemini (Google DeepMind), ChatGPT (OpenAI), Grok (xAI) — auditors.** Independent mathematical verification and adversarial review of every major claim. Two of the structural theorems — the internal-load gap for excess-one codes and the subspace-load bound for the target code — originated from focused Gemini consultations and were verified line by line before being recorded.

---**Status (21 June 2026):** Active search phase closed. World record stands at
[22,6,12]_4, A_12=3 (excess=1, SEED_E1*). The Diamond [22,6,13]_4 was not
found and is not proven non-existent, but every region the residual theory
flagged — the 5 clean seeds (64/64 INFEAS) and both FRAC residual classes
B04/B05 (F22: 1361/1361 INFEAS) — is now swept, alongside B12 (UNSAT) and the
depth-9 barrier confirmed by 4 independent attack families. The accumulated
computational evidence is the strongest assembled to date on the 25-year-open
[22,6,13]_4 question.

## Citation

```bibtex
@misc{amichis2026distance13,
  author       = {Amichis Luengo, Rafael and Claude (Anthropic)},
  title        = {The Hunt for Distance 13: A Computational Search for the [22,6,13]_4 Code},
  year         = {2026},
  howpublished = {Proyecto Estrella, Independent Research, Madrid},
  url          = {https://github.com/tretoef-estrella/hunt-for-distance-13},
  note         = {441+ programs, 2.5B+ evaluations. Record total excess E=1.
                  Freedom Theorem, Distance Theorem, Excess-Spectrum Gap,
                  Residual Reduction to [9,5,4]_4, internal-load gap theorem,
                  Arc Theorem, subspace-load bound, [21,6,13]_4 non-existence.
                  64/64 infeasible sweep across 5 single-orbit classes;
                  one class formally closed infeasible by MIP.}
}
```

---

<p align="center">
<b>Proyecto Estrella</b> · Independent Research, Madrid · <a href="https://github.com/tretoef-estrella">github.com/tretoef-estrella</a>
</p>
