// ESTRELLA_PORMISCOJONES_PAIR_SCIP_GEOM_v2_PROBE2.cpp
// =============================================================================
// PHASE 2 + PROBING DEPTH-2 — Pair-forcing SCIP dispatcher with binary
//                              clause generation by depth-2 failed-pair probing.
//
// HEREDA exactamente la formulación de
//   ESTRELLA_PORMISCOJONES_PAIR_SCIP_GEOM_v1.cpp
// (misma GF(4), mismas SEED_COL/AG_VEC, mismo emit_lp_for_pair, mismo run_scip).
// Único cambio: ANTES de emitir el .lp y llamar a SCIP, ejecutamos:
//
//   1. Apply x_0=x_a=x_b=1 + ARC kills (pair + seed).
//   2. Cascade GEOM saturation (slice-cap saturation + per-message tightening).
//      → estado coherente, varias columnas ya forzadas a 0 o 1.
//   3. PROBING DEPTH-2: para cada par (c1, c2) de columnas libres,
//      tentativamente forzar ambas a 1, correr cascade en sandbox. Si la
//      cascada deriva infeasibilidad, **emitimos un cut binario** al .lp:
//          x_c1 + x_c2 <= 1     (∀ tales pares)
//      Esto produce miles de cláusulas binarias que NO están en la formulación
//      4092-affine GEOM, atacan directamente la geometría de pares (V-out-of-V
//      asymmetry de F19), y aprietan la relajación LP en una forma que ningún
//      cut estático ha logrado.
//   4. Emitir .lp con (a) los 4092 affine GEOM, (b) los CUTS_BINARY de probing,
//      (c) los kills/forcings ya derivados por cascade (incluyendo cascada-1s).
//   5. Llamar a SCIP. Verdict normal: INFEAS/FEASIBLE/TIMEOUT.
//
// COSTE: ~C(n_free, 2) cascadas por par. Para n_free≈700 ⇒ ~245k cascadas.
//        Cascade individual ~0.3-1ms ⇒ 75-250s probing antes de cada SCIP call.
//        Sólo se justifica para pairs HARD (cluster pairs); para sanity rápido
//        usar --skip-probe.
//
// USO ESTRATÉGICO (4 Sabor B de B06 en ~15 min cada uno):
//   caffeinate -dims nice -n 10 ./PORMISCOJONES_PROBE2 B06 600 --single 66,90  2>&1 | tee probe2_B06_66_90.log
//   caffeinate -dims nice -n 10 ./PORMISCOJONES_PROBE2 B06 600 --single 66,95  2>&1 | tee probe2_B06_66_95.log
//   caffeinate -dims nice -n 10 ./PORMISCOJONES_PROBE2 B06 600 --single 66,166 2>&1 | tee probe2_B06_66_166.log
//   caffeinate -dims nice -n 10 ./PORMISCOJONES_PROBE2 B06 600 --single 66,183 2>&1 | tee probe2_B06_66_183.log
//
// BUILD
//   g++ -O3 -march=native -std=c++17 -funroll-loops \
//       ESTRELLA_PORMISCOJONES_PAIR_SCIP_GEOM_v2_PROBE2.cpp -o PORMISCOJONES_PROBE2
//
// FLAGS NUEVAS
//   --skip-probe          (omite probing; equivalente a v1 GEOM)
//   --probe-budget S      (máx segundos para probing, default 300)
//   --probe-only          (sale tras probing, no llama a SCIP; reporta cuts)
//
// =============================================================================

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

// ============================ GF(4) ==========================================
static const uint8_t ADD_GF[4][4] = {
    {0,1,2,3},{1,0,3,2},{2,3,0,1},{3,2,1,0}
};
static const uint8_t MUL[4][4] = {
    {0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}
};
static inline uint8_t gf_mul(uint8_t a, uint8_t b) { return MUL[a][b]; }
static inline uint8_t gf_add(uint8_t a, uint8_t b) { return ADD_GF[a][b]; }

// ============================ Globals ========================================
static const int N_SEED = 9;
static const int K      = 5;
static const int AG_N   = 1024;

static uint8_t SEED_COL[N_SEED][K];
static uint8_t AG_VEC[AG_N][K];

// Per-message GEOM cap (independent of alpha)
static int CAP_M[AG_N];     // CAP_M[m] = 9 - sc[m,0]; m=1..1023; CAP_M[0] unused

// Slice membership: SLICE[m][c] = dot(m,c) in GF(4)
static uint8_t SLICE[AG_N][AG_N];   // ~1MB

// Per-(m,alpha) point list
static std::vector<int> SLICE_LIST[AG_N][4];

// ============================ Catalogue (idéntica) ===========================
struct SeedDef {
    const char* label;
    uint8_t P[K][4];
    const char* structure;
};
static const SeedDef SEEDS[] = {
    {"B01", {{1,2,0,3},{1,1,3,2},{1,1,2,3},{1,0,1,1},{0,1,1,1}}, "CLEAN"},
    {"B02", {{1,2,1,0},{1,1,2,1},{1,1,0,2},{1,0,1,1},{0,1,1,1}}, "CLEAN"},
    {"B03", {{1,2,1,2},{1,1,2,0},{1,1,0,3},{1,0,1,1},{0,1,1,1}}, "CLEAN"},
    {"B06", {{1,1,3,2},{1,1,2,0},{1,0,1,2},{0,1,2,3},{0,1,1,1}}, "CLEAN"},
    {"B08", {{1,1,0,1},{1,0,2,3},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, "CLEAN"},
    {"B09", {{1,1,1,2},{1,0,2,3},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, "FROB_PAIR"},
    {"B10", {{1,1,1,0},{1,1,0,1},{1,0,1,1},{0,1,2,3},{0,1,1,1}}, "CLEAN"},
    {"B11", {{1,1,0,2},{1,0,1,1},{0,1,3,2},{0,1,2,3},{0,1,1,1}}, "CLEAN"},
    {"B12", {{1,0,2,3},{1,0,1,1},{0,1,3,2},{0,1,2,3},{0,1,1,1}}, "CLEAN_CLOSED"}
};
static const int N_SEEDS = sizeof(SEEDS)/sizeof(SEEDS[0]);

// ============================ Encoding (idéntica) ============================
static int enc_ag(const uint8_t v[K]) {
    int idx = 0;
    for (int d = K - 1; d >= 0; --d) idx = idx * 4 + v[d];
    return idx;
}
static void dec_ag(int idx, uint8_t v[K]) {
    for (int d = 0; d < K; ++d) { v[d] = idx & 3; idx >>= 2; }
}
static uint8_t dot(const uint8_t a[K], const uint8_t b[K]) {
    uint8_t s = 0;
    for (int d = 0; d < K; ++d) s ^= gf_mul(a[d], b[d]);
    return s;
}
static int dot_idx(int m_idx, int c_idx) {
    return dot(AG_VEC[m_idx], AG_VEC[c_idx]);
}
static void precompute_AG() {
    for (int i = 0; i < AG_N; ++i) dec_ag(i, AG_VEC[i]);
}
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
static void precompute_CAP_M() {
    CAP_M[0] = 0;
    for (int m = 1; m < AG_N; ++m) {
        int sc0 = 0;
        for (int j = 0; j < N_SEED; ++j)
            if (dot(AG_VEC[m], SEED_COL[j]) == 0) ++sc0;
        CAP_M[m] = 9 - sc0;
    }
}
static void precompute_SLICE() {
    for (int m = 0; m < AG_N; ++m)
        for (int c = 0; c < AG_N; ++c)
            SLICE[m][c] = (uint8_t)dot_idx(m, c);
}
static void precompute_SLICE_LIST() {
    for (int m = 1; m < AG_N; ++m) {
        for (int a = 0; a < 4; ++a) SLICE_LIST[m][a].reserve(256);
        for (int c = 0; c < AG_N; ++c) {
            SLICE_LIST[m][SLICE[m][c]].push_back(c);
        }
    }
}

// ============================ Logger =========================================
static auto T0 = std::chrono::steady_clock::now();
static double secs_since_start() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - T0).count();
}
static void logf(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    fprintf(stderr, "[%7.2fs] ", secs_since_start());
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fflush(stderr);
    va_end(ap);
}

// ============================ State + cascade ================================
// State: forced[c] in {0=free, 1, -1}; per-slice counts maintained incrementally.
struct State {
    int8_t forced[AG_N];
    int k1[AG_N][4];
    int kfree[AG_N][4];
    int total_forced_1;
    int total_free;

    void init_blank() {
        memset(forced, 0, sizeof(forced));
        total_forced_1 = 0;
        total_free = AG_N;
        for (int m = 1; m < AG_N; ++m)
            for (int a = 0; a < 4; ++a) {
                k1[m][a] = 0;
                kfree[m][a] = (int)SLICE_LIST[m][a].size();
            }
    }
};

static bool set_one(State& S, int c) {
    if (S.forced[c] == 1) return true;
    if (S.forced[c] == -1) return false;
    S.forced[c] = 1;
    S.total_forced_1++;
    S.total_free--;
    for (int m = 1; m < AG_N; ++m) {
        int a = SLICE[m][c];
        S.k1[m][a]++;
        S.kfree[m][a]--;
        if (S.k1[m][a] > CAP_M[m]) return false;
    }
    return true;
}
static bool set_zero(State& S, int c) {
    if (S.forced[c] == -1) return true;
    if (S.forced[c] == 1) return false;
    S.forced[c] = -1;
    S.total_free--;
    for (int m = 1; m < AG_N; ++m) {
        int a = SLICE[m][c];
        S.kfree[m][a]--;
    }
    return true;
}

// Cascade GEOM saturation (slice-saturation + per-m tightening). Returns false on infeas.
static bool cascade_geom(State& S, long long& passes) {
    while (true) {
        bool progress = false;
        passes++;

        // Rule A: slice cap saturation
        for (int m = 1; m < AG_N; ++m) {
            for (int a = 0; a < 4; ++a) {
                if (S.k1[m][a] > CAP_M[m]) return false;
                if (S.k1[m][a] == CAP_M[m] && S.kfree[m][a] > 0) {
                    for (int c : SLICE_LIST[m][a]) {
                        if (S.forced[c] == 0) {
                            if (!set_zero(S, c)) return false;
                            progress = true;
                        }
                    }
                }
            }
        }

        // Rule B: per-message exact need (sum_alpha picks = 13 - sum_alpha k1)
        for (int m = 1; m < AG_N; ++m) {
            int total_k1 = 0;
            int per_a_remcap[4], per_a_free[4];
            for (int a = 0; a < 4; ++a) {
                total_k1 += S.k1[m][a];
                per_a_remcap[a] = CAP_M[m] - S.k1[m][a];
                if (per_a_remcap[a] < 0) return false;
                per_a_free[a] = S.kfree[m][a];
            }
            int needed = 13 - total_k1;
            if (needed < 0) return false;
            int total_avail = 0, total_free_m = 0;
            for (int a = 0; a < 4; ++a) {
                total_avail += std::min(per_a_remcap[a], per_a_free[a]);
                total_free_m += per_a_free[a];
            }
            if (needed > total_avail) return false;

            if (needed == total_free_m && total_free_m > 0) {
                for (int a = 0; a < 4; ++a) {
                    for (int c : SLICE_LIST[m][a]) {
                        if (S.forced[c] == 0) {
                            if (!set_one(S, c)) return false;
                            progress = true;
                        }
                    }
                }
                continue;
            }

            for (int a = 0; a < 4; ++a) {
                int max_elsewhere = 0;
                for (int b = 0; b < 4; ++b) if (b != a)
                    max_elsewhere += std::min(per_a_remcap[b], per_a_free[b]);
                int min_here = needed - max_elsewhere;
                int max_here = std::min(per_a_remcap[a], per_a_free[a]);
                if (min_here < 0) min_here = 0;
                if (min_here > max_here) return false;
                if (min_here == per_a_free[a] && per_a_free[a] > 0) {
                    for (int c : SLICE_LIST[m][a]) {
                        if (S.forced[c] == 0) {
                            if (!set_one(S, c)) return false;
                            progress = true;
                        }
                    }
                }
                if (max_here == 0 && per_a_free[a] > 0) {
                    for (int c : SLICE_LIST[m][a]) {
                        if (S.forced[c] == 0) {
                            if (!set_zero(S, c)) return false;
                            progress = true;
                        }
                    }
                }
            }
        }

        if (!progress) break;
    }
    return true;
}

// ============================ ARC kills (idéntica al canonical) =============
static bool apply_arc_kills(State& S, const SeedDef* sd, int a, int b) {
    if (!set_one(S, 0)) return false;
    if (!set_one(S, a)) return false;
    if (!set_one(S, b)) return false;

    {
        uint8_t dab[K];
        for (int d = 0; d < K; ++d) dab[d] = gf_add(AG_VEC[a][d], AG_VEC[b][d]);
        for (uint8_t lam = 2; lam < 4; ++lam) {
            uint8_t c[K];
            for (int d = 0; d < K; ++d) c[d] = gf_add(AG_VEC[a][d], gf_mul(lam, dab[d]));
            int ci = enc_ag(c);
            if (ci != a && ci != b && ci != 0) {
                if (!set_zero(S, ci)) return false;
            }
        }
    }
    for (int j = 0; j < N_SEED; ++j) {
        for (int ex : {a, b}) {
            for (uint8_t lam = 1; lam < 4; ++lam) {
                uint8_t c[K];
                for (int d = 0; d < K; ++d)
                    c[d] = gf_add(AG_VEC[ex][d], gf_mul(lam, SEED_COL[j][d]));
                int ci = enc_ag(c);
                if (ci != a && ci != b && ci != 0) {
                    if (S.forced[ci] == 1) return false;
                    if (S.forced[ci] == 0)
                        if (!set_zero(S, ci)) return false;
                }
            }
        }
    }
    return true;
}

// ============================ Probing depth-2 ===============================
// For each pair (c1, c2) of currently free columns, sandbox-force both to 1
// and run cascade. If cascade derives infeasibility → emit binary cut
// x_c1 + x_c2 <= 1.
//
// Optimization: also exploit DEPTH-1 fixed points. After cascade on the base
// state, any free c such that (force c=1 → infeas) gets c=0 (probing depth-1).
// We do this opportunistically while iterating pairs: the first time we
// sandbox-force c1=1, if cascade fails, we add c1=0 to base state (and
// re-cascade base) and skip pairs with that c1.
//
// Time-bounded: stop iterating pairs when secs_since_probe_start > probe_budget.

struct ProbeStats {
    long long pairs_tried = 0;
    long long pairs_failed = 0;       // → binary cut emitted
    long long depth1_forced_zero = 0; // unary cut
    long long depth1_forced_one  = 0; // dual: if (c1=0 → infeas), c1=1
    double elapsed = 0.0;
    bool timed_out = false;
    bool base_infeas = false;
};

// Try forcing c=v in sandbox; return true if cascade closes infeasible.
static bool sandbox_force_infeasible(const State& S_base, int c, int v, long long& passes_inc) {
    State S = S_base; // ~ K1+kfree+forced, ~33KB; trivial copy
    bool ok = (v == 1) ? set_one(S, c) : set_zero(S, c);
    if (!ok) return true; // immediate conflict
    long long p = 0;
    bool cas_ok = cascade_geom(S, p);
    passes_inc += p;
    return !cas_ok;
}
static bool sandbox_pair_force_one_infeasible(const State& S_base, int c1, int c2, long long& passes_inc) {
    State S = S_base;
    if (!set_one(S, c1)) return true;
    if (!set_one(S, c2)) return true;
    long long p = 0;
    bool cas_ok = cascade_geom(S, p);
    passes_inc += p;
    return !cas_ok;
}

static void run_probing_depth_2(
    State& S_base,
    std::vector<std::pair<int,int>>& binary_cuts,
    std::vector<int>& unary_zeros,
    std::vector<int>& unary_ones,
    double probe_budget_s,
    ProbeStats& PS)
{
    double t_probe_start = secs_since_start();
    long long cascade_count = 0;

    // Phase 1: depth-1 probing on each remaining free.
    //   For each free c: try c=1; if infeas → c forced to 0 in base.
    //                    else try c=0; if infeas → c forced to 1 in base.
    //   Re-cascade base after each fix and re-collect frees.
    bool fixed_something;
    int phase1_iters = 0;
    do {
        fixed_something = false;
        phase1_iters++;
        std::vector<int> frees;
        frees.reserve(AG_N);
        for (int c = 0; c < AG_N; ++c) if (S_base.forced[c] == 0) frees.push_back(c);
        for (int c : frees) {
            if (S_base.forced[c] != 0) continue; // got fixed by cascade in this pass
            if (secs_since_start() - t_probe_start > probe_budget_s * 0.20) break; // ≤20% budget on D1
            if (sandbox_force_infeasible(S_base, c, 1, cascade_count)) {
                // c=1 → infeas, force c=0
                if (!set_zero(S_base, c)) { PS.base_infeas = true; return; }
                long long p = 0;
                if (!cascade_geom(S_base, p)) { PS.base_infeas = true; return; }
                cascade_count += p;
                unary_zeros.push_back(c);
                PS.depth1_forced_zero++;
                fixed_something = true;
            } else if (sandbox_force_infeasible(S_base, c, 0, cascade_count)) {
                // c=0 → infeas, force c=1
                if (!set_one(S_base, c)) { PS.base_infeas = true; return; }
                long long p = 0;
                if (!cascade_geom(S_base, p)) { PS.base_infeas = true; return; }
                cascade_count += p;
                unary_ones.push_back(c);
                PS.depth1_forced_one++;
                fixed_something = true;
            }
        }
        logf("PROBE D1 pass %d: forced0+=%lld forced1+=%lld free=%d cascade_count=%lld",
             phase1_iters, PS.depth1_forced_zero, PS.depth1_forced_one,
             S_base.total_free, cascade_count);
    } while (fixed_something && phase1_iters < 5);

    // Phase 2: depth-2 probing pairs of remaining frees.
    std::vector<int> frees;
    frees.reserve(AG_N);
    for (int c = 0; c < AG_N; ++c) if (S_base.forced[c] == 0) frees.push_back(c);
    int n_free = (int)frees.size();
    long long total_pairs = (long long)n_free * (long long)(n_free - 1) / 2;
    logf("PROBE D2 START: n_free=%d total_pairs=%lld budget_remaining=%.0fs",
         n_free, total_pairs, probe_budget_s - (secs_since_start() - t_probe_start));

    long long log_every = 5000;
    for (int i = 0; i < n_free; ++i) {
        if (S_base.forced[frees[i]] != 0) continue;
        for (int j = i + 1; j < n_free; ++j) {
            if (S_base.forced[frees[j]] != 0) continue;
            if (secs_since_start() - t_probe_start > probe_budget_s) {
                PS.timed_out = true;
                goto probe_done;
            }
            PS.pairs_tried++;
            if (sandbox_pair_force_one_infeasible(S_base, frees[i], frees[j], cascade_count)) {
                binary_cuts.push_back({frees[i], frees[j]});
                PS.pairs_failed++;
            }
            if (PS.pairs_tried % log_every == 0) {
                double el = secs_since_start() - t_probe_start;
                double rate = PS.pairs_tried / el;
                long long remain = total_pairs - PS.pairs_tried;
                double eta = remain / rate;
                logf("PROBE D2: tried=%lld/%lld failed=%lld (%.2f%%) "
                     "rate=%.0fpairs/s elapsed=%.0fs eta=%.0fs",
                     PS.pairs_tried, total_pairs, PS.pairs_failed,
                     100.0 * PS.pairs_failed / std::max(1ll, PS.pairs_tried),
                     rate, el, eta);
            }
        }
    }
probe_done:
    PS.elapsed = secs_since_start() - t_probe_start;
    logf("PROBE END: pairs_tried=%lld pairs_failed=%lld depth1_zeros=%lld depth1_ones=%lld "
         "elapsed=%.1fs cascade_count=%lld base_infeas=%d timed_out=%d",
         PS.pairs_tried, PS.pairs_failed, PS.depth1_forced_zero, PS.depth1_forced_one,
         PS.elapsed, cascade_count, (int)PS.base_infeas, (int)PS.timed_out);
}

// ============================ LP emission =====================================
struct LPStats {
    int n_aff_added, n_aff_dedup, n_aff_trivial;
    int n_forced_one, n_forced_zero;
    int n_binary_cuts;
};

static std::string hashkey(const std::vector<int>& v, int k) {
    std::string s; s.reserve(v.size()*6+8);
    char buf[32]; snprintf(buf,sizeof(buf),"k=%d|",k); s += buf;
    for (int x : v) { snprintf(buf,sizeof(buf),"%d,",x); s += buf; }
    return s;
}

// Emit .lp using current State (which already has cascade + probing applied):
//   - All forced=1 → unit constraint xC = 1
//   - All forced=-1 → unit constraint xC = 0
//   - Sum x_c = 13 over all c
//   - 4092 affine GEOM constraints (deduped, trivial dropped)
//   - Binary cuts: x_c1 + x_c2 <= 1
static LPStats emit_lp_from_state(
    FILE* f, const SeedDef* s, int a, int b,
    const State& S,
    const std::vector<std::pair<int,int>>& binary_cuts)
{
    LPStats st = {};
    int n_forced_1 = 0, n_forced_0 = 0;
    for (int c = 0; c < AG_N; ++c) {
        if (S.forced[c] == 1) n_forced_1++;
        else if (S.forced[c] == -1) n_forced_0++;
    }
    st.n_forced_one = n_forced_1;
    st.n_forced_zero = n_forced_0;
    st.n_binary_cuts = (int)binary_cuts.size();

    fprintf(f, "\\ ESTRELLA_PORMISCOJONES_PAIR_SCIP_GEOM_v2_PROBE2 -- seed %s (%s), pair (a=%d, b=%d)\n",
            s->label, s->structure, a, b);
    fprintf(f, "\\ GEOM cap rule: cap[m,alpha] = 9 - sc[m,0]\n");
    fprintf(f, "\\ Probing depth-2 applied: %d binary cuts, forced1=%d forced0=%d\n",
            st.n_binary_cuts, n_forced_1, n_forced_0);
    fprintf(f, "Minimize\n");
    fprintf(f, " obj: x0\n");
    fprintf(f, "Subject To\n");

    // Forced ones
    int u_idx = 0;
    for (int c = 0; c < AG_N; ++c) {
        if (S.forced[c] == 1) {
            fprintf(f, " u%d: x%d = 1\n", u_idx++, c);
        }
    }
    // Forced zeros
    for (int c = 0; c < AG_N; ++c) {
        if (S.forced[c] == -1) {
            fprintf(f, " u%d: x%d = 0\n", u_idx++, c);
        }
    }

    // Sum = 13
    fprintf(f, " sum13:");
    for (int c = 0; c < AG_N; ++c) {
        if (c > 0) fprintf(f, " +");
        fprintf(f, " x%d", c);
    }
    fprintf(f, " = 13\n");

    // 4092 affine GEOM constraints (deduped)
    std::unordered_set<std::string> seen;
    int n_added = 0, n_dedup = 0, n_trivial = 0;
    for (int m = 1; m < AG_N; ++m) {
        int cap = CAP_M[m];
        for (int alpha = 0; alpha < 4; ++alpha) {
            const auto& cols = SLICE_LIST[m][alpha];
            int ws = cap;
            if ((int)cols.size() <= ws) { ++n_trivial; continue; }
            std::vector<int> sc = cols;
            std::sort(sc.begin(), sc.end());
            std::string key = hashkey(sc, ws);
            if (seen.count(key)) { ++n_dedup; continue; }
            seen.insert(key);
            fprintf(f, " c%d:", n_added);
            for (size_t i = 0; i < cols.size(); ++i) {
                if (i > 0) fprintf(f, " +");
                fprintf(f, " x%d", cols[i]);
            }
            fprintf(f, " <= %d\n", ws);
            ++n_added;
        }
    }
    st.n_aff_added = n_added;
    st.n_aff_dedup = n_dedup;
    st.n_aff_trivial = n_trivial;

    // Binary cuts from probing depth-2
    int b_idx = 0;
    for (auto& pr : binary_cuts) {
        fprintf(f, " bc%d: x%d + x%d <= 1\n", b_idx++, pr.first, pr.second);
    }

    // Binaries
    fprintf(f, "Binaries\n");
    for (int c = 0; c < AG_N; ++c) fprintf(f, " x%d\n", c);
    fprintf(f, "End\n");
    return st;
}

// ============================ SCIP run (idéntica) ============================
struct Verdict {
    enum Kind { INFEASIBLE, OPTIMAL_FEASIBLE, TIMEOUT, ERROR } kind;
    double solve_time;
    std::vector<int> solution_cols;
};
static Verdict run_scip(const std::string& lp_path, int timeout_s,
                        const std::string& sol_tmp_path)
{
    Verdict v; v.kind = Verdict::ERROR; v.solve_time = 0.0;
    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
        "scip -c 'read %s' "
             "-c 'set limits time %d' "
             "-c 'optimize' "
             "-c 'display statistics' "
             "-c 'write solution %s' "
             "-c 'quit' 2>&1",
        lp_path.c_str(), timeout_s, sol_tmp_path.c_str());

    auto t0 = std::chrono::steady_clock::now();
    FILE* pp = popen(cmd, "r");
    if (!pp) { v.kind = Verdict::ERROR; return v; }
    std::string out; char buf[4096];
    while (fgets(buf, sizeof(buf), pp)) out += buf;
    pclose(pp);
    auto t1 = std::chrono::steady_clock::now();
    v.solve_time = std::chrono::duration<double>(t1 - t0).count();

    if (out.find("[infeasible]") != std::string::npos) v.kind = Verdict::INFEASIBLE;
    else if (out.find("[optimal solution found]") != std::string::npos) v.kind = Verdict::OPTIMAL_FEASIBLE;
    else if (out.find("[time limit reached]") != std::string::npos) v.kind = Verdict::TIMEOUT;
    else v.kind = Verdict::ERROR;

    if (v.kind == Verdict::OPTIMAL_FEASIBLE) {
        FILE* fs = fopen(sol_tmp_path.c_str(), "r");
        if (fs) {
            char line[512];
            while (fgets(line, sizeof(line), fs)) {
                if (line[0] == '#' || line[0] == '\n') continue;
                if (line[0] == 'x') {
                    int c_idx = -1;
                    double val = 0.0;
                    if (sscanf(line, "x%d %lf", &c_idx, &val) >= 1) {
                        if (val > 0.5 && c_idx >= 0 && c_idx < AG_N) {
                            v.solution_cols.push_back(c_idx);
                        }
                    }
                }
            }
            fclose(fs);
        }
    }
    return v;
}

// ============================ Diamond banner ================================
static void celebrate_diamond(const SeedDef* s, int a, int b, const std::vector<int>& cols) {
    printf("\n\n");
    printf("########################################################################\n");
    printf("########################################################################\n");
    printf("##                                                                    ##\n");
    printf("##     DIAMOND [22,6,13]_4 FOUND  --  seed %s  --  pair (%d, %d)        \n",
           s->label, a, b);
    printf("##                                                                    ##\n");
    printf("########################################################################\n");
    printf("########################################################################\n\n");
    printf("Seed %s (%s), P matrix:\n", s->label, s->structure);
    for (int r = 0; r < K; ++r) {
        printf("  [%d %d %d %d]\n", s->P[r][0], s->P[r][1], s->P[r][2], s->P[r][3]);
    }
    printf("\nExtension: %d AG(5,4) columns (x_c = 1):\n", (int)cols.size());
    for (int c : cols) {
        printf("  c=%4d  [%d,%d,%d,%d,%d]\n",
               c, AG_VEC[c][0], AG_VEC[c][1], AG_VEC[c][2], AG_VEC[c][3], AG_VEC[c][4]);
    }
    printf("\nFull 22-column generator G = [I_5 | P | ext]:\n");
    for (int r = 0; r < K; ++r) {
        printf("  ");
        for (int j = 0; j < K; ++j) printf("%d ", (r==j) ? 1 : 0);
        printf("| ");
        for (int j = 0; j < 4; ++j) printf("%d ", s->P[r][j]);
        printf("| ");
        for (int c : cols) printf("%d ", AG_VEC[c][r]);
        printf("\n");
    }
    printf("\nVERIFY against ESTRELLA_DIAMOND_VERIFY_v1 IMMEDIATELY. STOP ALL ENGINES.\n\n");
    fflush(stdout);
}

// ============================ Main ===========================================
static void usage() {
    fprintf(stderr,
        "Usage: PORMISCOJONES_PROBE2 <LABEL> <scip_timeout_s> --single A,B\n"
        "       [--skip-probe] [--probe-budget S] [--probe-only] [--keep-lp]\n"
        "  --single A,B    SOLO modo single-pair (no batch).\n"
        "  --skip-probe    omite probing depth-2 (= GEOM v1).\n"
        "  --probe-budget  segundos máximos de probing (default 300).\n"
        "  --probe-only    sale tras probing, no llama SCIP.\n"
        "  --keep-lp       no borra .lp tras INFEAS.\n");
    exit(1);
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL); setbuf(stderr, NULL);
    if (argc < 4) usage();

    std::string label = argv[1];
    int scip_timeout = std::atoi(argv[2]);
    int single_a = -1, single_b = -1;
    bool skip_probe = false;
    bool probe_only = false;
    bool keep_lp = false;
    double probe_budget = 300.0;
    for (int i = 3; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "--single" && i+1 < argc) {
            const char* arg = argv[++i];
            if (sscanf(arg, "%d,%d", &single_a, &single_b) != 2) {
                fprintf(stderr, "ERROR: --single requires A,B\n"); return 1;
            }
        } else if (s == "--skip-probe") skip_probe = true;
        else if (s == "--probe-only") probe_only = true;
        else if (s == "--keep-lp") keep_lp = true;
        else if (s == "--probe-budget" && i+1 < argc) probe_budget = std::atof(argv[++i]);
        else { fprintf(stderr, "Unknown arg: %s\n", argv[i]); usage(); }
    }
    if (single_a < 0 || single_b < 0) {
        fprintf(stderr, "ERROR: this v2 PROBE2 only supports --single mode currently.\n");
        return 1;
    }

    const SeedDef* sd = nullptr;
    for (int i = 0; i < N_SEEDS; ++i)
        if (label == SEEDS[i].label) { sd = &SEEDS[i]; break; }
    if (!sd) { fprintf(stderr, "ERROR: unknown seed %s\n", label.c_str()); return 1; }

    T0 = std::chrono::steady_clock::now();
    time_t now = time(nullptr);
    char tbuf[64]; strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    logf("PROBE2 launching: seed=%s a=%d b=%d scip_timeout=%ds probe_budget=%.0fs skip_probe=%d",
         label.c_str(), single_a, single_b, scip_timeout, probe_budget, (int)skip_probe);
    logf("Wall start: %s", tbuf);

    precompute_AG();
    load_seed(sd);
    precompute_CAP_M();
    precompute_SLICE();
    precompute_SLICE_LIST();
    logf("Precompute done.");

    State S;
    S.init_blank();

    if (!apply_arc_kills(S, sd, single_a, single_b)) {
        logf("ARC kills produced contradiction. PAIR INFEAS at setup.");
        logf("VERDICT: INFEAS_AT_ARC");
        return 0;
    }
    logf("Post-ARC: forced1=%d free=%d", S.total_forced_1, S.total_free);

    long long passes = 0;
    if (!cascade_geom(S, passes)) {
        logf("CASCADE proves PAIR INFEAS in %lld passes.", passes);
        logf("VERDICT: INFEAS_AT_CASCADE");
        return 0;
    }
    int forced0_post_cas = 0;
    for (int c = 0; c < AG_N; ++c) if (S.forced[c] == -1) forced0_post_cas++;
    logf("Post-cascade (%lld passes): forced1=%d forced0=%d free=%d",
         passes, S.total_forced_1, forced0_post_cas, S.total_free);

    std::vector<std::pair<int,int>> binary_cuts;
    std::vector<int> unary_zeros, unary_ones;
    ProbeStats PS;
    if (!skip_probe) {
        run_probing_depth_2(S, binary_cuts, unary_zeros, unary_ones, probe_budget, PS);
        if (PS.base_infeas) {
            logf("PROBING derived BASE INFEAS during depth-1 phase.");
            logf("VERDICT: INFEAS_AT_PROBE_D1");
            return 0;
        }
    }
    int forced0_post_probe = 0, forced1_post_probe = 0;
    for (int c = 0; c < AG_N; ++c) {
        if (S.forced[c] == 1) forced1_post_probe++;
        else if (S.forced[c] == -1) forced0_post_probe++;
    }
    logf("Post-probe state: forced1=%d forced0=%d free=%d binary_cuts=%zu",
         forced1_post_probe, forced0_post_probe, S.total_free, binary_cuts.size());

    if (probe_only) {
        logf("--probe-only set, exiting after probing. NO SCIP CALL.");
        logf("VERDICT: PROBE_ONLY_DONE");
        return 0;
    }

    // Emit LP and run SCIP
    const char* tmpdir = "/tmp/estrella_pair";
    mkdir(tmpdir, 0755);
    char lp_path[512], sol_path[512];
    snprintf(lp_path, sizeof(lp_path), "%s/probe2_%s_a%d_b%d.lp",
             tmpdir, sd->label, single_a, single_b);
    snprintf(sol_path, sizeof(sol_path), "%s/probe2_%s_a%d_b%d.sol",
             tmpdir, sd->label, single_a, single_b);
    FILE* flp = fopen(lp_path, "w");
    if (!flp) { fprintf(stderr, "ERROR: cannot write %s\n", lp_path); return 4; }
    LPStats st = emit_lp_from_state(flp, sd, single_a, single_b, S, binary_cuts);
    fclose(flp);
    logf("LP emitted: %s aff_added=%d aff_dedup=%d aff_trivial=%d forced1=%d forced0=%d binary_cuts=%d",
         lp_path, st.n_aff_added, st.n_aff_dedup, st.n_aff_trivial,
         st.n_forced_one, st.n_forced_zero, st.n_binary_cuts);

    Verdict v = run_scip(lp_path, scip_timeout, sol_path);
    const char* vs =
        (v.kind == Verdict::INFEASIBLE)        ? "INFEAS"   :
        (v.kind == Verdict::TIMEOUT)           ? "TIMEOUT"  :
        (v.kind == Verdict::OPTIMAL_FEASIBLE)  ? "FEASIBLE" : "ERROR";
    logf("SCIP done: verdict=%s solve_t=%.2fs", vs, v.solve_time);

    fprintf(stderr, "\n");
    fprintf(stderr, "==============================================\n");
    fprintf(stderr, "[PROBE2] FINAL VERDICT  : %s\n", vs);
    fprintf(stderr, "[PROBE2] SCIP solve_t   : %.2f s\n", v.solve_time);
    fprintf(stderr, "[PROBE2] probe_elapsed  : %.2f s\n", PS.elapsed);
    fprintf(stderr, "[PROBE2] probe_pairs    : tried=%lld failed=%lld\n", PS.pairs_tried, PS.pairs_failed);
    fprintf(stderr, "[PROBE2] probe_d1_zeros : %lld\n", PS.depth1_forced_zero);
    fprintf(stderr, "[PROBE2] probe_d1_ones  : %lld\n", PS.depth1_forced_one);
    fprintf(stderr, "[PROBE2] binary_cuts    : %d\n", st.n_binary_cuts);
    fprintf(stderr, "[PROBE2] pair           : seed=%s a=%d b=%d\n", sd->label, single_a, single_b);
    fprintf(stderr, "==============================================\n");

    if (v.kind == Verdict::OPTIMAL_FEASIBLE) {
        celebrate_diamond(sd, single_a, single_b, v.solution_cols);
        return 0;
    }
    if (!keep_lp && v.kind == Verdict::INFEASIBLE) {
        unlink(lp_path); unlink(sol_path);
    }
    return (v.kind == Verdict::ERROR) ? 5 : 0;
}
