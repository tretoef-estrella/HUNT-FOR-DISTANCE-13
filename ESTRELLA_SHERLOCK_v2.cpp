// ESTRELLA_SHERLOCK_v2 — Tree size estimator for 10 remaining [9,5,4]_4 seeds
// Proyecto Estrella · April 2026
//
// WHAT THIS DOES:
//   Monte Carlo estimate of the DFS tree size with AFFINE constraints
//   (4092 per m,α) for each of the 10 remaining seeds.
//
//   Technique: Knuth tree-size estimator.  At each branching node, we
//   multiply by the degree (# children we could branch on) and then
//   pick ONE child uniformly at random to recurse into.  The expected
//   value of this product across many random walks equals the total tree
//   size (exactly, for unbiased weights).
//
// Output per seed: estimated tree size ± standard error after 200 walks,
//                  plus best depth reached in any walk.
// Runtime: ~5-10 min total (10 seeds × ~30-60s each).
//
// Compile: g++ -O3 -march=native -std=c++17 -funroll-loops -o SH2 ESTRELLA_SHERLOCK_v2.cpp
// Run:     caffeinate -dims nice -n 10 ./SH2 2>&1 | tee sh2.log
// Custom walks per seed:  ./SH2 500

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <vector>
#include <random>
#include <cmath>

static const int ADD[4][4]={{0,1,2,3},{1,0,3,2},{2,3,0,1},{3,2,1,0}};
static const int MUL[4][4]={{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};

static const int SEED_P[12][5][4] = {
    {{0,1,1,1},{0,1,2,3},{0,1,3,2},{1,0,1,1},{1,0,2,3}},
    {{0,1,1,1},{0,1,2,3},{0,1,3,2},{1,0,1,1},{1,1,0,2}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,0,2,3},{1,1,0,1}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,0,2,3},{1,1,1,2}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,0,2,3},{1,2,0,2}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,1,0,1},{1,1,1,0}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,1,0,1},{1,1,1,2}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,1,0,2},{1,1,2,1}},
    {{0,1,1,1},{0,1,2,3},{1,0,1,1},{1,2,0,2},{1,2,1,3}},
    {{0,1,1,1},{1,0,1,1},{1,1,0,2},{1,1,2,1},{1,2,1,0}},
    {{0,1,1,1},{1,0,1,1},{1,1,0,2},{1,1,3,0},{1,2,1,2}},
    {{0,1,1,1},{1,0,1,1},{1,1,2,3},{1,1,3,2},{1,2,0,3}}
};
static const int SEED_A4[12]={78,72,60,63,57,66,54,51,54,45,48,42};

#define NW 16
typedef struct { uint64_t w[NW]; } bs1024;
static inline void bs_zero(bs1024* b){ for(int i=0;i<NW;i++) b->w[i]=0; }
static inline int bs_popcount(const bs1024* b){
    int c=0; for(int i=0;i<NW;i++) c += __builtin_popcountll(b->w[i]);
    return c;
}

static unsigned char DT[1024][1024];
static int WS[1024];
static bs1024 SLICE_MASK[1024][4];

static int COUNT[1024][4];
static int BLOCKED[1024];
static bs1024 IN_PATH;
static bs1024 BLOCKED_MASK;

// Simplified: no stack, we track state via global arrays and restore via
// undo vectors.
struct Undo {
    int point;
    std::vector<int16_t> blocked_inc;
};

static void build_seed_tables(int seed_id){
    int P[5][4];
    for(int i=0;i<5;i++)for(int j=0;j<4;j++) P[i][j] = SEED_P[seed_id-1][i][j];
    int G[5][9];
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++) G[i][j] = (i==j)?1:0;
        for(int j=0;j<4;j++) G[i][5+j] = P[i][j];
    }
    for(int msg=1;msg<1024;msg++){
        int m[5]={msg%4,(msg/4)%4,(msg/16)%4,(msg/64)%4,(msg/256)%4};
        int wt=0;
        for(int j=0;j<9;j++){
            int v=0; for(int r=0;r<5;r++) v=ADD[v][MUL[m[r]][G[r][j]]];
            if(v) wt++;
        }
        WS[msg]=wt;
        for(int c=0;c<1024;c++){
            int cc[5]={c%4,(c/4)%4,(c/16)%4,(c/64)%4,(c/256)%4};
            int s=0;
            for(int i=0;i<5;i++) s=ADD[s][MUL[m[i]][cc[i]]];
            DT[msg][c]=(unsigned char)s;
        }
    }
    for(int m=1;m<1024;m++){
        for(int a=0;a<4;a++) bs_zero(&SLICE_MASK[m][a]);
        for(int c=0;c<1024;c++){
            int a = DT[m][c];
            SLICE_MASK[m][a].w[c>>6] |= (1ULL << (c&63));
        }
    }
}

static bool place(int c, Undo& u){
    u.point = c; u.blocked_inc.clear();
    for(int m=1;m<1024;m++){
        int a = DT[m][c];
        COUNT[m][a]++;
        if(COUNT[m][a] > WS[m]){
            COUNT[m][a]--;
            for(int m2=1;m2<m;m2++){
                int a2 = DT[m2][c];
                COUNT[m2][a2]--;
            }
            return false;
        }
    }
    bs1024 cascade; bs_zero(&cascade);
    for(int m=1;m<1024;m++){
        int a = DT[m][c];
        if(COUNT[m][a] == WS[m]){
            const bs1024* sm = &SLICE_MASK[m][a];
            for(int i=0;i<NW;i++) cascade.w[i] |= sm->w[i];
        }
    }
    cascade.w[c>>6] &= ~(1ULL << (c&63));
    for(int i=0;i<NW;i++) cascade.w[i] &= ~IN_PATH.w[i];
    for(int wi=0;wi<NW;wi++){
        uint64_t bits = cascade.w[wi];
        while(bits){
            int b = __builtin_ctzll(bits); bits &= bits - 1;
            int c2 = (wi<<6) + b;
            BLOCKED[c2]++;
            u.blocked_inc.push_back((int16_t)c2);
            if(BLOCKED[c2] == 1) BLOCKED_MASK.w[wi] |= (1ULL << b);
        }
    }
    IN_PATH.w[c>>6] |= (1ULL << (c&63));
    return true;
}

static void unplace(const Undo& u){
    int c = u.point;
    for(int16_t c2 : u.blocked_inc){
        BLOCKED[c2]--;
        if(BLOCKED[c2] == 0) BLOCKED_MASK.w[c2>>6] &= ~(1ULL << (c2&63));
    }
    for(int m=1;m<1024;m++){
        int a = DT[m][c];
        COUNT[m][a]--;
    }
    IN_PATH.w[c>>6] &= ~(1ULL << (c&63));
}

// Knuth tree-size estimator: random walk, multiply by branching factor.
// Starts from root.  At each node, we enumerate children (free cols with
// c > lowbound), pick one uniformly, recurse.  Product of branching
// factors at each level = estimator.
static std::mt19937 rng(42);

static double one_walk(int& deepest_depth){
    // Reset state
    memset(BLOCKED, 0, sizeof(BLOCKED));
    for(int m=1;m<1024;m++) for(int a=0;a<4;a++) COUNT[m][a] = 0;
    bs_zero(&IN_PATH); bs_zero(&BLOCKED_MASK);

    double product = 1.0;
    int lowbound = 0;
    int depth = 0;
    std::vector<Undo> undo_stack;
    undo_stack.reserve(16);

    // Depth 0: translation symmetry forces c=0
    // (We don't count this in product since it's deterministic)
    {
        Undo u;
        if(!place(0, u)) return 0.0;
        undo_stack.push_back(std::move(u));
        depth = 1;
        lowbound = 1;
    }

    while(depth < 13){
        // Collect viable children
        std::vector<int> kids;
        kids.reserve(256);
        for(int wi=0;wi<NW;wi++){
            uint64_t bits = ~IN_PATH.w[wi] & ~BLOCKED_MASK.w[wi];
            int base = wi<<6;
            while(bits){
                int b = __builtin_ctzll(bits); bits &= bits - 1;
                int c = base + b;
                if(c < lowbound) continue;
                kids.push_back(c);
            }
        }

        if(kids.empty()){
            // leaf (can't extend further)
            if(depth > deepest_depth) deepest_depth = depth;
            // unwind
            while(!undo_stack.empty()){ unplace(undo_stack.back()); undo_stack.pop_back(); }
            return product;
        }

        // Randomly pick one kid to descend; multiply by branch factor
        std::uniform_int_distribution<int> dist(0, (int)kids.size()-1);
        int pick_idx = dist(rng);
        int c = kids[pick_idx];
        // Try place. If it fails (shouldn't — free means no constraint
        // overflow — but BLOCKED is necessary-but-not-sufficient: a point
        // being "free" by BLOCKED might still overflow a constraint because
        // BLOCKED only triggers at COUNT==WS, not pre-overflow.
        // Actually wait: if COUNT[m][a] < WS[m] and we increment, the new
        // value is ≤ WS[m], so no overflow. BLOCKED triggers when ==WS,
        // which means NEXT placement on that slice would overflow. Points
        // in that slice are blocked, so place(c) only succeeds if no m has
        // COUNT[m][DT[m,c]] == WS[m] — which is exactly the BLOCKED
        // criterion. So free => place succeeds.
        Undo u;
        if(!place(c, u)){
            // Shouldn't happen. Treat as leaf.
            if(depth > deepest_depth) deepest_depth = depth;
            while(!undo_stack.empty()){ unplace(undo_stack.back()); undo_stack.pop_back(); }
            return product;
        }
        undo_stack.push_back(std::move(u));
        product *= (double)kids.size();
        depth++;
        lowbound = c + 1;
    }

    // Reached depth 13 — diamond! (very unlikely in random walk, but possible)
    if(depth > deepest_depth) deepest_depth = depth;
    while(!undo_stack.empty()){ unplace(undo_stack.back()); undo_stack.pop_back(); }
    return product;
}

int main(int argc, char** argv){
    setbuf(stdout,NULL); setbuf(stderr,NULL);
    int N_WALKS = (argc > 1) ? atoi(argv[1]) : 200;
    if(N_WALKS < 10) N_WALKS = 10;

    fprintf(stderr,"===== ESTRELLA_SHERLOCK_v2 — Tree-Size Estimator =====\n");
    fprintf(stderr,"Walks per seed: %d  (AFFINE constraints)\n", N_WALKS);

    // Table summary
    struct SeedResult {
        int seed;
        double mean_tree_size;
        double stderr_tree_size;
        double log10_mean;
        int max_deepest;
        double elapsed;
    };
    std::vector<SeedResult> results;

    for(int sid = 3; sid <= 12; sid++){
        fprintf(stderr,"\n[SEED #%d] A4=%d  walking %d trails...\n", sid, SEED_A4[sid-1], N_WALKS);
        time_t t0 = time(NULL);
        build_seed_tables(sid);
        double sum = 0, sumsq = 0;
        int max_deep = 0;
        long long max_val_reported = 0;
        (void)max_val_reported;
        for(int w=0; w<N_WALKS; w++){
            int depth = 0;
            double v = one_walk(depth);
            sum += v;
            sumsq += v*v;
            if(depth > max_deep) max_deep = depth;
            if(w % 50 == 49){
                double el = difftime(time(NULL), t0);
                fprintf(stderr,"  walk %4d/%d  running_mean=%.2e  max_depth=%d  t=%.0fs\n",
                        w+1, N_WALKS, sum/(w+1), max_deep, el);
            }
        }
        double mean = sum / N_WALKS;
        double var  = (sumsq/N_WALKS) - mean*mean;
        if(var < 0) var = 0;
        double se   = std::sqrt(var / N_WALKS);
        double el = difftime(time(NULL), t0);

        SeedResult r;
        r.seed = sid;
        r.mean_tree_size = mean;
        r.stderr_tree_size = se;
        r.log10_mean = (mean > 0) ? std::log10(mean) : -1;
        r.max_deepest = max_deep;
        r.elapsed = el;
        results.push_back(r);

        fprintf(stderr,"[SEED #%d] estimated tree size = %.2e ± %.2e   max_depth=%d   time=%.0fs\n",
                sid, mean, se, max_deep, el);
    }

    // Summary table
    fprintf(stderr,"\n========== SHERLOCK-v2 REPORT ==========\n");
    fprintf(stderr,"Seed  A4  EstTreeSize    log10    MaxDepth  EstHours@250k/s\n");
    fprintf(stderr,"----  --  -------------  -------  --------  ---------------\n");
    for(const auto& r : results){
        double hours = r.mean_tree_size / 250000.0 / 3600.0;
        fprintf(stderr,"#%-3d  %2d  %.3e      %6.2f   %8d  %.2f\n",
                r.seed, SEED_A4[r.seed-1], r.mean_tree_size, r.log10_mean,
                r.max_deepest, hours);
    }

    fprintf(stderr,"\n=== INTERPRETATION ===\n");
    fprintf(stderr,"EstHours@250k/s = time for v5 DFS to close UNSAT on M2.\n");
    fprintf(stderr,"  < 1h:    easily closeable, just run v5\n");
    fprintf(stderr,"  1-24h:   feasible overnight\n");
    fprintf(stderr,"  > 24h:   need smarter engine (SAT, structural)\n");
    fprintf(stderr,"If max_depth stays at 8 across ALL walks, the geometric\n");
    fprintf(stderr,"ceiling hypothesis is very strongly supported.\n");
    return 0;
}
