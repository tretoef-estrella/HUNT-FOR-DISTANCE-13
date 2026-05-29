// ESTRELLA_PORMISCOJONES_CANON_v1.cpp
// =============================================================================
// PHASE 1 — Canonical pair catalogue under Aut_Mon(seed)
//
// PURPOSE
//   Enumerate the orbits of ORDERED pairs {a,b} (1<=a<b<=1023) of AG(5,4)
//   columns under Aut_Mon(seed), and write one canonical representative of
//   each orbit to a .cat file. This catalogue drives Phase 2 (the SCIP
//   pair-dispatcher) — instead of launching B&B over 1024 binary vars, we
//   launch 1 small SCIP per canonical pair with x_a=x_b=1 forced. The orbit
//   quotient divides the work by up to |Aut_Mon|=288 on B10.
//
// STRATEGY RATIONALE
//   - Every seed has |Aut_Mon| automorphisms fixing its code.
//   - AUTMON_v3 (22 Apr) used this at depth 1 of DFS, but the stabilizer
//     collapses after placement 1, so DFS symmetry is weak past depth 2.
//   - The right move is to FIX 2 columns AHEAD of SCIP. With depth-2
//     orbit-reduction the branching problem sees |Aut_Mon|-fold fewer
//     subtrees right at the root.
//   - F17 (LP invariance across seeds) is NOT a blocker here: F17 was about
//     NO columns fixed. We force x_a=x_b=1 simultaneously — F17c only tried
//     single-column forcing. Pair-forcing collapses many slice constraints
//     hard, often producing LP infeasibility immediately.
//
// WHAT THIS ENGINE PRODUCES
//   Writes `pairs_<LABEL>.cat` with format (space-separated):
//       # header lines starting with #
//       orbit_id  a  b  orbit_size  dead_flag  a_vec  b_vec
//   dead_flag=1 iff (seed_j, a, b) collinear for some j∈1..9 in PG(5,4)
//   (the Arc Theorem forbids any such triple — these pairs are pre-killed).
//   Equivalent test: a⊕b = λ·seed_j for some λ∈{1,2,3}.
//
// BUILD
//   g++ -O3 -march=native -std=c++17 -funroll-loops \
//       ESTRELLA_PORMISCOJONES_CANON_v1.cpp -o PORMISCOJONES_CANON
//
// RUN (Mac M2, <25% CPU, caffeinated)
//   caffeinate -dims nice -n 10 ./PORMISCOJONES_CANON B10 \
//       2>&1 | tee pormiscojones_canon_B10.log
//
// =============================================================================

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <chrono>
#include <algorithm>

// ------------------------------ GF(4) ----------------------------------------
static const uint8_t ADD_GF[4][4] = {
    {0,1,2,3},{1,0,3,2},{2,3,0,1},{3,2,1,0}
};
static const uint8_t MUL[4][4] = {
    {0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}
};
static inline uint8_t gf_mul(uint8_t a, uint8_t b) { return MUL[a][b]; }
static inline uint8_t gf_add(uint8_t a, uint8_t b) { return ADD_GF[a][b]; }

// ------------------------------ Globals --------------------------------------
static const int N_SEED = 9;
static const int K      = 5;
static const int AG_N   = 1024;

static uint8_t SEED_COL[N_SEED][K];
static uint8_t AG_VEC[AG_N][K];

// ------------------------------ Catalogue ------------------------------------
struct SeedDef {
    const char* label;
    uint8_t P[K][4];
    int aut_expected;
};
static const SeedDef SEEDS[] = {
    {"B01", {{1,2,0,3},{1,1,3,2},{1,1,2,3},{1,0,1,1},{0,1,1,1}}, 72},
    {"B02", {{1,2,1,0},{1,1,2,1},{1,1,0,2},{1,0,1,1},{0,1,1,1}}, 3},
    {"B03", {{1,2,1,2},{1,1,2,0},{1,1,0,3},{1,0,1,1},{0,1,1,1}}, 6},
    {"B06", {{1,1,3,2},{1,1,2,0},{1,0,1,2},{0,1,2,3},{0,1,1,1}}, 96},
    {"B08", {{1,1,0,1},{1,0,2,3},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, 6},
    {"B09", {{1,1,1,2},{1,0,2,3},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, 36},
    {"B10", {{1,1,1,0},{1,1,0,1},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, 288},
    {"B11", {{1,1,0,2},{1,0,1,1},{0,1,3,2},{0,1,2,3},{0,1,1,1}}, 18},
    {"B12", {{1,0,2,3},{1,0,1,1},{0,1,3,2},{0,1,2,3},{0,1,1,1}}, 72}
};
static const int N_SEEDS = sizeof(SEEDS)/sizeof(SEEDS[0]);

// ------------------------------ Seed loading ---------------------------------
static void load_seed(const SeedDef* s) {
    uint8_t SEED_G[K][N_SEED];
    memset(SEED_G, 0, sizeof(SEED_G));
    for (int i = 0; i < K; ++i) SEED_G[i][i] = 1;
    for (int i = 0; i < K; ++i)
        for (int j = 0; j < 4; ++j)
            SEED_G[i][K + j] = s->P[i][j];
    for (int j = 0; j < N_SEED; ++j)
        for (int i = 0; i < K; ++i)
            SEED_COL[j][i] = SEED_G[i][j];
}

// ------------------------------ AG encoding ----------------------------------
static int enc_ag(const uint8_t v[K]) {
    int idx = 0;
    for (int d = K - 1; d >= 0; --d) idx = idx * 4 + v[d];
    return idx;
}
static void dec_ag(int idx, uint8_t v[K]) {
    for (int d = 0; d < K; ++d) { v[d] = idx & 3; idx >>= 2; }
}
static void precompute_AG() {
    for (int i = 0; i < AG_N; ++i) dec_ag(i, AG_VEC[i]);
}

// ------------------------------ Aut_Mon --------------------------------------
struct AutElem { uint8_t M[K][K]; };
static std::vector<AutElem> AUT;

static void mat_vec(const uint8_t M[K][K], const uint8_t v[K], uint8_t out[K]) {
    for (int i = 0; i < K; ++i) {
        uint8_t s = 0;
        for (int j = 0; j < K; ++j) s ^= gf_mul(M[i][j], v[j]);
        out[i] = s;
    }
}
static bool are_LI(const uint8_t cols[K][K]) {
    uint8_t A[K][K];
    for (int j = 0; j < K; ++j) for (int i = 0; i < K; ++i) A[i][j] = cols[i][j];
    for (int c = 0; c < K; ++c) {
        int piv = -1;
        for (int r = c; r < K; ++r) if (A[r][c] != 0) { piv = r; break; }
        if (piv < 0) return false;
        if (piv != c) for (int j = 0; j < K; ++j) std::swap(A[c][j], A[piv][j]);
        uint8_t inv;
        switch (A[c][c]) {
            case 1: inv = 1; break;
            case 2: inv = 3; break;
            case 3: inv = 2; break;
            default: return false;
        }
        for (int j = 0; j < K; ++j) A[c][j] = gf_mul(A[c][j], inv);
        for (int r = 0; r < K; ++r) if (r != c && A[r][c] != 0) {
            uint8_t f = A[r][c];
            for (int j = 0; j < K; ++j) A[r][j] ^= gf_mul(f, A[c][j]);
        }
    }
    return true;
}
static void build_M_from_cols(const uint8_t target[K][K], uint8_t M[K][K]) {
    for (int i = 0; i < K; ++i)
        for (int j = 0; j < K; ++j)
            M[i][j] = target[j][i];
}
static bool is_scalar_multiple_of_seed_col(const uint8_t v[K], int& out_j, uint8_t& out_lambda) {
    bool z = true;
    for (int d = 0; d < K; ++d) if (v[d]) { z = false; break; }
    if (z) return false;
    for (int j = 0; j < N_SEED; ++j) {
        int d0 = -1;
        for (int d = 0; d < K; ++d) if (SEED_COL[j][d]) { d0 = d; break; }
        if (d0 < 0) continue;
        uint8_t sj = SEED_COL[j][d0], vj = v[d0];
        if (vj == 0) continue;
        uint8_t sj_inv = (sj == 1) ? 1 : (sj == 2 ? 3 : 2);
        uint8_t lambda = gf_mul(vj, sj_inv);
        bool ok = true;
        for (int d = 0; d < K; ++d)
            if (gf_mul(lambda, SEED_COL[j][d]) != v[d]) { ok = false; break; }
        if (ok) { out_j = j; out_lambda = lambda; return true; }
    }
    return false;
}
static void compute_aut_mon() {
    fprintf(stderr, "[AUT] enumerating Aut_Mon(seed) candidates...\n"); fflush(stderr);
    AUT.clear();
    uint8_t picks[K][K];
    auto try_tuple = [&](int perm[K], uint8_t lam[K]) {
        for (int i = 0; i < K; ++i)
            for (int d = 0; d < K; ++d)
                picks[i][d] = gf_mul(lam[i], SEED_COL[perm[i]][d]);
        if (!are_LI(picks)) return;
        uint8_t M[K][K];
        build_M_from_cols(picks, M);
        int used[N_SEED] = {0};
        bool ok = true;
        for (int j = 0; j < N_SEED; ++j) {
            uint8_t img[K];
            mat_vec(M, SEED_COL[j], img);
            int tj; uint8_t tl;
            if (!is_scalar_multiple_of_seed_col(img, tj, tl)) { ok = false; break; }
            if (used[tj]) { ok = false; break; }
            used[tj] = 1;
        }
        if (ok) { AutElem e; memcpy(e.M, M, sizeof(M)); AUT.push_back(e); }
    };
    int c[K];
    for (c[0] = 0; c[0] < N_SEED; ++c[0])
    for (c[1] = 0; c[1] < N_SEED; ++c[1]) if (c[1] != c[0])
    for (c[2] = 0; c[2] < N_SEED; ++c[2]) if (c[2] != c[0] && c[2] != c[1])
    for (c[3] = 0; c[3] < N_SEED; ++c[3]) if (c[3] != c[0] && c[3] != c[1] && c[3] != c[2])
    for (c[4] = 0; c[4] < N_SEED; ++c[4]) {
        if (c[4] == c[0] || c[4] == c[1] || c[4] == c[2] || c[4] == c[3]) continue;
        uint8_t lam[K];
        for (lam[0] = 1; lam[0] < 4; ++lam[0])
        for (lam[1] = 1; lam[1] < 4; ++lam[1])
        for (lam[2] = 1; lam[2] < 4; ++lam[2])
        for (lam[3] = 1; lam[3] < 4; ++lam[3])
        for (lam[4] = 1; lam[4] < 4; ++lam[4])
            try_tuple(c, lam);
    }
    fprintf(stderr, "[AUT] |Aut_Mon| = %zu\n", AUT.size()); fflush(stderr);
}

// ------------------------------ Canonical pair enumeration -------------------
static std::vector<std::vector<int>> AG_ACTION;

static void precompute_action() {
    AG_ACTION.assign(AUT.size(), std::vector<int>(AG_N));
    for (size_t g = 0; g < AUT.size(); ++g)
        for (int c = 0; c < AG_N; ++c) {
            uint8_t img[K];
            mat_vec(AUT[g].M, AG_VEC[c], img);
            AG_ACTION[g][c] = enc_ag(img);
        }
}

// ARC Theorem: a pair (a,b) with a⊕b = λ·seed_j for some λ∈{1,2,3}, j∈0..8
// is seed-collinear (placing both would produce a PG(5,4)-collinear triple
// with seed_j). Arc Theorem forbids this. Such pairs are DEAD.
static bool is_seed_collinear_pair(int a, int b) {
    if (a == 0 || b == 0) return false;
    uint8_t diff[K];
    for (int d = 0; d < K; ++d) diff[d] = gf_add(AG_VEC[a][d], AG_VEC[b][d]);
    int tj; uint8_t tl;
    return is_scalar_multiple_of_seed_col(diff, tj, tl);
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL); setbuf(stderr, NULL);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <seed_label>\n", argv[0]);
        fprintf(stderr, "  seed_label in: ");
        for (int i = 0; i < N_SEEDS; ++i) fprintf(stderr, "%s ", SEEDS[i].label);
        fprintf(stderr, "\n");
        return 1;
    }
    const SeedDef* s = nullptr;
    for (int i = 0; i < N_SEEDS; ++i)
        if (strcmp(SEEDS[i].label, argv[1]) == 0) { s = &SEEDS[i]; break; }
    if (!s) { fprintf(stderr, "Unknown seed: %s\n", argv[1]); return 1; }

    fprintf(stderr, "=======================================================\n");
    fprintf(stderr, "ESTRELLA_PORMISCOJONES_CANON_v1 — seed %s\n", s->label);
    fprintf(stderr, "Expected |Aut_Mon| = %d\n", s->aut_expected);
    fprintf(stderr, "=======================================================\n");
    fflush(stderr);

    auto t_start = std::chrono::steady_clock::now();

    fprintf(stderr, "[1/5] Loading seed %s...\n", s->label); fflush(stderr);
    load_seed(s);

    fprintf(stderr, "[2/5] Precomputing AG vectors (1024 pts in GF(4)^5)...\n"); fflush(stderr);
    precompute_AG();

    fprintf(stderr, "[3/5] Computing Aut_Mon(seed)...\n"); fflush(stderr);
    compute_aut_mon();

    if ((int)AUT.size() != s->aut_expected) {
        fprintf(stderr, "[ERROR] |Aut_Mon| = %zu but expected %d. ABORT.\n",
                AUT.size(), s->aut_expected);
        return 2;
    }

    fprintf(stderr, "[4/5] Precomputing action table (%zu x 1024)...\n", AUT.size());
    fflush(stderr);
    precompute_action();

    fprintf(stderr, "[5/5] Canonical pair enumeration + seed-collinearity pruning...\n");
    fflush(stderr);

    char fname[64];
    snprintf(fname, sizeof(fname), "pairs_%s.cat", s->label);
    FILE* fc = fopen(fname, "w");
    if (!fc) { fprintf(stderr, "[ERROR] cannot write %s\n", fname); return 3; }

    fprintf(fc, "# ESTRELLA_PORMISCOJONES_CANON_v1 — canonical pair catalogue\n");
    fprintf(fc, "# Seed: %s, |Aut_Mon|=%zu\n", s->label, AUT.size());
    fprintf(fc, "# Columns: orbit_id  a  b  orbit_size  dead_flag  a_vec  b_vec\n");
    fprintf(fc, "# dead_flag=1 iff (seed_j,a,b) collinear for some j (Arc Theorem skip)\n");
    fprintf(fc, "#\n");

    // Enumerate canonical pairs.
    const uint64_t NK = (uint64_t)AG_N * AG_N;
    std::vector<uint64_t> visited(NK / 64, 0ULL);
    auto is_visited = [&](uint64_t k) -> bool {
        return (visited[k >> 6] >> (k & 63)) & 1ULL;
    };
    auto set_visited = [&](uint64_t k) {
        visited[k >> 6] |= (1ULL << (k & 63));
    };
    auto pkey = [&](int a, int b) -> uint64_t {
        return (uint64_t)a * AG_N + (uint64_t)b;
    };

    long long total = (long long)(AG_N - 1) * (AG_N - 2) / 2;
    long long n_orb = 0, n_dead = 0;
    long long sum_orbit_size = 0;
    int min_orbit = INT32_MAX, max_orbit = 0;

    auto t0 = std::chrono::steady_clock::now();
    for (int a = 1; a < AG_N; ++a) {
        for (int b = a + 1; b < AG_N; ++b) {
            uint64_t k0 = pkey(a, b);
            if (is_visited(k0)) continue;

            // Walk the orbit.
            std::vector<uint64_t> orb;
            orb.push_back(k0);
            set_visited(k0);
            for (size_t idx = 0; idx < orb.size(); ++idx) {
                uint64_t kk = orb[idx];
                int aa = (int)(kk / AG_N), bb = (int)(kk % AG_N);
                for (size_t g = 0; g < AUT.size(); ++g) {
                    int ga = AG_ACTION[g][aa];
                    int gb = AG_ACTION[g][bb];
                    if (ga == 0 || gb == 0) continue;
                    int x = (ga < gb) ? ga : gb;
                    int y = (ga < gb) ? gb : ga;
                    uint64_t kn = pkey(x, y);
                    if (!is_visited(kn)) {
                        set_visited(kn);
                        orb.push_back(kn);
                    }
                }
            }

            int osize = (int)orb.size();
            sum_orbit_size += osize;
            if (osize < min_orbit) min_orbit = osize;
            if (osize > max_orbit) max_orbit = osize;

            bool dead = is_seed_collinear_pair(a, b);
            if (dead) ++n_dead;

            fprintf(fc, "%lld %d %d %d %d", n_orb, a, b, osize, dead ? 1 : 0);
            fprintf(fc, " [%d,%d,%d,%d,%d]",
                    AG_VEC[a][0], AG_VEC[a][1], AG_VEC[a][2], AG_VEC[a][3], AG_VEC[a][4]);
            fprintf(fc, " [%d,%d,%d,%d,%d]\n",
                    AG_VEC[b][0], AG_VEC[b][1], AG_VEC[b][2], AG_VEC[b][3], AG_VEC[b][4]);
            ++n_orb;
        }
    }
    fclose(fc);

    auto t1 = std::chrono::steady_clock::now();
    double el_pair = std::chrono::duration<double>(t1 - t0).count();

    fprintf(stderr, "[CAT] wrote %s\n", fname);
    fprintf(stderr, "[CAT] total pairs (non-origin)         = %lld\n", total);
    fprintf(stderr, "[CAT] canonical orbits                  = %lld\n", n_orb);
    fprintf(stderr, "[CAT] dead (seed-collinear, Arc-killed) = %lld\n", n_dead);
    fprintf(stderr, "[CAT] LIVE canonical pairs to attack    = %lld\n", n_orb - n_dead);
    fprintf(stderr, "[CAT] orbit size: min=%d  max=%d  mean=%.2f\n",
            min_orbit, max_orbit, (double)sum_orbit_size / n_orb);
    fprintf(stderr, "[CAT] consistency: sum_orbit_sizes=%lld vs C(1023,2)=%lld (%s)\n",
            sum_orbit_size, total, (sum_orbit_size == total ? "OK" : "FAIL"));
    fprintf(stderr, "[CAT] enumeration time: %.2fs\n", el_pair);

    auto t_end = std::chrono::steady_clock::now();
    double el = std::chrono::duration<double>(t_end - t_start).count();
    fprintf(stderr, "=======================================================\n");
    fprintf(stderr, "TOTAL wall-clock: %.2fs\n", el);
    fprintf(stderr, "OUTPUT: %s\n", fname);
    fprintf(stderr, "=======================================================\n");

    return 0;
}
