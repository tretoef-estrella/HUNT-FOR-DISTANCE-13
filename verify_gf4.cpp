// verify_gf4.cpp — Independent Verifier for [22,6,d]₄ codes
// Proyecto Estrella · R. Amichis (The Architect) · Claude (Anthropic)
// Compile: g++ -O3 -std=c++17 -o verify verify_gf4.cpp
// Run:     ./verify

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>

static const int N=22, K=6, NHP=1365, NPTS=1365, NCWD=4095;
static const int MUL[4][4]={{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};

struct Code {
    const char* name;
    int G[K][N];
};

// All seed matrices
Code seeds[] = {
    {"SEED_E1_STAR (excess=1, 24 Mar 2026)", {
        {1,1,0,1,1,0,0,1,0,0,0,1,1,0,1,1,0,1,1,1,0,1},
        {1,3,1,2,2,1,1,0,1,0,1,1,1,1,0,1,0,0,1,2,0,0},
        {2,3,3,2,3,2,0,0,0,1,1,3,3,2,0,1,1,0,0,1,1,0},
        {2,0,1,2,1,2,2,2,0,0,0,0,0,1,3,0,3,1,0,2,1,0},
        {1,1,0,1,2,3,1,1,1,0,3,0,0,2,2,2,0,2,2,3,0,0},
        {2,1,0,1,3,1,0,3,0,3,3,0,2,2,3,2,0,0,0,2,0,1}}},
    {"SEED_E3 (excess=3, REPULSION_v1)", {
        {1,0,1,0,1,1,1,0,0,1,1,0,1,0,0,1,0,1,1,1,1,0},
        {2,1,2,1,3,2,2,0,1,3,3,0,1,0,1,0,0,1,2,1,3,1},
        {1,3,1,3,0,2,1,1,3,1,3,0,3,1,0,3,1,0,0,1,1,1},
        {2,1,1,2,2,3,2,1,1,1,3,1,1,1,0,0,2,0,3,0,3,1},
        {0,2,3,0,1,2,2,3,3,0,2,1,3,1,3,2,1,3,3,2,1,3},
        {0,0,3,1,3,1,2,2,0,0,1,0,3,1,1,0,1,3,0,0,1,0}}},
    {"SEED_TRUENO (excess=4)", {
        {1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1},
        {1,3,0,2,2,0,1,0,1,0,1,1,1,1,0,1,0,0,1,2,0,0},
        {2,3,3,2,3,0,0,0,2,1,1,3,0,3,0,1,1,0,0,1,0,0},
        {2,0,1,2,1,1,2,2,2,0,0,0,0,1,3,0,1,1,0,2,3,0},
        {1,1,0,1,2,0,1,1,3,0,3,0,3,0,2,2,0,2,2,3,2,0},
        {2,1,1,1,3,1,0,3,1,3,3,0,0,0,3,2,0,0,0,2,0,1}}},
    {"SEED_RELAMPAGO (excess=6)", {
        {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {0,3,0,1,3,1,3,2,2,2,0,1,0,0,2,0,0,0,3,1,0,3},
        {2,1,0,1,0,2,3,1,1,2,0,2,3,0,0,0,3,0,3,3,0,0},
        {0,0,0,3,2,3,2,3,3,0,2,0,2,1,1,1,3,1,2,1,3,0},
        {0,3,2,2,2,3,0,0,1,2,0,0,0,1,2,2,0,1,3,1,3,0},
        {2,1,1,2,0,1,0,0,3,2,3,0,2,2,0,2,0,1,3,3,0,0}}},
    {"SEED_GOLDV8 (excess=7)", {
        {1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,1,1,0,1,0},
        {1,3,0,2,2,0,2,0,1,0,1,1,1,1,0,1,0,1,1,1,0,1},
        {2,3,0,2,3,1,1,0,0,1,1,3,0,3,0,1,0,1,0,0,3,2},
        {2,0,1,2,1,1,2,2,0,0,0,0,0,1,3,0,0,2,0,2,1,2},
        {1,1,2,1,2,0,3,1,3,0,3,0,1,0,2,2,0,1,2,1,0,3},
        {2,1,0,1,3,0,2,3,0,3,3,0,0,0,3,2,1,1,0,0,1,1}}},
    {"SEED_PIRAMIDE_ROJA (excess=1, from scratch)", {
        {1,0,0,0,1,0,0,1,1,1,0,1,1,1,1,0,1,0,1,0,1,1},
        {0,1,1,1,2,1,1,2,1,2,0,1,1,1,1,0,3,1,1,1,0,1},
        {2,0,0,0,0,3,1,0,2,2,1,1,2,1,3,1,1,3,1,1,2,1},
        {2,2,2,3,3,2,2,2,3,0,2,0,0,1,0,2,1,0,3,1,2,2},
        {3,0,2,3,2,2,1,0,2,2,1,1,1,1,0,2,2,1,3,0,1,1},
        {3,0,2,0,1,2,3,1,1,1,3,0,3,1,3,2,2,3,1,2,2,0}}}
};

// Canonicalize column to smallest nonzero leading entry = 1
void canon(int col[K]) {
    for (int i=0; i<K; i++) {
        if (col[i] != 0) {
            if (col[i] == 1) return;
            int inv = (col[i]==2) ? 3 : 2; // INV: 2->3, 3->2
            for (int j=i; j<K; j++) col[j] = MUL[inv][col[j]];
            return;
        }
    }
}

int compute_rank(int G[K][N]) {
    int M[K][N]; memcpy(M, G, sizeof(M));
    int rank=0;
    for (int c=0; c<N && rank<K; c++) {
        int piv=-1;
        for (int r=rank; r<K; r++) if(M[r][c]){piv=r; break;}
        if (piv<0) continue;
        if (piv!=rank) for(int j=0;j<N;j++) std::swap(M[rank][j],M[piv][j]);
        int inv=(M[rank][c]==1)?1:(M[rank][c]==2)?3:2;
        for(int j=0;j<N;j++) M[rank][j]=MUL[inv][M[rank][j]];
        for(int r=0;r<K;r++) {
            if(r==rank||M[r][c]==0) continue;
            int f=M[r][c];
            for(int j=0;j<N;j++) M[r][j]^=MUL[f][M[rank][j]];
        }
        rank++;
    }
    return rank;
}

void verify(Code& code) {
    printf("\n=== %s ===\n", code.name);
    int (&G)[K][N] = code.G;

    // 1. Rank
    int rank = compute_rank(G);
    printf("  Rank: %d %s\n", rank, rank==6?"OK":"FAIL");

    // 2. Enumerate all 4095 codewords, find d_min and weight enumerator
    int Aw[N+1] = {};
    int d_min = N+1;
    for (int v=1; v<(1<<(2*K)); v++) {
        int cw[N] = {};
        for (int i=0; i<K; i++) {
            int coeff = (v >> (2*i)) & 3;
            if (!coeff) continue;
            for (int j=0; j<N; j++) cw[j] ^= MUL[coeff][G[i][j]];
        }
        int w=0;
        for (int j=0; j<N; j++) if(cw[j]) w++;
        Aw[w]++;
        if (w < d_min) d_min = w;
    }
    printf("  d_min: %d\n", d_min);
    printf("  A_12: %d\n", Aw[12]);

    // 3. Weight enumerator
    printf("  Weight enumerator: ");
    for (int w=d_min; w<=N; w++) if(Aw[w]) printf("A_%d=%d ", w, Aw[w]);
    printf("\n");

    // 4. Check sum = 4095
    int total=0; for(int w=1;w<=N;w++) total+=Aw[w];
    printf("  Total codewords: %d %s\n", total, total==NCWD?"OK":"FAIL");

    // 5. Build PG(5,4) points from columns
    // 6. Compute hyperplane loads and excess
    // Enumerate all 1365 hyperplanes of PG(5,4)
    // A hyperplane normal is a point in PG(5,4): (n0,...,n5) with leading nonzero = 1
    int load_hist[N+1] = {};
    int excess = 0;
    int nhp = 0;
    for (int n0=0; n0<4; n0++)
    for (int n1=0; n1<4; n1++)
    for (int n2=0; n2<4; n2++)
    for (int n3=0; n3<4; n3++)
    for (int n4=0; n4<4; n4++)
    for (int n5=0; n5<4; n5++) {
        // Skip zero vector
        if(!n0&&!n1&&!n2&&!n3&&!n4&&!n5) continue;
        // Canonicalize: first nonzero must be 1
        int norm[6]={n0,n1,n2,n3,n4,n5};
        int lead=-1;
        for(int i=0;i<6;i++) if(norm[i]){lead=norm[i]; break;}
        if (lead!=1) continue;
        // Count how many columns are on this HP
        int load=0;
        for (int j=0; j<N; j++) {
            int dot=0;
            for(int i=0;i<K;i++) dot ^= MUL[norm[i]][G[i][j]];
            if (dot==0) load++;
        }
        load_hist[load]++;
        if (load > 9) excess += load - 9;
        nhp++;
    }
    printf("  Hyperplanes: %d %s\n", nhp, nhp==NHP?"OK":"FAIL");
    printf("  Excess: %d\n", excess);
    printf("  HP load histogram: ");
    for (int l=0; l<=N; l++) if(load_hist[l]) printf("%d:%d ", l, load_hist[l]);
    printf("\n");

    // 7. Compute mean and variance
    double mean=0; int total_load=0;
    for(int l=0;l<=N;l++) { total_load += l*load_hist[l]; }
    mean = (double)total_load / NHP;
    double var=0;
    for(int l=0;l<=N;l++) if(load_hist[l]) var += load_hist[l]*(l-mean)*(l-mean);
    var /= NHP;
    printf("  Mean load: %.4f (expected 5.4960)\n", mean);
    printf("  Variance:  %.4f (expected 4.0595)\n", var);

    // 8. Verify 22 distinct points
    int cols[N][K];
    for(int j=0;j<N;j++) {
        for(int i=0;i<K;i++) cols[j][i]=G[i][j];
        canon(cols[j]);
    }
    int distinct=1;
    for(int a=0;a<N;a++) for(int b=a+1;b<N;b++) {
        bool same=true;
        for(int i=0;i<K;i++) if(cols[a][i]!=cols[b][i]){same=false;break;}
        if(same){distinct=0; printf("  WARNING: cols %d and %d are identical!\n",a,b);}
    }
    printf("  22 distinct points: %s\n", distinct?"OK":"FAIL");
    printf("  VERDICT: %s\n", (rank==6 && d_min>=12 && nhp==NHP && total==NCWD && distinct) ? "VALID" : "INVALID");
}

int main() {
    printf("===========================================\n");
    printf("  GF(4) Code Verifier — Proyecto Estrella\n");
    printf("  R. Amichis (The Architect) + Claude\n");
    printf("===========================================\n");
    for (auto& s : seeds) verify(s);
    printf("\n--- All verifications complete ---\n");
    return 0;
}
