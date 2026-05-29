// ESTRELLA_FLAUTISTA_v1.cpp
// =============================================================================
// PROYECTO ESTRELLA — Diamante [22,6,13]_4 — THE LAST SHOT (honest billet)
//
// WHAT THIS IS
//   A STOCHASTIC random-restart depth-13 extension search on the residual
//   class C02 (the dominant near-record residual: 53.8% in M1, 30.4% in the
//   second record), biased toward the NON-SOFT region — the only region the
//   campaign never investigated (mystery γ: hard pairs live outside min-size=4).
//
// WHY STOCHASTIC, NOT EXHAUSTIVE
//   The depth-13 DFS tree for any clean class is ~10^16–10^17 nodes (measured,
//   Knuth estimator). Exhaustive closure = centuries per class. Brute force is
//   DEAD. But a [22,6,13] code, IF it exists in C02's non-soft region, is a
//   single needle. Massive random sampling can hit a needle that exhaustive
//   search cannot reach in human time — exactly how Phase 1 dropped excess
//   42→1 via random restarts. This is a lottery ticket, ~3–5%, aimed where the
//   evidence points. Not exhaustive proof. A shot.
//
// THE BIAS (where it looks)
//   Each restart fixes the origin c=0 and then a random "anchor" column drawn
//   preferentially from the NON-SOFT region: AG points whose min-expression
//   size in the seed basis is 2 or 3 (NOT 4 — size-4 is the soft region already
//   swept 64/64). From there it greedily descends to depth 13 picking among
//   non-blocked points with randomized tie-breaking. A restart that reaches
//   depth 13 is a CANDIDATE; it is then verified.
//
// VERIFICATION (no false positives)
//   Any depth-13 hit is checked three ways before any celebration:
//     (1) the affine load constraint already guarantees max hyperplane load <=9
//         by construction (place() rejects overflow), so excess=0 is structural;
//     (2) the subspace-load bound (Pair Theorem k<=5) is checked on the final
//         configuration: every PG(3,4) inside a load-9 hyperplane must have
//         load <=5. A genuine Diamond satisfies it; a bug would likely violate.
//     (3) the full 6x22 generator is printed for INDEPENDENT verification by
//         verify_gf4.cpp / ESTRELLA_DIAMOND_VERIFY_v1. NOTHING is called a
//         Diamond until that external check confirms d_min=13.
//
// HONESTY
//   This does not promise a Diamond. If it finds one, verify ruthlessly — it
//   could be a bug, not a code. If it finds nothing in the time budget, that is
//   the expected outcome and closes the last unlooked corner.
//
// BUILD
//   g++ -O3 -march=native -std=c++17 -funroll-loops \
//       ESTRELLA_FLAUTISTA_v1.cpp -o FLAUTISTA
//
// RUN (Mac M2, <=25% CPU, stay awake, ~1 hour)
//   caffeinate -dims nice -n 10 ./FLAUTISTA C02 3600 2>&1 | tee flautista_C02.log
//
//   Args:
//     <LABEL>       residual class (default C02)
//     <seconds>     wall-time budget in seconds (default 3600 = 1 h)
//     [--seed N]    RNG seed (default: time-based; set for reproducibility)
//     [--minsize M] anchor min-expression-size bias: 2,3 (default both 2&3)
// =============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <vector>
#include <random>
#include <algorithm>

static const int ADD[4][4]={{0,1,2,3},{1,0,3,2},{2,3,0,1},{3,2,1,0}};
static const int MUL[4][4]={{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};

// Residual classes (P-matrices, monomial-index = PORMISCOJONES geometry).
struct SeedDef { const char* label; int P[5][4]; };
static const SeedDef SEEDS[] = {
    {"C01", {{1,2,0,3},{1,1,3,2},{1,1,2,3},{1,0,1,1},{0,1,1,1}}},
    {"C02", {{1,2,1,0},{1,1,2,1},{1,1,0,2},{1,0,1,1},{0,1,1,1}}},
    {"C03", {{1,2,1,2},{1,1,2,0},{1,1,0,3},{1,0,1,1},{0,1,1,1}}},
    {"C06", {{1,1,3,2},{1,1,2,0},{1,0,1,2},{0,1,2,3},{0,1,1,1}}},
    {"C10", {{1,1,1,0},{1,1,0,1},{1,0,1,1},{0,1,2,3},{0,1,1,1}}},
    {"C12", {{1,0,2,3},{1,0,1,1},{0,1,3,2},{0,1,2,3},{0,1,1,1}}}
};
static const int N_SEEDS = sizeof(SEEDS)/sizeof(SEEDS[0]);
static const int K=5, NS=9, AGN=1024;
#define NW 16
struct bs1024 { uint64_t w[NW]; };
static inline void bs_zero(bs1024&b){ for(int i=0;i<NW;i++) b.w[i]=0; }
static inline int bs_pop(const bs1024&b){ int c=0; for(int i=0;i<NW;i++) c+=__builtin_popcountll(b.w[i]); return c; }

static unsigned char DT[AGN][AGN];
static int  SEEDCNT[AGN][4];
static bs1024 SLICE_MASK[AGN][4];
static int  AG_V[AGN][K];
static int  SEED_COL[NS][K];
static int  MINSIZE[AGN];        // min-expression size of each AG point in seed basis

static int COUNT[AGN][4];
static int BLOCKED[AGN];
static bs1024 IN_PATH, BLOCKED_MASK;

static void dec(int idx,int v[K]){ for(int d=0;d<K;d++){ v[d]=idx&3; idx>>=2; } }
static int enc(const int v[K]){ int idx=0; for(int d=K-1;d>=0;d--) idx=idx*4+v[d]; return idx; }

static const SeedDef* g_seed=nullptr;

static void build_tables(const SeedDef*s){
    for(int i=0;i<AGN;i++) dec(i,AG_V[i]);
    int G[K][NS];
    for(int i=0;i<K;i++){ for(int j=0;j<K;j++) G[i][j]=(i==j); for(int j=0;j<4;j++) G[i][5+j]=s->P[i][j]; }
    for(int j=0;j<NS;j++) for(int i=0;i<K;i++) SEED_COL[j][i]=G[i][j];
    for(int m=1;m<AGN;m++){
        for(int c=0;c<AGN;c++){
            int sdot=0; for(int d=0;d<K;d++) sdot=ADD[sdot][MUL[AG_V[m][d]][AG_V[c][d]]];
            DT[m][c]=(unsigned char)sdot;
        }
        int sc[4]={0,0,0,0};
        for(int j=0;j<NS;j++){ int sd=0; for(int d=0;d<K;d++) sd=ADD[sd][MUL[AG_V[m][d]][SEED_COL[j][d]]]; sc[sd]++; }
        for(int a=0;a<4;a++) SEEDCNT[m][a]=sc[a];
        for(int a=0;a<4;a++) bs_zero(SLICE_MASK[m][a]);
        for(int c=0;c<AGN;c++){ int a=DT[m][c]; SLICE_MASK[m][a].w[c>>6]|=(1ULL<<(c&63)); }
    }
}

// min-expression size: smallest k s.t. vec = sum of k nonzero multiples of seed cols.
// Computed by BFS over reachable combinations up to size 4 (enough to label 2/3/4).
static void build_minsize(){
    for(int i=0;i<AGN;i++) MINSIZE[i]=99;
    MINSIZE[0]=0;
    // size 1
    std::vector<int> cur;
    for(int j=0;j<NS;j++) for(int lam=1;lam<4;lam++){
        int v[K]; for(int d=0;d<K;d++) v[d]=MUL[lam][SEED_COL[j][d]];
        int idx=enc(v); if(MINSIZE[idx]>1){ MINSIZE[idx]=1; }
    }
    // sizes 2..4 by adding one seed-multiple to size-(k-1) reachable set
    for(int ksz=2; ksz<=4; ksz++){
        std::vector<int> prev;
        for(int i=0;i<AGN;i++) if(MINSIZE[i]==ksz-1) prev.push_back(i);
        for(int p: prev) for(int j=0;j<NS;j++) for(int lam=1;lam<4;lam++){
            int v[K]; for(int d=0;d<K;d++) v[d]=ADD[AG_V[p][d]][MUL[lam][SEED_COL[j][d]]];
            int idx=enc(v); if(MINSIZE[idx]>ksz) MINSIZE[idx]=ksz;
        }
    }
}

static inline int cap_ext(int m,int a){ return 9 - SEEDCNT[m][a]; }

struct Undo { int point; std::vector<int> blocked_inc; };

static bool place(int c, Undo& u){
    u.point=c; u.blocked_inc.clear();
    for(int m=1;m<AGN;m++){
        int a=DT[m][c];
        COUNT[m][a]++;
        if(COUNT[m][a] > cap_ext(m,a)){
            COUNT[m][a]--;
            for(int m2=1;m2<m;m2++){ int a2=DT[m2][c]; COUNT[m2][a2]--; }
            return false;
        }
    }
    bs1024 cascade; bs_zero(cascade);
    for(int m=1;m<AGN;m++){
        int a=DT[m][c];
        if(COUNT[m][a]==cap_ext(m,a)){
            const bs1024& sm=SLICE_MASK[m][a];
            for(int i=0;i<NW;i++) cascade.w[i]|=sm.w[i];
        }
    }
    cascade.w[c>>6] &= ~(1ULL<<(c&63));
    for(int i=0;i<NW;i++) cascade.w[i] &= ~IN_PATH.w[i];
    for(int wi=0;wi<NW;wi++){
        uint64_t bits=cascade.w[wi];
        while(bits){
            int b=__builtin_ctzll(bits); bits&=bits-1;
            int c2=(wi<<6)+b;
            BLOCKED[c2]++;
            u.blocked_inc.push_back(c2);
            if(BLOCKED[c2]==1) BLOCKED_MASK.w[wi]|=(1ULL<<b);
        }
    }
    IN_PATH.w[c>>6]|=(1ULL<<(c&63));
    return true;
}
static void unplace(const Undo& u){
    int c=u.point;
    for(int c2:u.blocked_inc){ BLOCKED[c2]--; if(BLOCKED[c2]==0) BLOCKED_MASK.w[c2>>6]&=~(1ULL<<(c2&63)); }
    for(int m=1;m<AGN;m++){ int a=DT[m][c]; COUNT[m][a]--; }
    IN_PATH.w[c>>6]&=~(1ULL<<(c&63));
}

// Verify subspace-load bound (k<=5) on a completed depth-13 configuration.
// Returns true if all PG(3,4) inside every load-9 hyperplane have load <=5.
static int SEED_IDX[NS];
static bool verify_k5(const std::vector<int>& path){
    // recompute hyperplane loads (seed + path) and find load-9 ones
    for(int m=1;m<AGN;m++){
        for(int a=0;a<4;a++){
            int load = SEEDCNT[m][a];
            for(int c : path) if(DT[m][c]==a) load++;
            if(load==9){
                // check all PG(3,4): second direction m2, coset a2
                for(int m2=1;m2<AGN;m2++){
                    if(m2==m) continue;
                    for(int a2=0;a2<4;a2++){
                        int s=0;
                        for(int j=0;j<NS;j++) if(DT[m][SEED_IDX[j]]==a && DT[m2][SEED_IDX[j]]==a2) s++;
                        for(int c : path) if(DT[m][c]==a && DT[m2][c]==a2) s++;
                        if(s>5) return false;
                    }
                }
            }
        }
    }
    return true;
}

static void print_generator(const std::vector<int>& path){
    printf("\n\n########################################################################\n");
    printf("##   DEPTH-13 CANDIDATE FOUND  —  residual class %s\n", g_seed->label);
    printf("########################################################################\n\n");
    printf("Residual P matrix:\n");
    for(int r=0;r<K;r++) printf("  [%d %d %d %d]\n",g_seed->P[r][0],g_seed->P[r][1],g_seed->P[r][2],g_seed->P[r][3]);
    printf("\n13 extension AG(5,4) column indices: ");
    for(int c: path) printf("%d ", c);
    printf("\n\nFull 5x22 generator over GF(4)  [I_5 | P | ext]:\n");
    int G[K][NS+13];
    for(int i=0;i<K;i++){ for(int j=0;j<K;j++) G[i][j]=(i==j); for(int j=0;j<4;j++) G[i][5+j]=g_seed->P[i][j]; }
    for(size_t e=0;e<path.size();e++){ int c=path[e]; for(int i=0;i<K;i++) G[i][NS+e]=AG_V[c][i]; }
    for(int r=0;r<K;r++){ printf("  "); for(int j=0;j<NS+13;j++) printf("%d ",G[r][j]); printf("\n"); }
    printf("\n*** VERIFY with verify_gf4.cpp / ESTRELLA_DIAMOND_VERIFY_v1 BEFORE celebrating. ***\n");
    printf("*** d_min must equal 13 by exhaustive 4^6 enumeration. STOP ALL ENGINES. ***\n\n");
    fflush(stdout);
}

int main(int argc,char**argv){
    setbuf(stdout,NULL); setbuf(stderr,NULL);
    const char* label="C02"; int budget=3600;
    unsigned int rng_seed=(unsigned)time(NULL);
    bool ms2=true, ms3=true;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"--seed")&&i+1<argc) rng_seed=(unsigned)atoi(argv[++i]);
        else if(!strcmp(argv[i],"--minsize")&&i+1<argc){ int m=atoi(argv[++i]); ms2=(m==2); ms3=(m==3); }
        else if(argv[i][0]!='-'){ if(label[0]=='C'&&strlen(argv[i])>0&&argv[i][0]=='C') label=argv[i]; else budget=atoi(argv[i]); }
    }
    // re-parse positional cleanly: label then budget
    {
        std::vector<char*> pos; for(int i=1;i<argc;i++) if(argv[i][0]!='-' && !(i>1 && (!strcmp(argv[i-1],"--seed")||!strcmp(argv[i-1],"--minsize")))) pos.push_back(argv[i]);
        if(pos.size()>=1) label=pos[0];
        if(pos.size()>=2) budget=atoi(pos[1]);
    }

    for(int i=0;i<N_SEEDS;i++) if(!strcmp(SEEDS[i].label,label)) g_seed=&SEEDS[i];
    if(!g_seed){ fprintf(stderr,"Unknown class %s (have C01,C02,C03,C06,C10,C12)\n",label); return 1; }

    fprintf(stderr,"=======================================================\n");
    fprintf(stderr,"ESTRELLA_FLAUTISTA_v1 — class %s — THE LAST SHOT\n", g_seed->label);
    fprintf(stderr,"budget=%ds  rng_seed=%u  anchor min-size bias={%s%s}\n",
            budget, rng_seed, ms2?"2 ":"", ms3?"3":"");
    fprintf(stderr,"Stochastic random-restart, biased to NON-SOFT region (mystery gamma).\n");
    fprintf(stderr,"=======================================================\n");

    build_tables(g_seed);
    build_minsize();
    for(int j=0;j<NS;j++) SEED_IDX[j]=enc(SEED_COL[j]);

    // anchor pool: AG points with min-size in {2,3} (the non-soft region)
    std::vector<int> anchors;
    for(int c=1;c<AGN;c++){
        if((ms2 && MINSIZE[c]==2) || (ms3 && MINSIZE[c]==3)) anchors.push_back(c);
    }
    fprintf(stderr,"[anchors] %zu non-soft candidate anchors (min-size 2/3)\n", anchors.size());
    // report soft pool size for context
    int n4=0; for(int c=1;c<AGN;c++) if(MINSIZE[c]==4) n4++;
    fprintf(stderr,"[context] %d size-4 (soft, already swept 64/64), avoided\n", n4);

    std::mt19937 rng(rng_seed);
    std::uniform_int_distribution<int> pick_anchor(0,(int)anchors.size()-1);

    long long restarts=0, best_depth=0;
    long long depth_hist[16]={0};
    time_t t0=time(NULL);
    bool found=false;

    while(!found){
        time_t now=time(NULL);
        double el=difftime(now,t0);
        if(el>=budget) break;

        // reset state
        memset(BLOCKED,0,sizeof(BLOCKED));
        for(int m=1;m<AGN;m++) for(int a=0;a<4;a++) COUNT[m][a]=0;
        bs_zero(IN_PATH); bs_zero(BLOCKED_MASK);

        std::vector<Undo> us; us.reserve(16);
        std::vector<int> path; path.reserve(13);

        // depth 0: origin
        { Undo u; if(!place(0,u)){ continue; } us.push_back(std::move(u)); path.push_back(0); }

        // depth 1: random non-soft anchor (must be free & placeable)
        {
            int tries=0; bool ok=false;
            while(tries++<32){
                int a=anchors[pick_anchor(rng)];
                if(IN_PATH.w[a>>6]&(1ULL<<(a&63))) continue;
                if(BLOCKED_MASK.w[a>>6]&(1ULL<<(a&63))) continue;
                Undo u; if(place(a,u)){ us.push_back(std::move(u)); path.push_back(a); ok=true; break; }
            }
            if(!ok){ // unwind origin and retry
                while(!us.empty()){ unplace(us.back()); us.pop_back(); }
                restarts++; continue;
            }
        }

        // greedy randomized descent to depth 13
        int depth=2; int lowbound=0; // no lowbound: stochastic, allow any order
        while(depth<13){
            // collect free points
            int free_pts[1024]; int nf=0;
            for(int wi=0;wi<NW;wi++){
                uint64_t bits = ~IN_PATH.w[wi] & ~BLOCKED_MASK.w[wi];
                int base=wi<<6;
                while(bits){ int b=__builtin_ctzll(bits); bits&=bits-1; int c=base+b; if(c>0) free_pts[nf++]=c; }
            }
            (void)lowbound;
            if(nf==0) break; // dead end
            // random pick (stochastic); try a few until one places
            bool placed=false;
            std::uniform_int_distribution<int> pf(0,nf-1);
            for(int t=0;t<8 && !placed;t++){
                int c=free_pts[pf(rng)];
                if(IN_PATH.w[c>>6]&(1ULL<<(c&63))) continue;
                Undo u; if(place(c,u)){ us.push_back(std::move(u)); path.push_back(c); placed=true; depth++; }
            }
            if(!placed) break;
        }

        if((long long)path.size()-1 > best_depth) best_depth=(long long)path.size()-1;
        depth_hist[path.size()-1>15?15:path.size()-1]++;

        if(path.size()==13){
            // CANDIDATE. Verify k<=5 then print.
            fprintf(stderr,"\n[!!!] depth-13 reached at restart %lld. Verifying subspace-load bound...\n",restarts);
            if(verify_k5(path)){
                fprintf(stderr,"[!!!] k<=5 PASSED. Printing candidate generator.\n");
                print_generator(path);
                found=true;
            } else {
                fprintf(stderr,"[!!!] k<=5 FAILED — not a valid target code (bug or false hit). Continuing.\n");
            }
        }

        while(!us.empty()){ unplace(us.back()); us.pop_back(); }
        restarts++;

        if((restarts % 200000)==0){
            fprintf(stderr,"[t=%5.0fs] restarts=%lld (%.0f k/s) best_depth=%lld  hist:",
                    el, restarts, restarts/(el<1?1:el)/1000.0, best_depth);
            for(int d=2;d<=13;d++) if(depth_hist[d]) fprintf(stderr," d%d=%lld",d,depth_hist[d]);
            fprintf(stderr,"\n");
        }
    }

    double el=difftime(time(NULL),t0);
    fprintf(stderr,"\n=======================================================\n");
    fprintf(stderr,"FLAUTISTA %s FINISHED\n", g_seed->label);
    fprintf(stderr,"  restarts     : %lld (%.0f k/s)\n", restarts, restarts/(el<1?1:el)/1000.0);
    fprintf(stderr,"  best depth   : %lld  (target 13)\n", best_depth);
    fprintf(stderr,"  elapsed      : %.1f min\n", el/60.0);
    fprintf(stderr,"  depth histogram:");
    for(int d=2;d<=13;d++) if(depth_hist[d]) fprintf(stderr," d%d=%lld",d,depth_hist[d]);
    fprintf(stderr,"\n");
    if(found) fprintf(stderr,"  *** CANDIDATE FOUND — VERIFY EXTERNALLY NOW ***\n");
    else      fprintf(stderr,"  no depth-13 candidate in budget. The corner stays dark; close with conscience clean.\n");
    fprintf(stderr,"=======================================================\n");
    return 0;
}
