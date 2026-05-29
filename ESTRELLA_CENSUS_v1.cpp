/*
 * ESTRELLA_CENSUS_v1 — Complete enumeration of ALL valid [22,6,13]_4
 * weight distributions satisfying MacWilliams identities + dual non-negativity.
 *
 * What it does:
 *   Iterates over 6 free variables (A13,A14,A15,A20,A21,A22) in steps of 3.
 *   Computes 4 dependent variables (A16..A19) from Section 54.8 formulas.
 *   Checks non-negativity of all A_i AND all dual weights B_j (j=4..22).
 *   Separates results into d'=4 (B4>0) and d'>=5 (B4=0).
 *
 * Output: every valid distribution + full statistical analysis.
 * Log:    ~/Downloads/ESTRELLA_CENSUS_v1_log.txt
 *
 * Compile: g++ -O3 -march=native -std=c++17 -funroll-loops -o ESTRELLA_CENSUS_v1 ESTRELLA_CENSUS_v1.cpp
 * Run:     cd ~/Downloads && ./ESTRELLA_CENSUS_v1
 *
 * Author: Claude (Anthropic) + R. Amichis (The Architect)
 * Date:   10 April 2026 — Proyecto Estrella
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <ctime>
#include <climits>

// === Binomial coefficient (exact integer) ===
static long long binom(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k > n - k) k = n - k;
    long long r = 1;
    for (int i = 0; i < k; i++) r = r * (n - i) / (i + 1);
    return r;
}

// === Integer power ===
static long long ipow(long long base, int exp) {
    long long r = 1;
    for (int i = 0; i < exp; i++) r *= base;
    return r;
}

// === Krawtchouk polynomial K_j(i; n=22, q=4) ===
static long long kraw(int j, int i) {
    long long val = 0;
    for (int s = 0; s <= std::min(i, j); s++) {
        long long term = ipow(-1, s) * ipow(3, j - s) * binom(i, s) * binom(22 - i, j - s);
        val += term;
    }
    return val;
}

// Precomputed K[j][i]
static long long K[23][23];

// Dual constraints: 4096*B_j = C[idx] + v[idx][0..5] . (A13,A14,A15,A20,A21,A22)
static long long DC_C[19];      // constant term, idx 0..18 = j 4..22
static long long DC_v[19][6];   // coefficients: [0]=c13 [1]=c14 [2]=c15 [3]=c20 [4]=c21 [5]=c22

struct Dist {
    int A[10];  // A13..A22 (indices 0..9)
    int B4;     // dual weight B_4
};

static inline long long ceildiv(long long a, long long b) {
    // ceiling(a/b) for b > 0
    if (a >= 0) return (a + b - 1) / b;
    else return -((-a) / b);
}

int main() {
    clock_t t0 = clock();

    // Precompute Krawtchouk
    for (int j = 0; j <= 22; j++)
        for (int i = 0; i <= 22; i++)
            K[j][i] = kraw(j, i);

    // Compute dual constraint linear coefficients
    // After substituting A16..A19 from Section 54.8 formulas:
    for (int idx = 0; idx < 19; idx++) {
        int j = idx + 4;
        DC_v[idx][0] = K[j][13] - 20*K[j][16] + 45*K[j][17] - 36*K[j][18] + 10*K[j][19];
        DC_v[idx][1] = K[j][14] - 10*K[j][16] + 20*K[j][17] - 15*K[j][18] +  4*K[j][19];
        DC_v[idx][2] = K[j][15] -  4*K[j][16] +  6*K[j][17] -  4*K[j][18] +  1*K[j][19];
        DC_v[idx][3] = K[j][20] +  1*K[j][16] -  4*K[j][17] +  6*K[j][18] -  4*K[j][19];
        DC_v[idx][4] = K[j][21] +  4*K[j][16] - 15*K[j][17] + 20*K[j][18] - 10*K[j][19];
        DC_v[idx][5] = K[j][22] + 10*K[j][16] - 36*K[j][17] + 45*K[j][18] - 20*K[j][19];
        DC_C[idx]    = K[j][0]  + 14391LL*K[j][16] - 27216LL*K[j][17]
                                 + 21480LL*K[j][18] -  4560LL*K[j][19];
    }

    // === VERIFY with distribution #1 from Section 54.5 ===
    {
        int tA[] = {411,483,420,312,1020,1041,144,207,21,36}; // A13..A22
        int A16c = 14391 - 20*411 - 10*483 - 4*420 + 207 + 4*21 + 10*36;
        int A17c = -27216 + 45*411 + 20*483 + 6*420 - 4*207 - 15*21 - 36*36;
        int A18c = 21480 - 36*411 - 15*483 - 4*420 + 6*207 + 20*21 + 45*36;
        int A19c = -4560 + 10*411 + 4*483 + 420 - 4*207 - 10*21 - 20*36;
        printf("VERIFY formulas: A16=%d(312) A17=%d(1020) A18=%d(1041) A19=%d(144)\n",
               A16c, A17c, A18c, A19c);
        bool ok = true;
        for (int idx = 0; idx < 19; idx++) {
            long long val = DC_C[idx];
            long long fv[] = {411,483,420,207,21,36};
            for (int k = 0; k < 6; k++) val += DC_v[idx][k] * fv[k];
            if (val < 0 || val % 4096 != 0) {
                printf("  B_%d FAIL: val=%lld mod=%lld\n", idx+4, val, val%4096);
                ok = false;
            }
        }
        printf("VERIFY dual: %s\n\n", ok ? "ALL OK" : "FAILED");
        if (!ok) { printf("BUG IN FORMULAS. ABORT.\n"); return 1; }
    }

    // === Open log ===
    FILE *logf = fopen("ESTRELLA_CENSUS_v1_log.txt", "w");
    if (!logf) { printf("Cannot open log file!\n"); return 1; }
    fprintf(logf, "ESTRELLA_CENSUS_v1 — Diamond [22,6,13]_4 Weight Distribution Census\n");
    fprintf(logf, "Date: 10 April 2026\n\n");

    // === Precompute c15 sign categories for fast A15 bounding ===
    std::vector<int> c15_pos_idx, c15_neg_idx, c15_zero_idx;
    for (int idx = 0; idx < 19; idx++) {
        long long c15 = DC_v[idx][2];
        if (c15 > 0) c15_pos_idx.push_back(idx);
        else if (c15 < 0) c15_neg_idx.push_back(idx);
        else c15_zero_idx.push_back(idx);
    }
    printf("c15 categories: pos=%zu neg=%zu zero=%zu\n",
           c15_pos_idx.size(), c15_neg_idx.size(), c15_zero_idx.size());

    // === MAIN ENUMERATION ===
    printf("Starting enumeration...\n");
    fflush(stdout);

    std::vector<Dist> results;
    long long checked = 0, outer = 0;

    for (int A22 = 0; A22 <= 138; A22 += 3) {
      for (int A21 = 0; A21 <= 315; A21 += 3) {
        for (int A20 = 0; A20 <= 600; A20 += 3) {
            outer++;

            // Partial sums for dual constraints from (A20,A21,A22)
            long long dc_p0[19];
            for (int idx = 0; idx < 19; idx++)
                dc_p0[idx] = DC_C[idx] + DC_v[idx][3]*(long long)A20
                                       + DC_v[idx][4]*(long long)A21
                                       + DC_v[idx][5]*(long long)A22;

            // Non-neg constraint constants
            long long U1 = 14391LL + A20 + 4*A21 + 10*A22;    // A16>=0: 20*A13+10*A14+4*A15 <= U1
            long long L2 = 27216LL + 4*A20 + 15*A21 + 36*A22; // A17>=0: 45*A13+20*A14+6*A15 >= L2
            long long U3 = 21480LL + 6*A20 + 20*A21 + 45*A22; // A18>=0: 36*A13+15*A14+4*A15 <= U3
            long long L4 = 4560LL  + 4*A20 + 10*A21 + 20*A22; // A19>=0: 10*A13+4*A14+A15 >= L4

            // Quick skip
            if (L4 > 10*786 + 4*1512 + 1560) continue;
            if (L2 > 45*786 + 20*1512 + 6*1560) continue;

            int a13_hi = (int)std::min({786LL, U1/20, U3/36});
            if (a13_hi < 0) continue;
            a13_hi -= a13_hi % 3;

            for (int A13 = 0; A13 <= a13_hi; A13 += 3) {
                long long rI = U1 - 20*A13;
                long long rIII = U3 - 36*A13;
                if (rI < 0 || rIII < 0) break;
                long long rII = L2 - 45*A13;
                long long rIV = L4 - 10*A13;

                // Dual partials with A13
                long long dc_p1[19];
                for (int idx = 0; idx < 19; idx++)
                    dc_p1[idx] = dc_p0[idx] + DC_v[idx][0]*(long long)A13;

                // A14 bounds
                int a14_hi = (int)std::min({1512LL, rI/10, rIII/15});
                int a14_lo = 0;
                if (rII - 6*1560 > 0) a14_lo = std::max(a14_lo, (int)ceildiv(rII - 6*1560, 20));
                if (rIV - 1560 > 0)   a14_lo = std::max(a14_lo, (int)ceildiv(rIV - 1560, 4));
                // Round up to mult of 3
                if (a14_lo % 3 != 0) a14_lo += 3 - (a14_lo % 3);
                if (a14_lo > a14_hi) continue;

                for (int A14 = a14_lo; A14 <= a14_hi; A14 += 3) {
                    long long rI2 = rI - 10*A14;
                    long long rIII2 = rIII - 15*A14;
                    if (rI2 < 0 || rIII2 < 0) break;
                    long long rII2 = rII - 20*A14;
                    long long rIV2 = rIV - 4*A14;

                    // Dual partials with A14
                    long long dc_p2[19];
                    for (int idx = 0; idx < 19; idx++)
                        dc_p2[idx] = dc_p1[idx] + DC_v[idx][1]*(long long)A14;

                    // A15 bounds from non-neg
                    long long a15_hi_l = std::min({1560LL, rI2/4, rIII2/4});
                    long long a15_lo_l = 0;
                    if (rII2 > 0) a15_lo_l = std::max(a15_lo_l, ceildiv(rII2, 6));
                    if (rIV2 > 0) a15_lo_l = std::max(a15_lo_l, rIV2);

                    // Dual constraints on A15: c15_zero feasibility
                    bool skip = false;
                    for (int idx : c15_zero_idx) {
                        if (dc_p2[idx] < 0) { skip = true; break; }
                    }
                    if (skip) continue;

                    // c15_pos: lower bounds
                    for (int idx : c15_pos_idx) {
                        long long p = dc_p2[idx];
                        if (p < 0) {
                            long long c15 = DC_v[idx][2];
                            long long lb = ceildiv(-p, c15);
                            if (lb > a15_lo_l) a15_lo_l = lb;
                        }
                    }

                    // c15_neg: upper bounds
                    for (int idx : c15_neg_idx) {
                        long long p = dc_p2[idx];
                        long long neg_c15 = -DC_v[idx][2];
                        if (p < 0) { skip = true; break; }
                        long long ub = p / neg_c15;
                        if (ub < a15_hi_l) a15_hi_l = ub;
                    }
                    if (skip) continue;

                    // Round to mult of 3
                    int a15_lo = (int)a15_lo_l;
                    if (a15_lo % 3 != 0) a15_lo += 3 - (a15_lo % 3);
                    int a15_hi = (int)a15_hi_l;
                    a15_hi -= a15_hi % 3;
                    if (a15_lo < 0) a15_lo = 0;

                    if (a15_lo > a15_hi) continue;

                    for (int A15 = a15_lo; A15 <= a15_hi; A15 += 3) {
                        checked++;

                        // Compute dependent vars
                        int A16 = (int)(14391 - 20*A13 - 10*A14 - 4*A15 + A20 + 4*A21 + 10*A22);
                        int A17 = (int)(-27216 + 45*A13 + 20*A14 + 6*A15 - 4*A20 - 15*A21 - 36*A22);
                        int A18 = (int)(21480 - 36*A13 - 15*A14 - 4*A15 + 6*A20 + 20*A21 + 45*A22);
                        int A19 = (int)(-4560 + 10*A13 + 4*A14 + A15 - 4*A20 - 10*A21 - 20*A22);

                        if (A16 < 0 || A17 < 0 || A18 < 0 || A19 < 0) continue;

                        // Check ALL dual weights
                        bool valid = true;
                        int B4_val = 0;
                        for (int idx = 0; idx < 19; idx++) {
                            long long val = dc_p2[idx] + DC_v[idx][2]*(long long)A15;
                            if (val < 0 || val % 4096 != 0) { valid = false; break; }
                            if (idx == 0) B4_val = (int)(val / 4096);
                        }
                        if (!valid) continue;

                        // VALID DISTRIBUTION FOUND
                        Dist d;
                        d.A[0]=A13; d.A[1]=A14; d.A[2]=A15; d.A[3]=A16;
                        d.A[4]=A17; d.A[5]=A18; d.A[6]=A19; d.A[7]=A20;
                        d.A[8]=A21; d.A[9]=A22;
                        d.B4 = B4_val;
                        results.push_back(d);

                        int n9=A13/3, n8=A14/3, n7=A15/3, n6=A16/3,
                            n5=A17/3, n4=A18/3, n3=A19/3, n2=A20/3,
                            n1=A21/3, n0=A22/3;
                        int cnt = (int)results.size();
                        if (cnt <= 30 || cnt % 500 == 0) {
                            printf("#%d: n9=%d n8=%d n7=%d n6=%d n5=%d n4=%d n3=%d n2=%d n1=%d n0=%d | B4=%d\n",
                                   cnt, n9,n8,n7,n6,n5,n4,n3,n2,n1,n0, B4_val);
                        }
                        fprintf(logf, "%d %d %d %d %d %d %d %d %d %d | B4=%d | n: %d %d %d %d %d %d %d %d %d %d\n",
                                A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,
                                B4_val, n9,n8,n7,n6,n5,n4,n3,n2,n1,n0);
                    }
                }
            }

            if (outer % 200000 == 0) {
                double elapsed = (double)(clock()-t0)/CLOCKS_PER_SEC;
                printf("  ... outer=%lld/~1M found=%zu checked=%lld time=%.1fs\n",
                       outer, results.size(), checked, elapsed);
                fflush(stdout);
            }
        }
      }
    }

    double elapsed = (double)(clock()-t0)/CLOCKS_PER_SEC;

    // === RESULTS ===
    int total = (int)results.size();
    printf("\n========================================\n");
    printf("ENUMERATION COMPLETE\n");
    printf("========================================\n");
    printf("Outer iterations: %lld\n", outer);
    printf("Inner candidates checked: %lld\n", checked);
    printf("Valid distributions (d'>=4): %d\n", total);
    printf("Time: %.2f seconds\n", elapsed);

    if (total == 0) {
        printf("\n*** NO VALID DISTRIBUTIONS FOUND ***\n");
        printf("*** THIS WOULD PROVE [22,6,13]_4 DOES NOT EXIST ***\n");
        fprintf(logf, "\n*** NO VALID DISTRIBUTIONS ***\n");
        fclose(logf);
        return 0;
    }

    // Separate d'=4 and d'>=5
    std::vector<Dist> d4_only, d5_plus;
    for (auto &d : results) {
        if (d.B4 == 0) d5_plus.push_back(d);
        else d4_only.push_back(d);
    }
    printf("\nWith d'=4 exactly (B4>0): %d\n", (int)d4_only.size());
    printf("With d'>=5 (B4=0):        %d\n", (int)d5_plus.size());

    // === ANALYSIS ===
    fprintf(logf, "\n=== ANALYSIS ===\n");
    printf("\n=== WEIGHT DISTRIBUTION RANGES ===\n");
    const char *names[] = {"A13","A14","A15","A16","A17","A18","A19","A20","A21","A22"};
    const char *hpnames[] = {"n9","n8","n7","n6","n5","n4","n3","n2","n1","n0"};
    int e1_A[] = {390,390,540,675,636,690,540,225,6,0};
    int e1_n[] = {130,130,180,225,212,230,180,75,2,0};

    for (int i = 0; i < 10; i++) {
        int mn = INT_MAX, mx = INT_MIN;
        for (auto &d : results) { mn = std::min(mn, d.A[i]); mx = std::max(mx, d.A[i]); }
        printf("  %s: [%d, %d]  (E1=%d)  |  %s: [%d, %d]  (E1=%d)\n",
               names[i], mn, mx, e1_A[i], hpnames[i], mn/3, mx/3, e1_n[i]);
        fprintf(logf, "  %s: [%d, %d]  %s: [%d, %d]\n", names[i], mn, mx, hpnames[i], mn/3, mx/3);
    }

    // Universal properties
    printf("\n=== UNIVERSAL PROPERTIES ===\n");
    int min_n0=INT_MAX, max_n0=0, min_n9=INT_MAX, max_n9=0;
    int min_n5=INT_MAX, max_n5=0;
    int always_n0_gt0 = 1;
    for (auto &d : results) {
        int n0 = d.A[9]/3, n9 = d.A[0]/3, n5 = d.A[4]/3;
        min_n0 = std::min(min_n0, n0); max_n0 = std::max(max_n0, n0);
        min_n9 = std::min(min_n9, n9); max_n9 = std::max(max_n9, n9);
        min_n5 = std::min(min_n5, n5); max_n5 = std::max(max_n5, n5);
        if (n0 == 0) always_n0_gt0 = 0;
    }
    printf("  Empty HPs (n0): [%d, %d]  always>0? %s\n", min_n0, max_n0, always_n0_gt0?"YES":"NO");
    printf("  Sat9 HPs  (n9): [%d, %d]  (E1=130)\n", min_n9, max_n9);
    printf("  Load5 HPs (n5): [%d, %d]  (E1=212)\n", min_n5, max_n5);

    // Which load is the mode most often?
    int mode_count[10] = {};
    for (auto &d : results) {
        int mx = 0;
        for (int i = 0; i < 10; i++) mx = std::max(mx, d.A[i]);
        for (int i = 0; i < 10; i++) if (d.A[i] == mx) mode_count[i]++;
    }
    printf("  Mode (most populated load):\n");
    for (int i = 0; i < 10; i++)
        if (mode_count[i] > 0)
            printf("    %s is mode in %d/%d distributions (%.1f%%)\n",
                   hpnames[i], mode_count[i], total, 100.0*mode_count[i]/total);

    // B4 statistics
    if (!d4_only.empty()) {
        int b4min=INT_MAX, b4max=0;
        for (auto &d : d4_only) { b4min=std::min(b4min,d.B4); b4max=std::max(b4max,d.B4); }
        printf("\n  B4 range (d'=4 only): [%d, %d]\n", b4min, b4max);
    }

    // Check sum = 4095 for all
    printf("\n=== SANITY CHECKS ===\n");
    int sum_ok = 0, sum_fail = 0;
    for (auto &d : results) {
        int s = 0;
        for (int i = 0; i < 10; i++) s += d.A[i];
        if (s == 4095) sum_ok++; else sum_fail++;
    }
    printf("  Sum A13..A22 = 4095: %d OK, %d FAIL\n", sum_ok, sum_fail);

    // Check all divisible by 3
    int div3_ok = 0, div3_fail = 0;
    for (auto &d : results) {
        bool ok = true;
        for (int i = 0; i < 10; i++) if (d.A[i] % 3 != 0) ok = false;
        if (ok) div3_ok++; else div3_fail++;
    }
    printf("  All A_i divisible by 3: %d OK, %d FAIL\n", div3_ok, div3_fail);

    // === d'=5 DETAILED ===
    if (!d5_plus.empty()) {
        printf("\n=== d'>=5 DISTRIBUTIONS (all %d) ===\n", (int)d5_plus.size());
        fprintf(logf, "\n=== d'>=5 DISTRIBUTIONS ===\n");
        for (int i = 0; i < (int)d5_plus.size() && i < 50; i++) {
            auto &d = d5_plus[i];
            printf("  #%d: n9=%d n8=%d n7=%d n6=%d n5=%d n4=%d n3=%d n2=%d n1=%d n0=%d\n",
                   i+1, d.A[0]/3,d.A[1]/3,d.A[2]/3,d.A[3]/3,d.A[4]/3,
                   d.A[5]/3,d.A[6]/3,d.A[7]/3,d.A[8]/3,d.A[9]/3);
            fprintf(logf, "  #%d: %d %d %d %d %d %d %d %d %d %d\n",
                    i+1, d.A[0],d.A[1],d.A[2],d.A[3],d.A[4],
                    d.A[5],d.A[6],d.A[7],d.A[8],d.A[9]);
        }

        // d'=5 specific ranges
        printf("\n  d'>=5 ranges:\n");
        for (int i = 0; i < 10; i++) {
            int mn = INT_MAX, mx = INT_MIN;
            for (auto &d : d5_plus) { mn = std::min(mn, d.A[i]); mx = std::max(mx, d.A[i]); }
            printf("    %s: [%d, %d]  %s: [%d, %d]\n", names[i], mn, mx, hpnames[i], mn/3, mx/3);
        }
    }

    // === CORRELATION ANALYSIS ===
    printf("\n=== KEY CORRELATIONS ===\n");
    // Does n0>0 correlate with higher/lower n9?
    if (min_n0 > 0 || max_n0 > 0) {
        long long sum_n9_when_n0eq0 = 0, cnt0 = 0;
        long long sum_n9_when_n0gt0 = 0, cntg = 0;
        for (auto &d : results) {
            int n0 = d.A[9]/3, n9 = d.A[0]/3;
            if (n0 == 0) { sum_n9_when_n0eq0 += n9; cnt0++; }
            else { sum_n9_when_n0gt0 += n9; cntg++; }
        }
        if (cnt0 > 0) printf("  n0=0: avg_n9=%.1f (%lld distributions)\n", (double)sum_n9_when_n0eq0/cnt0, cnt0);
        if (cntg > 0) printf("  n0>0: avg_n9=%.1f (%lld distributions)\n", (double)sum_n9_when_n0gt0/cntg, cntg);
    }

    // Sum of n9+n8 (loads 8-9) vs n0+n1 (loads 0-1)
    {
        int min_high = INT_MAX, max_high = 0;
        int min_low = INT_MAX, max_low = 0;
        for (auto &d : results) {
            int high = d.A[0]/3 + d.A[1]/3; // n9+n8
            int low = d.A[9]/3 + d.A[8]/3;  // n0+n1
            min_high = std::min(min_high, high);
            max_high = std::max(max_high, high);
            min_low = std::min(min_low, low);
            max_low = std::max(max_low, low);
        }
        printf("  n9+n8 (high loads): [%d, %d]\n", min_high, max_high);
        printf("  n0+n1 (empty+starving): [%d, %d]\n", min_low, max_low);
    }

    // === WRITE ALL DISTRIBUTIONS TO LOG ===
    fprintf(logf, "\n=== ALL %d VALID DISTRIBUTIONS ===\n", total);
    fprintf(logf, "# A13 A14 A15 A16 A17 A18 A19 A20 A21 A22 B4 n9 n8 n7 n6 n5 n4 n3 n2 n1 n0\n");
    for (int i = 0; i < total; i++) {
        auto &d = results[i];
        fprintf(logf, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                d.A[0],d.A[1],d.A[2],d.A[3],d.A[4],d.A[5],d.A[6],
                d.A[7],d.A[8],d.A[9],d.B4,
                d.A[0]/3,d.A[1]/3,d.A[2]/3,d.A[3]/3,d.A[4]/3,
                d.A[5]/3,d.A[6]/3,d.A[7]/3,d.A[8]/3,d.A[9]/3);
    }

    fprintf(logf, "\nTotal: %d (d4=%d, d5+=%d)\n", total, (int)d4_only.size(), (int)d5_plus.size());
    fclose(logf);

    printf("\n========================================\n");
    printf("Log saved: ESTRELLA_CENSUS_v1_log.txt\n");
    printf("========================================\n");

    return 0;
}
