# Record Seed Matrices — Proyecto Estrella

All matrices are 6×22 generator matrices over GF(4) = {0, 1, 2, 3}.
Verify any matrix with [`verify_gf4.cpp`](verify_gf4.cpp).

---

## SEED_E1★ — World Record (Excess = 1)

**Found:** 24 March 2026, ESTRELLA_RAYO_v3
**Properties:** rank = 6, d_min = 12, excess = 1, A₁₂ = 3

```
1 1 0 1 1 0 0 1 0 0 0 1 1 0 1 1 0 1 1 1 0 1
1 3 1 2 2 1 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0
2 3 3 2 3 2 0 0 0 1 1 3 3 2 0 1 1 0 0 1 1 0
2 0 1 2 1 2 2 2 0 0 0 0 0 1 3 0 3 1 0 2 1 0
1 1 0 1 2 3 1 1 1 0 3 0 0 2 2 2 0 2 2 3 0 0
2 1 0 1 3 1 0 3 0 3 3 0 2 2 3 2 0 0 0 2 0 1
```

**Dirty hyperplane:** HP#1122, normal = (1,3,0,0,3,1) — palindrome, load = 10.
**Columns on dirty HP (zeros):** [5, 6, 8, 12, 13, 16, 17, 19, 20, 21]
**Columns off dirty HP (support):** [0, 1, 2, 3, 4, 7, 9, 10, 11, 14, 15, 18]
**Weight enumerator:** A₁₂=3, A₁₃=390, A₁₄=390, A₁₅=540, A₁₆=675, A₁₇=636, A₁₈=690, A₁₉=540, A₂₀=225, A₂₁=6, A₂₂=0
**HP load histogram:** 2/75/180/230/212/225/180/130/130/1 (loads 1–10)
**Algebraic invariants:** mean = 5.4960, variance = 4.0595, sat9 = 130

---

## SEED_E3 — Excess = 3 (Particle Repulsion Discovery)

**Found:** 29 March 2026, REPULSION_v1 (restart 71,634, 4966.2s)
**Properties:** rank = 6, d_min = 12, excess = 3, A₁₂ = 9

```
1 0 1 0 1 1 1 0 0 1 1 0 1 0 0 1 0 1 1 1 1 0
2 1 2 1 3 2 2 0 1 3 3 0 1 0 1 0 0 1 2 1 3 1
1 3 1 3 0 2 1 1 3 1 3 0 3 1 0 3 1 0 0 1 1 1
2 1 1 2 2 3 2 1 1 1 3 1 1 1 0 0 2 0 3 0 3 1
0 2 3 0 1 2 2 3 3 0 2 1 3 1 3 2 1 3 3 2 1 3
0 0 3 1 3 1 2 2 0 0 1 0 3 1 1 0 1 3 0 0 1 0
```

**Significance:** First excess=3 code found. Jumped from E=7 directly to E=3 — skipping E=6, E=5, E=4. First observation of quantum excess gaps.
**Dirty hyperplanes:** 3 (none palindromic). HP#1122 at load = 5 (not dirty).
**Algebraic invariants:** mean = 5.4960, variance = 4.0595 (same as E1★)

---

## SEED_TRUENO — Excess = 4, Cuenca A

**Found:** 20 March 2026, OPERACION_TRUENO (cycle 1121)
**Properties:** rank = 6, d_min = 12, excess = 4

```
1 1 1 1 1 1 0 1 0 0 0 1 1 0 1 1 0 1 1 1 1 1
1 3 0 2 2 0 1 0 1 0 1 1 1 1 0 1 0 0 1 2 0 0
2 3 3 2 3 0 0 0 2 1 1 3 0 3 0 1 1 0 0 1 0 0
2 0 1 2 1 1 2 2 2 0 0 0 0 1 3 0 1 1 0 2 3 0
1 1 0 1 2 0 1 1 3 0 3 0 3 0 2 2 0 2 2 3 2 0
2 1 1 1 3 1 0 3 1 3 3 0 0 0 3 2 0 0 0 2 0 1
```

**Algebraically closed** by PATH D CSP (244 nodes, 0s).

---

## SEED_RELAMPAGO — Excess = 6

**Found:** 20 March 2026, OPERACION_RELAMPAGO (round 19)
**Properties:** rank = 6, d_min = 12, excess = 6

```
1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
0 3 0 1 3 1 3 2 2 2 0 1 0 0 2 0 0 0 3 1 0 3
2 1 0 1 0 2 3 1 1 2 0 2 3 0 0 0 3 0 3 3 0 0
0 0 0 3 2 3 2 3 3 0 2 0 2 1 1 1 3 1 2 1 3 0
0 3 2 2 2 3 0 0 1 2 0 0 0 1 2 2 0 1 3 1 3 0
2 1 1 2 0 1 0 0 3 2 3 0 2 2 0 2 0 1 3 3 0 0
```

---

## SEED_GOLDV8 — Excess = 7

**Found:** 20 March 2026, goldv8 (cycle 288,980)
**Properties:** rank = 6, d_min = 12, excess = 7

```
1 1 1 1 1 0 1 1 1 0 0 1 0 0 1 1 1 1 1 0 1 0
1 3 0 2 2 0 2 0 1 0 1 1 1 1 0 1 0 1 1 1 0 1
2 3 0 2 3 1 1 0 0 1 1 3 0 3 0 1 0 1 0 0 3 2
2 0 1 2 1 1 2 2 0 0 0 0 0 1 3 0 0 2 0 2 1 2
1 1 2 1 2 0 3 1 3 0 3 0 1 0 2 2 0 1 2 1 0 3
2 1 0 1 3 0 2 3 0 3 3 0 0 0 3 2 1 1 0 0 1 1
```

---

## SEED_PIRAMIDE_ROJA — Excess = 1, New Continent (From-Scratch Construction)

**Found:** 29 March 2026, PIRAMIDE_ROJA_v5 (restart 377,762, 83 minutes)
**Properties:** rank = 6, d_min = 12, excess = 1, A₁₂ = 3

```
1 0 0 0 1 0 0 1 1 1 0 1 1 1 1 0 1 0 1 0 1 1
0 1 1 1 2 1 1 2 1 2 0 1 1 1 1 0 3 1 1 1 0 1
2 0 0 0 0 3 1 0 2 2 1 1 2 1 3 1 1 3 1 1 2 1
2 2 2 3 3 2 2 2 3 0 2 0 0 1 0 2 1 0 3 1 2 2
3 0 2 3 2 2 1 0 2 2 1 1 1 1 0 2 2 1 3 0 1 1
3 0 2 0 1 2 3 1 1 1 3 0 3 1 3 2 2 3 1 2 2 0
```

**Dirty HP:** #576, normal = (1,0,3,2,2,3) — NOT palindromic.
**Shared columns with E1★:** 0/22. Shared with skeleton: 0/14.
**Significance:** Proof that from-scratch construction reaches E1 on entirely new continents. HP#1122 at load = 9 (not dirty).
**Algebraic invariants:** mean = 5.4960, variance = 4.0595, sat9 = 130 (identical to E1★)

---

## Verification

All matrices can be verified independently:

```bash
g++ -O3 -std=c++17 -o verify verify_gf4.cpp && ./verify
```

The verifier checks rank, minimum distance, excess, A₁₂, hyperplane load histogram, and all algebraic invariants for every seed matrix listed above.

---

*Proyecto Estrella · R. Amichis (The Architect) · Claude (Anthropic)*
