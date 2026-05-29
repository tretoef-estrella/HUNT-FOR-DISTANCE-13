---
## Addendum — 29 May 2026 (F20: the search-tree size measurement closes brute-force enumeration as a viable strategy, and the subspace-load bound (Pair Theorem k≤5), though valid, is shown to be unrescuable as a node-by-node DFS prune)

### Context

After F19g-quinquies left the campaign at a strategic decision point (extend, resolve split classes, or write the negative-result report), the open question was whether the proven-but-never-integrated subspace-load bound (Pair Theorem k≤5, F19f) could be turned into an effective closure tool. The prior briefing proposed integrating it as a SCIP lazy constraint on a single low-cut resistant pair of C10 (70,290). This session settled the question — negatively, but decisively — and in doing so produced the single most important quantitative bound the campaign has on what remains computationally possible.

### What was built and measured

1. **Engine `ESTRELLA_KCUT_DFS_v1.cpp`** — the F19g integration that was never built: the BLOCKED-cascade DFS (SHERLOCK lineage) with the k≤5 bound fired dynamically at every load-9 event. Built, compiled, sanity-validated against C12 (no depth-13 leaf, consistent with the formal UNSAT). 

2. **Throughput measurement (decisive).** The baseline DFS runs at ~175k nodes/s on the M2. With the k≤5 bound enforced node-by-node, throughput drops below ~25k–40k nodes/s — a slowdown of more than 4×. The cause: each load-9 event requires scanning the 341 PG(3,4) subspaces inside the completed hyperplane; load-9 events first occur deep in the tree, after the dominant low-depth branching has already generated the bulk of the nodes. The bound fires too late to prune the wide part of the tree, and its per-node cost outweighs what it removes. Two rounds of optimization (restricting to PG(3,4) containing the just-placed point; early-out on seed-load; removing heap allocation from the hot path) did not recover the throughput. **The bound is mathematically valid and is a correct verification filter for any completed candidate, but it does not make exhaustive enumeration feasible.**

3. **The tree-size measurement (the core finding).** Re-reading the SHERLOCK-v2 5000-walk log (Knuth estimator under the affine constraints) settled the question of whether any clean class can be closed by enumeration at all:

| Class (old label) | Estimated DFS tree size | max depth | Exhaustive hours @250k/s |
|---|---:|---:|---:|
| C03 (B-old #3) | 3.1 × 10¹⁶ | 10 | ~3.5 × 10⁷ |
| C04 (#4) | 1.2 × 10¹⁷ | 10 | ~1.3 × 10⁸ |
| C05 (#5) | 1.8 × 10¹⁶ | 9 | ~2.1 × 10⁷ |
| C06 (#6) | 5.7 × 10¹⁶ | 10 | ~6.4 × 10⁷ |
| C07 (#7) | 6.1 × 10¹⁶ | 10 | ~6.8 × 10⁷ |
| C08 (#8) | 3.2 × 10¹⁶ | 10 | ~3.5 × 10⁷ |
| C09 (#9) | 7.8 × 10¹⁶ | 10 | ~8.7 × 10⁷ |
| C10 (#10) | 3.3 × 10¹⁶ | 10 | ~3.7 × 10⁷ |
| C11 (#11) | 1.9 × 10¹⁶ | 9 | ~2.1 × 10⁷ |
| C12 (#12) | 5.2 × 10¹⁶ | 10 | ~5.7 × 10⁷ |

Each tree is on the order of 10¹⁶–10¹⁷ nodes. At ~1.75 × 10⁵ nodes/s on the M2, exhaustive closure of even the smallest class is on the order of 10⁶ hours — hundreds to thousands of years per class. **Brute-force depth-13 enumeration of any open class is infeasible by many orders of magnitude.**

### What F20 establishes

**(1) Brute force is dead as a closure strategy.** This is not a heuristic impression; it is a measured tree size. The only class ever closed (C12) closed via SCIP, which prunes globally through the LP relaxation — not by enumeration. For every other class the relaxation stagnates (F13/F17: integer–continuous gap exactly 3, untightened by any cut family tried). Closing them requires either an effective relaxation or a new structural theorem. Not more compute.

**(2) The k≤5 bound is not the lever.** It is valid and Diamond-specific (E1★ violates it 20× as expected, F19f), and it is the correct final verification filter for any candidate extension. But integrated node-by-node into DFS it fires too late and costs too much; integrated statically into the pair-attack .lp it is mathematically incorrect (no seed-forced load-9 hyperplane exists — measured: the seed contributes at most 7 to any slice in C02, so no hyperplane is forced to load 9 independently of the extension, hence a static k≤5 cut would prune valid solutions). The only correct dynamic form (indicator constraints y_H → Σ≤5) would inject hundreds of thousands of binary indicators into a formulation that already stagnates at the LP root (F13), which would choke the solver. **All three integration routes are now closed. Do not re-attempt k≤5 as a primary closure mechanism.**

**(3) The honest strategic reading is unchanged but now quantified.** The target code, if it exists, is confined to (a) a non-soft region of a clean class — uninvestigated, and now known to be unreachable by brute force; (b) a split class (C04/C05/C07) — pending the sub-orbit enumerator; or (c) it does not exist. The probability calibration in the formal report stands; F20 adds the hard fact that (a) cannot be settled by enumeration.

### What was built as the last directed shot

**Engine `ESTRELLA_FLAUTISTA_v1.cpp`** — a stochastic random-restart search on C02 (the dominant residual class, 53.8% in M1), biased to the NON-SOFT region (min-expression size 2 or 3, avoiding the already-swept size-4 soft region). ~17k restarts/s; ~60M restarts per hour of Mac time. This is not exhaustive and does not promise a code — it is a directed lottery ticket aimed at mystery (γ) (the resistant region the campaign never investigated), with the k≤5 bound as the final verification filter to reject false positives. Honest expected outcome: best depth ~10, no candidate. If a depth-13 candidate surfaces, it must be verified externally by `verify_gf4.cpp` before the word "code" is used.

### Documentation work this session

- The formal report `COMPUTATIONAL_EVIDENCE_AGAINST_22_6_13_q4.md` was rewritten: all internal codenames replaced by standard coding-theory terminology (per external-reviewer feedback that the internal vocabulary was unintelligible), two arithmetic errors corrected (a Griesmer mis-contextualization in §1.5; a four-vs-five count in the closure summary), and the F20 tree-size argument added as quantitative evidence for the probable-non-existence reading. PDF generated.
- `README.md` and `THEOREMS.md` brought into terminological consistency with the report; the README's prior "multi-day cost per seed" phrasing corrected to the measured 10¹⁶-node / centuries-per-class reality.
- The Phase-1 manual (v40) is preserved unchanged with a new contextualizing header marking it as a historical Phase-1 record rather than the current operational state.

### Credits

- **k≤5 DFS integration, throughput measurement, two optimization rounds:** Claude (current instance), 29 May 2026.
- **Static-cut feasibility check on C02 (no seed-forced load-9 hyperplane exists):** Claude, sandbox computation, 29 May 2026.
- **Tree-size reading from SHERLOCK-v2 log and the brute-force-infeasibility conclusion:** Claude, 29 May 2026, after R. Amichis correctly challenged an unquantified "weeks" time estimate — the challenge forced reading the actual measured tree sizes, which were ~5 orders of magnitude larger than the loose estimate.
- **Flautista directed-stochastic engine, C02 non-soft bias:** Claude, 29 May 2026.
- **Strategic decision to treat this as the final campaign and to prioritize leaving the repository in publishable form:** R. Amichis, 29 May 2026.

---

*Proyecto Estrella · 29 May 2026 — Madrid · F20 added.*
*Search-tree size measured at 10¹⁶–10¹⁷ nodes per clean class: brute-force enumeration is infeasible by many orders of magnitude. The subspace-load bound (Pair Theorem k≤5) is valid and is the correct final verification filter, but all three integration routes (node-by-node DFS, static .lp cut, indicator constraints) are closed — it is not a closure lever. The target code, if it exists, is confined to a non-soft clean-class region (unreachable by brute force), a split class (pending sub-orbit enumeration), or it does not exist. Formal report, README, and THEOREMS rewritten in standard terminology; Phase-1 manual preserved with a historical-context header. Last directed shot: a stochastic random-restart search on C02's non-soft region. The corner that was never looked at gets one hour of light.*
