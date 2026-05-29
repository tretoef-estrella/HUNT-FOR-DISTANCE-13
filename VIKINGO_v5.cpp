// VIKINGO_v5 — Viking + Monk with REAL d_min verification
// Proyecto Estrella — 04 April 2026
// Engine: Claude (Anthropic) + The Architect (R. Amichis)
// Target: [22,6,13]_4 over GF(4) — THE DIAMOND
// Base: VIKINGO_v4 (fast Viking+Monk), fixed with verify_dmin()
// Bug in v4: exc() = sum(max(0,load-9)) is HP-load-excess, NOT d_min-based.
//            hp_exc=0 does NOT mean d_min>=13. v2/SAMAEL proved this.
// Fix: verify_dmin() via full 4096-codeword enumeration on every hp_exc<=2.
// Compile: g++ -O3 -march=native -std=c++17 -funroll-loops -o VIKINGO_v5 VIKINGO_v5.cpp
// Run: cd ~/Downloads && caffeinate -dims ./VIKINGO_v5
#include <cstdio>
#include <cstring>
#include <chrono>
#include <random>
using namespace std;

static const int ML[4][4]={{0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}};
static const int AD[4][4]={{0,1,2,3},{1,0,3,2},{2,3,0,1},{3,2,1,0}};
static const int IV[4]={0,1,3,2};
static int P[1365][6],NP=0;
static short PH[1365][341];
static short PC[1365];
static int L[1365],C[22],NC;
static bool U[1365];

inline void norm(int*c){for(int i=0;i<6;i++)if(c[i]){int v=IV[c[i]];for(int j=0;j<6;j++)c[j]=ML[v][c[j]];return;}}
inline int dot(const int*a,const int*b){return ML[a[0]][b[0]]^ML[a[1]][b[1]]^ML[a[2]][b[2]]^ML[a[3]][b[3]]^ML[a[4]][b[4]]^ML[a[5]][b[5]];}

void init(){
    bool S[4][4][4][4][4][4]={{{{{{}}}}}};
    for(int a=1;a<4096;a++){
        int c[6]={(a>>10)&3,(a>>8)&3,(a>>6)&3,(a>>4)&3,(a>>2)&3,a&3};
        norm(c);
        if(S[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]])continue;
        S[c[0]][c[1]][c[2]][c[3]][c[4]][c[5]]=1;
        for(int i=0;i<6;i++)P[NP][i]=c[i];NP++;
    }
    for(int p=0;p<NP;p++){PC[p]=0;for(int h=0;h<NP;h++)if(dot(P[h],P[p])==0)PH[p][PC[p]++]=h;}
}

inline int sc(int p){int s=0;for(int k=0;k<PC[p];k++){int l=L[PH[p][k]];if(l>=9)s+=10;else if(l==8)s+=3;else if(l==7)s++;}return s;}
inline void addp(int p){C[NC]=p;NC++;U[p]=1;for(int k=0;k<PC[p];k++)L[PH[p][k]]++;}
inline void remp(int idx){int p=C[idx];U[p]=0;for(int k=0;k<PC[p];k++)L[PH[p][k]]--;C[idx]=C[NC-1];NC--;}
inline int hp_exc(){int e=0;for(int h=0;h<NP;h++)if(L[h]>9)e+=L[h]-9;return e;}
inline int pres(int p){int s=0;for(int k=0;k<PC[p];k++){int l=L[PH[p][k]];if(l>=10)s+=(l-8)*(l-8);else if(l==9)s++;}return s;}
inline bool on_hp(int pt, int hp){return dot(P[pt],P[hp])==0;}

int rnk(){
    int m[6][22],r=0;
    for(int j=0;j<NC;j++)for(int i=0;i<6;i++)m[i][j]=P[C[j]][i];
    bool used[22]={};
    for(int row=0;row<6;row++){
        int pv=-1;for(int j=0;j<NC;j++)if(!used[j]&&m[row][j]){pv=j;break;}
        if(pv<0)continue;used[pv]=1;r++;
        int iv=IV[m[row][pv]];for(int i=0;i<6;i++)m[i][pv]=ML[iv][m[i][pv]];
        for(int j=0;j<NC;j++)if(j!=pv&&m[row][j]){int f=m[row][j];for(int i=0;i<6;i++)m[i][j]^=ML[f][m[i][pv]];}
    }
    return r;
}

// ========== REAL d_min: enumerate all 4^6 = 4096 codewords ==========
int verify_dmin(){
    int G[6][22];
    for(int c=0;c<22;c++)
        for(int r=0;r<6;r++)
            G[r][c]=P[C[c]][r];
    
    int dmin=23;
    for(int c0=0;c0<4;c0++)
    for(int c1=0;c1<4;c1++)
    for(int c2=0;c2<4;c2++)
    for(int c3=0;c3<4;c3++)
    for(int c4=0;c4<4;c4++)
    for(int c5=0;c5<4;c5++){
        if(c0==0&&c1==0&&c2==0&&c3==0&&c4==0&&c5==0) continue;
        int wt=0;
        for(int j=0;j<22;j++){
            int val=0;
            val=AD[val][ML[c0][G[0][j]]];
            val=AD[val][ML[c1][G[1][j]]];
            val=AD[val][ML[c2][G[2][j]]];
            val=AD[val][ML[c3][G[3][j]]];
            val=AD[val][ML[c4][G[4][j]]];
            val=AD[val][ML[c5][G[5][j]]];
            if(val) wt++;
        }
        if(wt<dmin){
            dmin=wt;
            if(dmin<12) return dmin; // early exit: worse than E1★
        }
    }
    return dmin;
}

int griesmer_n(int k, int d){
    int n=0,qi=1;
    for(int i=0;i<k;i++){n+=(d+qi-1)/qi;qi*=4;}
    return n;
}

int find_dirty(){
    for(int h=0;h<NP;h++) if(L[h]>9) return h;
    return -1;
}

void monk_surgery(int dirty_hp, mt19937 &R){
    int on[22], non=0;
    for(int i=0;i<NC;i++)
        if(on_hp(C[i], dirty_hp)) on[non++]=i;
    
    int removed[12]; int nrem=0;
    for(int i=0;i<non;i++) removed[i]=C[on[i]];
    nrem=non;
    for(int i=non-1;i>=0;i--) remp(on[i]);
    
    for(int q=0;q<nrem;q++){
        int bp=-1, bs=999999;
        for(int t=0;t<300;t++){
            int p=R()%NP; if(U[p]) continue;
            int s=sc(p);
            if(on_hp(p, dirty_hp)) s += 5;
            if(s<bs){bs=s;bp=p;}
        }
        if(bp<0) for(int p=0;p<NP;p++) if(!U[p]){bp=p;break;}
        addp(bp);
    }
}

static long long dmin_checks=0;
static int best_real_dmin=0;

int main(){
    setbuf(stdout,0);
    printf("VIKINGO_v5 — Viking + Monk with REAL d_min verification\n");
    printf("Proyecto Estrella — 04 April 2026\n\n");
    init();
    printf("PG(5,4): %d pts\n\n",NP);

    mt19937 R(chrono::steady_clock::now().time_since_epoch().count());
    auto T0=chrono::steady_clock::now();
    int pm[1365];for(int i=0;i<NP;i++)pm[i]=i;
    int be=999;long long rs=0,monks=0;

    while(1){
        rs++;
        // === PHASE 1: VIKING — Greedy + Repulsion to E1 ===
        NC=0;memset(L,0,sizeof(L));memset(U,0,sizeof(U));
        for(int i=NP-1;i>0;i--){int j=R()%(i+1);int t=pm[i];pm[i]=pm[j];pm[j]=t;}

        for(int st=0;st<22;st++){
            int bs=999999;
            for(int k=0;k<NP;k++){int p=pm[k];if(U[p])continue;int s=sc(p);if(s<bs)bs=s;if(!bs)break;}
            static int cb[64];int cn=0,th=bs+1;
            for(int k=0;k<NP&&cn<32;k++){int p=pm[k];if(U[p])continue;if(sc(p)<=th)cb[cn++]=p;}
            if(!cn)break;
            addp(cb[R()%cn]);
        }
        if(NC<22)continue;
        int e=hp_exc();

        if(e<=15){
            int sbe=e,sbc[22];memcpy(sbc,C,88);
            int stag=0,quakes=0;

            for(int it=0;it<50000&&e>0;it++){
                int pr[22];
                for(int i=0;i<22;i++) pr[i]=pres(C[i]);
                int idx[22];for(int i=0;i<22;i++)idx[i]=i;
                for(int t=0;t<3;t++){int mx=t;for(int j=t+1;j<22;j++)if(pr[idx[j]]>pr[idx[mx]])mx=j;int tmp=idx[t];idx[t]=idx[mx];idx[mx]=tmp;}
                int rv=R()%100;
                int vi=(rv<60)?idx[0]:(rv<85)?idx[1]:idx[2];
                
                int op=C[vi];
                U[op]=0;for(int k=0;k<PC[op];k++)L[PH[op][k]]--;
                C[vi]=C[NC-1];NC--;

                int bp=-1,bd=999;
                for(int t=0;t<100;t++){
                    int p=R()%NP;if(U[p])continue;
                    int d=0;for(int k=0;k<PC[p];k++)if(L[PH[p][k]]>=9)d++;
                    if(d<bd){bd=d;bp=p;}
                }
                
                if(bp>=0){
                    addp(bp);e=hp_exc();
                    if(e<=8||e<sbe){sbe=e;memcpy(sbc,C,88);stag=0;}
                    else if(e>sbe+3){remp(NC-1);addp(op);e=hp_exc();}
                } else {addp(op);}

                stag++;
                if(stag>=500){
                    int nq=3+(R()%2);
                    for(int q=0;q<nq;q++){
                        int ri=R()%(22-q);int rp=C[ri];
                        U[rp]=0;for(int k=0;k<PC[rp];k++)L[PH[rp][k]]--;
                        C[ri]=C[NC-1];NC--;
                    }
                    for(int q=0;q<nq;q++){
                        int bp2=-1,bd2=999;
                        for(int t=0;t<200;t++){int p=R()%NP;if(U[p])continue;int d=0;for(int k=0;k<PC[p];k++)if(L[PH[p][k]]>=9)d++;if(d<bd2){bd2=d;bp2=p;}}
                        if(bp2<0)for(int p=0;p<NP;p++)if(!U[p]){bp2=p;break;}
                        addp(bp2);
                    }
                    e=hp_exc();if(e<sbe){sbe=e;memcpy(sbc,C,88);}
                    stag=0;quakes++;
                    if(quakes>30)break;
                }
            }
            NC=0;memset(L,0,sizeof(L));memset(U,0,sizeof(U));
            for(int j=0;j<22;j++)addp(sbc[j]);
            e=sbe;
        }

        // === PHASE 2: MONK — If hp_exc<=10, surgical dirty-HP assault ===
        if(e>0 && e<=10){
            int monk_best=e, monk_bc[22]; memcpy(monk_bc,C,88);
            
            for(int attempt=0; attempt<30; attempt++){
                NC=0;memset(L,0,sizeof(L));memset(U,0,sizeof(U));
                for(int j=0;j<22;j++)addp(monk_bc[j]);
                
                int dirty=find_dirty();
                if(dirty<0){e=0;break;}
                
                monk_surgery(dirty, R);
                monks++;
                e=hp_exc();
                
                if(e<monk_best){
                    monk_best=e;memcpy(monk_bc,C,88);
                    if(e==0) break;
                }
                
                int sbe2=e;
                for(int it=0;it<20000&&e>0;it++){
                    int pr2[22];
                    for(int i=0;i<22;i++) pr2[i]=pres(C[i]);
                    int idx2[22];for(int i=0;i<22;i++)idx2[i]=i;
                    for(int t=0;t<3;t++){int mx=t;for(int j=t+1;j<22;j++)if(pr2[idx2[j]]>pr2[idx2[mx]])mx=j;int tmp=idx2[t];idx2[t]=idx2[mx];idx2[mx]=tmp;}
                    int rv2=R()%100;
                    int vi2=(rv2<60)?idx2[0]:(rv2<85)?idx2[1]:idx2[2];
                    
                    int op2=C[vi2];
                    U[op2]=0;for(int k=0;k<PC[op2];k++)L[PH[op2][k]]--;
                    C[vi2]=C[NC-1];NC--;
                    
                    int bp2=-1,bd2=999;
                    for(int t=0;t<100;t++){int p=R()%NP;if(U[p])continue;int d=0;for(int k=0;k<PC[p];k++)if(L[PH[p][k]]>=9)d++;if(d<bd2){bd2=d;bp2=p;}}
                    if(bp2>=0){
                        addp(bp2);e=hp_exc();
                        if(e<=8||e<sbe2){sbe2=e;}
                        else if(e>sbe2+3){remp(NC-1);addp(op2);e=hp_exc();}
                    } else {addp(op2);}
                }
                
                if(e<monk_best){monk_best=e;memcpy(monk_bc,C,88);}
                if(monk_best==0) break;
            }
            
            NC=0;memset(L,0,sizeof(L));memset(U,0,sizeof(U));
            for(int j=0;j<22;j++)addp(monk_bc[j]);
            e=monk_best;
        }

        // === PHASE 3: VERIFICATION — real d_min on hp_exc<=2 ===
        if(e<=2 && NC==22){
            if(rnk()==6){
                dmin_checks++;
                int dmin=verify_dmin();
                int real_excess=22-griesmer_n(6,dmin);
                double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
                
                printf("[%.1fs] >>> DMIN CHECK #%lld: hp_exc=%d d_min=%d REAL_EXCESS=%d r=%lld\n",
                       el,dmin_checks,e,dmin,real_excess,rs);
                
                if(dmin>best_real_dmin){
                    best_real_dmin=dmin;
                    printf("  ★ NEW BEST d_min=%d\n",dmin);
                    printf("  Columns:");for(int i=0;i<22;i++)printf(" %d",C[i]);printf("\n");
                    for(int i=0;i<6;i++){for(int j=0;j<22;j++)printf("%d ",P[C[j]][i]);printf("\n");}
                    printf("\n");
                }
                
                if(dmin>=13){
                    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
                    printf("║          ★ ★ ★  THE AMICHIS CODE  ★ ★ ★                 ║\n");
                    printf("║     VERIFIED [22,6,%d]₄ — THE DIAMOND — d_min=%d        ║\n",dmin,dmin);
                    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
                    
                    for(int i=0;i<6;i++){for(int j=0;j<22;j++)printf("%d ",P[C[j]][i]);printf("\n");}
                    
                    system("mkdir -p ESTRELLA_RECORDS");
                    FILE*f=fopen("THE_AMICHIS_CODE.txt","w");
                    if(f){
                        fprintf(f,"THE AMICHIS CODE — DIAMOND [22,6,%d]₄\n",dmin);
                        fprintf(f,"d_min: %d (VERIFIED by full 4096-codeword enumeration)\n",dmin);
                        fprintf(f,"Griesmer excess: %d\n",real_excess);
                        fprintf(f,"Columns:");for(int i=0;i<22;i++)fprintf(f," %d",C[i]);fprintf(f,"\n");
                        fprintf(f,"Matrix:\n");
                        for(int i=0;i<6;i++){for(int j=0;j<22;j++)fprintf(f,"%d ",P[C[j]][i]);fprintf(f,"\n");}
                        fclose(f);
                    }
                    f=fopen("ESTRELLA_RECORDS/THE_AMICHIS_CODE.txt","w");
                    if(f){
                        fprintf(f,"THE AMICHIS CODE — DIAMOND [22,6,%d]₄\n",dmin);
                        fprintf(f,"d_min: %d (VERIFIED)\n",dmin);
                        fprintf(f,"Griesmer excess: %d\n",real_excess);
                        fprintf(f,"Columns:");for(int i=0;i<22;i++)fprintf(f," %d",C[i]);fprintf(f,"\n");
                        fprintf(f,"Matrix:\n");
                        for(int i=0;i<6;i++){for(int j=0;j<22;j++)fprintf(f,"%d ",P[C[j]][i]);fprintf(f,"\n");}
                        fclose(f);
                    }
                    return 0;
                }
            }
        }

        // === REPORTING (hp_exc-based, clearly labeled) ===
        if(e<be){
            if(rnk()<6)continue;
            be=e;
            double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
            printf("[%.1fs] r=%lld monks=%lld hp_exc*=%d dchk=%lld best_d=%d\n",el,rs,monks,be,dmin_checks,best_real_dmin);
            for(int i=0;i<6;i++){for(int j=0;j<22;j++)printf("%d ",P[C[j]][i]);printf("\n");}
            int hist[12]={};for(int h=0;h<NP;h++)if(L[h]<=11)hist[L[h]]++;
            printf("Hist:");for(int i=0;i<=10;i++)if(hist[i])printf(" %d:%d",i,hist[i]);
            int dirty=find_dirty();
            if(dirty>=0) printf(" | dirty=HP[%d] load=%d",dirty,L[dirty]);
            printf("\n\n");

            if(be<=8){
                char fn[256];snprintf(fn,sizeof(fn),"VIKINGO_v5_hpexc%d_r%lld.txt",be,rs);
                FILE*f=fopen(fn,"w");
                if(f){
                    fprintf(f,"VIKINGO_v5 | hp_exc=%d | r=%lld | monks=%lld | 04 April 2026\n",be,rs,monks);
                    fprintf(f,"WARNING: hp_exc is NOT d_min-based excess. Real d_min not yet verified.\n\n");
                    for(int i=0;i<6;i++){for(int j=0;j<22;j++)fprintf(f,"%d ",P[C[j]][i]);fprintf(f,"\n");}
                    fclose(f);
                }
            }
        }

        if(rs%500==0){
            double el2=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
            printf("[%.0fs] r=%lld monks=%lld hp_best=%d dchk=%lld best_d=%d (%.1f r/s)\n",
                   el2,rs,monks,be,dmin_checks,best_real_dmin,rs/el2);
        }
    }
}
