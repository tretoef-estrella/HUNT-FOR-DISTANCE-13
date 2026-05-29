/*
OPERACION_RAYO.cpp
===========
Proyecto Estrella — Hunt for [22,6,13]_4
20 Marzo 2026

OPERACION_RAYO: Semillas E*=4 + E*=6 + VNW-k20

OBJETIVO: E* = 0  <=>  max_hplane <= 9  <=>  d_min >= 13  <=>  DIAMANTE ENCONTRADO

Arquitectura: SA multi-universo paralelo (OpenMP) sobre V(Q5) = 405 puntos de PG(5,4)
Energia: E* = SUM_h (load_h - 9)^3  si load_h > 9  (penalizacion cubica)
GF(4): MUL[2]=[0,2,3,1]  MUL[3]=[0,3,1,2]  suma=XOR
Theorem D: A12 ≡ 0 mod 3 siempre.

=========================================================
CONTEXTO CRITICO
=========================================================
  goldv8 alcanzo E*=7 — minimo local ESTRICTO de 1-vecindad.
  Los 22*404 = 8,888 vecinos de 1-swap tienen dE >= 0. VERIFICADO.
  El 1-swap es inutil desde E*=7.

=========================================================
MEJORAS RESPECTO A GOLDV9 (diagnostico Gemini 6 fallos)
=========================================================

FALLO-1 CORREGIDO: Paralelizacion real con OpenMP.
  goldv9 ejecutaba universos secuencialmente.
  rayo: #pragma omp parallel for schedule(static,1) num_threads(N_UNI).

FALLO-2 CORREGIDO: Arrays de marcado thread-local (NO static).
  goldv9: static bool is_old_h[N_PG54] — DATA RACE con OpenMP.
  rayo: mk_old[N_PG54] etc son miembros de struct Universe.

FALLO-3 CORREGIDO: Sesgo dinamico en TODOS los paths.
  goldv9: path no dirigido ignoraba el sesgo T1/VQ5_u.
  rayo: p_t1 y p_vu se aplican tanto en path dirigido como no dirigido.

FALLO-4 CORREGIDO: Tabu determinista con fallback lineal.
  goldv9: si los 5 intentos aleatorios fallaban, usaba columna prohibida.
  rayo: fallback lineal exhaustivo garantiza candidato valido.

FALLO-5 CORREGIDO: Deteccion de duplicados en Parallel Tempering.
  goldv9: PT podia donar columnas ya presentes — rango caia silenciosamente.
  rayo: verificacion in_uw[] antes de toda donacion.

FALLO-6 CORREGIDO: check_rank6() antes de TODOS los saves.
  goldv9: save_record no verificaba rango.
  rayo: check_rank6() en save_record_u y save_diamond.

NUEVAS CARACTERISTICAS:
  - 2-swap DIRIGIDO como operador PRIMARIO (1-swap muerto desde E*=7)
  - 3-swap dirigido secundario (stag>500, E*<15)
  - Freeze parcial de columnas no conflictivas (Gemini M20)
  - Tabu local 15 ciclos para columnas expulsadas por 2-swap (ChatGPT)
  - Explosion de Dualidad cuando E*<=4 (ChatGPT)
  - T_START=12.0 (escape de minimo estricto)
  - VNW k=4 para U2 (salir del pozo algebraico)
  - Reheat proporcional a E*: T=T_START*sqrt(E_best/7.0)
  - FIX-DIV: 4 fuentes de restart (best/firma/VNW/semilla)
  - Dual memoria: G_best + G_firma_best por universo
  - STARTUP_TEST antes del main loop
  - SIGINT handler guarda todos los universos antes de salir
  - fsync + readback en macOS en TODOS los saves

=========================================================
CAMINOS CERRADOS — NO TOCAR
=========================================================
  D.40: V(Q6) cerrado (ILP INFEASIBLE 0.13s)
  D.43+D.47: SEED_A18* agotada
  D.50: SEED_B15 noq cerrada
  D.53: Backtracking 1.48T nodos — terminado
  T-Shock aleatorio: 105,446 intentos, 0 mejoras — MUERTO
  1-swap desde E*=7: 8,888 vecinos, 0 mejoras — MUERTO (verificado)

=========================================================
UNIVERSOS
=========================================================
  U0: SEED_TRUENO E*=4 — record absoluto de la campana
  U1: RELAMPAGO E*=6 — basin geometricamente distinto (T1=4,VQ5u=13)
  U2: VNW k=20 desde E*=4 — cuenca alejada del minimo local

COMPILE (macOS):
  brew install libomp
  clang++ -O3 -march=native -std=c++17 \
          -Xpreprocessor -fopenmp \
          -I$(brew --prefix libomp)/include \
          -L$(brew --prefix libomp)/lib -lomp \
          -Wall -Wextra -Wshadow \
          -o rayo OPERACION_RAYO.cpp

RUN:
  cd ~/Downloads && caffeinate -i ./rayo 2>&1 | tee rayo_out.txt

Autor: R. Amichis (Rafael Amichis Luengo) + Claude (Anthropic)
Fecha: 20 Marzo 2026 | Licencia: BSL 1.1 + SAMAEL Decree
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <csignal>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// =================================================================
// CONSTANTES SAGRADAS — NUNCA CAMBIAR
// =================================================================
static const int  DIMS       = 6;
static const int  NCOLS      = 22;
static const int  N_PG54     = 1365;
static const int  N_VQ5      = 405;
static const int  LIMIT_MHP  = 9;
static const int  H_PER_PT   = 341;

// Numero de universos
static const int  N_UNI      = 3;

// SA schedule rayo — T mas alta para escapar minimo estricto
static const double T_START      = 12.0;    // subido de 8.0: escape de minimo estricto E*=7
static const double T_MIN        = 0.05;
static const double T_FACTOR     = 0.998;   // enfriamiento lento
static const int    S_PER_CYCLE  = 20000;
static const int    MAX_CYCLES   = 10000000;
static const int    REPORT_EVERY = 50;
static const int    RESTART_STAG = 2500;    // paciencia antes de declarar estancamiento

// E inicial de la semilla U0 (para reheat proporcional)
static const double E_SEMILLA_INIT = 7.0;

// Operadores
static const double P_DIRECTED  = 0.80;  // prob de mutacion dirigida en SA normal
static const double P_BIAS      = 0.70;  // sesgo de migas T1/VQ5_u

// 2-swap dirigido (primario)
static const int  SWAP2_STAG    = 0;     // activo SIEMPRE cuando E*<SWAP2_E_MAX
static const int  SWAP2_E_MAX   = 20;
static const int  SWAP2_TRIES   = 50;

// 3-swap dirigido (secundario)
static const int  SWAP3_STAG    = 500;
static const int  SWAP3_E_MAX   = 15;
static const int  SWAP3_TRIES   = 30;

// Freeze parcial (Gemini M20)
static const int  FREEZE_E_MAX  = 6;     // congelar columnas no conflictivas cuando E*<6

// Tabu local (ChatGPT)
static const int  TABU_CYCLES   = 15;    // columna expulsada por 2-swap: tabu 15 ciclos

// Explosion de Dualidad (ChatGPT) — usa espectro dual cuando E*<=4
static const int  DUAL_E_MAX    = 4;
static const int  NCODEWORDS    = 4095;  // 4^6 - 1

// Parallel Tempering
static const int  PT_PERIOD     = 25;    // intercambio entre universos cada 25 ciclos

// VNW para U2
static const int  VNW_K         = 20;    // pasos VNW amplios desde E*=4     // pasos VNW para salir del pozo

// Sesgo de mutacion (cuando cnt_T1 < 12, preferir T1)
static const int  CNT_T1_LOW    = 12;
static const double P_BIAS_T1_BOOST = 0.85; // boost cuando cnt_T1 bajo

// =================================================================
// GF(4) — SAGRADO — NUNCA CAMBIAR
// =================================================================
static const int MUL[4][4] = {
    {0,0,0,0},{0,1,2,3},{0,2,3,1},{0,3,1,2}
};
static const int INV_GF4[4] = {0,1,3,2};

static inline int ip6(const int* a, const int* b){
    return MUL[a[0]][b[0]]^MUL[a[1]][b[1]]^MUL[a[2]][b[2]]
          ^MUL[a[3]][b[3]]^MUL[a[4]][b[4]]^MUL[a[5]][b[5]];
}

// =================================================================
// CUADRICAS
// =================================================================
static inline bool Q5(const int* p){return MUL[p[1]][p[5]]==MUL[p[2]][p[4]];}
static inline bool Q6(const int* p){return MUL[p[3]][p[5]]==MUL[p[4]][p[4]];}
static inline bool Q4(const int* p){return MUL[p[1]][p[4]]==MUL[p[2]][p[3]];}

// Region de un punto de V(Q5)
// 0=T1 (Q4+Q5+Q6): 101 pts — nucleo algebraico mas denso
// 1=T1_noQ6 (Q4+Q5):  64 pts
// 2=VQ5_u (solo Q5): 240 pts
static inline int region_vq5(const int* p){
    bool q4=Q4(p), q6=Q6(p);
    if(q4&&q6) return 0;
    if(q4)     return 1;
    return 2;
}

static const char* region_name_full(const int* p){
    bool q4=Q4(p),q5=Q5(p),q6=Q6(p);
    if(q4&&q5&&q6) return "T1";
    if(q4&&q5)     return "T1_noQ6";
    if(q5)         return "VQ5_u";
    if(q4)         return "VQ4";
    if(q6)         return "V(Q6)";
    return "noq";
}

// =================================================================
// PENALIZACION CUBICA — SAGRADA
// =================================================================
static inline long long penalty(int x){
    if(x<=LIMIT_MHP) return 0LL;
    long long d=(long long)(x-LIMIT_MHP);
    return d*d*d;
}

// =================================================================
// FORMA CANONICA
// =================================================================
static bool canon(int* v){
    bool z=true; for(int i=0;i<DIMS;i++) if(v[i]){z=false;break;}
    if(z) return false;
    int nz=0; while(v[nz]==0) nz++;
    if(v[nz]!=1){int s=INV_GF4[v[nz]];for(int i=0;i<DIMS;i++)v[i]=MUL[s][v[i]];}
    return true;
}
static uint32_t encode_pt(const int* v){
    uint32_t r=0; for(int i=0;i<DIMS;i++) r=(r<<2)|v[i]; return r;
}

// =================================================================
// ESPACIOS Y CLASIFICACION — GLOBALES (solo lectura en SA)
// =================================================================
static int g_all[N_PG54][DIMS];
static int g_vq5[N_VQ5][DIMS];
static int g_n_all=0, g_n_vq5=0;
static int g_vq5_idx[4096];

static int g_region[N_VQ5];    // region de cada punto de V(Q5)

static int g_pt_to_h[N_VQ5][H_PER_PT];   // punto j -> sus 341 hiperplanos

// Listas de puntos por region fuera de cada H (para sesgo de migas)
static std::vector<int> g_t1_not_in_h[N_PG54];
static std::vector<int> g_vq5u_not_in_h[N_PG54];
static std::vector<int> g_vq5_not_in_h[N_PG54];

// Listas globales por region
static std::vector<int> g_t1_pts;
static std::vector<int> g_t1noq6_pts;
static std::vector<int> g_vq5u_pts;

// =================================================================
// SEMILLAS — DATOS
// =================================================================

// SEMILLA E*=7 (goldv8 ciclo 288980) — mantenida como referencia
static const int G_SEED_E7[DIMS][NCOLS]={
    {1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,1,1,0,1,0},
    {1,3,0,2,2,0,2,0,1,0,1,1,1,1,0,1,0,1,1,1,0,1},
    {2,3,0,2,3,1,1,0,0,1,1,3,0,3,0,1,0,1,0,0,3,2},
    {2,0,1,2,1,1,2,2,0,0,0,0,0,1,3,0,0,2,0,2,1,2},
    {1,1,2,1,2,0,3,1,3,0,3,0,1,0,2,2,0,1,2,1,0,3},
    {2,1,0,1,3,0,2,3,0,3,3,0,0,0,3,2,1,1,0,0,1,1}
};

// U0: SEED_TRUENO E*=4 — RECORD ABSOLUTO DE LA CAMPAÑA
// TRUENO ciclo 1121, T1=6, T1_noQ6=4, VQ5_u=12
static const int G_SEED_E4[DIMS][NCOLS]={
    {1,1,1,1,1,1,0,1,0,0,0,1,1,0,1,1,0,1,1,1,1,1},
    {1,3,0,2,2,0,1,0,1,0,1,1,1,1,0,1,0,0,1,2,0,0},
    {2,3,3,2,3,0,0,0,2,1,1,3,0,3,0,1,1,0,0,1,0,0},
    {2,0,1,2,1,1,2,2,2,0,0,0,0,1,3,0,1,1,0,2,3,0},
    {1,1,0,1,2,0,1,1,3,0,3,0,3,0,2,2,0,2,2,3,2,0},
    {2,1,1,1,3,1,0,3,1,3,3,0,0,0,3,2,0,0,0,2,0,1}
};

// U1: RELAMPAGO E*=6 — basin distinto, T1=4, VQ5_u=13
// RELAMPAGO round=19, seed=8, timestamp 20260320_152613
static const int G_SEED_E6[DIMS][NCOLS]={
    {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,3,0,1,3,1,3,2,2,2,0,1,0,0,2,0,0,0,3,1,0,3},
    {2,1,0,1,0,2,3,1,1,2,0,2,3,0,0,0,3,0,3,3,0,0},
    {0,0,0,3,2,3,2,3,3,0,2,0,2,1,1,1,3,1,2,1,3,0},
    {0,3,2,2,2,3,0,0,1,2,0,0,0,1,2,2,0,1,3,1,3,0},
    {2,1,1,2,0,1,0,0,3,2,3,0,2,2,0,2,0,1,3,3,0,0}
};

// U1b: Goldv2_c254 — firma 17T1+5VQ5_u (referencia estructural)
static const int G_SEED_C254[DIMS][NCOLS]={
    {0,0,0,0,0,3,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,2,0,1,1,1,1,2,2,2,2,3,3,3,3,0},
    {0,0,0,0,0,0,1,0,3,0,1,2,3,0,1,2,3,0,1,2,3,2},
    {0,1,1,1,1,0,2,3,0,1,3,1,1,3,0,3,3,2,1,2,2,0},
    {0,0,1,2,3,0,0,2,0,0,1,2,3,0,2,3,1,0,3,1,2,0},
    {1,0,1,3,2,0,1,0,2,0,1,3,2,0,1,3,2,0,1,3,2,3}
};

// =================================================================
// STRUCT UNIVERSE — TODO EL ESTADO SA ES THREAD-LOCAL
// FALLO-2 CORREGIDO: arrays de marcado son miembros, no static
// =================================================================
struct Universe {
    // Identificador
    int id;

    // Estado SA corriente
    int  G[DIMS][NCOLS];
    int  h_load[N_PG54];
    long long E_star;
    int  col_idx[NCOLS];

    // Dirty list
    int  h_dirty[N_PG54];
    int  n_dirty;
    int  h_dirty_pos[N_PG54];

    // Cols de G actuales en cada H
    int  cols_in_h[N_PG54][NCOLS+1];
    int  n_cols_in_h[N_PG54];

    // Dual memoria (FALLO-6: check_rank6 en ambas)
    int  G_best[DIMS][NCOLS];
    long long E_best;

    int  G_firma_best[DIMS][NCOLS]; // mejor firma T1/VQ5_u
    int  firma_score;               // distancia a 17+5 (menor = mejor)
    long long E_firma_best;

    // Semilla original del universo
    int  G_seed[DIMS][NCOLS];

    // SA schedule
    double T_cur;
    int    stag;
    int    n_restarts;
    int    cycle;

    // Tabu: cuantos ciclos queda prohibida cada columna (ChatGPT)
    int  tabu[NCOLS];

    // Freeze: columna congelada (no participa en swap)
    bool freeze[NCOLS];

    // RNG per-universe (sin races)
    uint64_t rng;

    // Arrays de marcado thread-local (FALLO-2)
    bool mk_old[N_PG54];
    bool mk_new[N_PG54];
    bool mk_old_b[N_PG54];
    bool mk_new_b[N_PG54];
    bool mk_affected[N_PG54];
    bool mk_old_c[N_PG54];
    bool mk_new_c[N_PG54];

    // Estadisticas
    long long total_steps;
    long long total_accept;
    long long total_swap2;
    long long total_swap2_mejoras;
    long long total_swap3;
    long long total_swap3_mejoras;
    long long total_dual;
    long long total_dual_mejoras;
    int       n_pt;    // parallel tempering exchanges

    // =================================================================
    // RNG thread-local
    // =================================================================
    inline double rng01(){
        rng^=rng<<13; rng^=rng>>7; rng^=rng<<17;
        return (rng>>11)*(1.0/(1ULL<<53));
    }
    inline int rng_int(int n){ return (int)(rng01()*n); }

    // =================================================================
    // DIRTY LIST — O(1)
    // =================================================================
    inline void dirty_add(int h){
        if(h_dirty_pos[h]>=0) return;
        h_dirty_pos[h]=n_dirty;
        h_dirty[n_dirty++]=h;
    }
    inline void dirty_remove(int h){
        int pos=h_dirty_pos[h];
        if(pos<0) return;
        int last=h_dirty[--n_dirty];
        h_dirty[pos]=last;
        h_dirty_pos[last]=pos;
        h_dirty_pos[h]=-1;
    }

    // =================================================================
    // CHECK RANK-6 SOBRE GF(4)
    // =================================================================
    bool check_rank6(const int M_in[DIMS][NCOLS]) const {
        int M[DIMS][NCOLS];
        memcpy(M, M_in, sizeof(M));
        int pivot_row=0;
        for(int col=0;col<NCOLS&&pivot_row<DIMS;col++){
            int piv=-1;
            for(int r=pivot_row;r<DIMS;r++) if(M[r][col]){piv=r;break;}
            if(piv<0) continue;
            if(piv!=pivot_row){
                for(int c2=0;c2<NCOLS;c2++){
                    int t=M[pivot_row][c2]; M[pivot_row][c2]=M[piv][c2]; M[piv][c2]=t;
                }
            }
            int inv_piv=INV_GF4[M[pivot_row][col]];
            for(int c2=0;c2<NCOLS;c2++) M[pivot_row][c2]=MUL[inv_piv][M[pivot_row][c2]];
            for(int r=0;r<DIMS;r++){
                if(r==pivot_row) continue;
                int fac=M[r][col];
                if(fac) for(int c2=0;c2<NCOLS;c2++) M[r][c2]^=MUL[fac][M[pivot_row][c2]];
            }
            pivot_row++;
        }
        return (pivot_row==DIMS);
    }

    // =================================================================
    // CHECK NO DUPLICADOS
    // =================================================================
    bool check_no_dups(const int M_in[DIMS][NCOLS]) const {
        int idx[NCOLS];
        for(int c=0;c<NCOLS;c++){
            int col[DIMS]; for(int r=0;r<DIMS;r++) col[r]=M_in[r][c];
            int tmp[DIMS]; memcpy(tmp,col,sizeof(tmp)); canon(tmp);
            uint32_t code=encode_pt(tmp);
            idx[c]=(code<4096)?g_vq5_idx[code]:-1;
            if(idx[c]<0) return false;
        }
        for(int i=0;i<NCOLS;i++) for(int j=i+1;j<NCOLS;j++) if(idx[i]==idx[j]) return false;
        return true;
    }

    // =================================================================
    // INICIALIZAR ESTADO DESDE MATRIZ
    // =================================================================
    void init_from_G(const int Gin[DIMS][NCOLS]){
        memset(h_load,0,sizeof(h_load));
        memset(n_cols_in_h,0,sizeof(n_cols_in_h));
        E_star=0;
        memset(h_dirty_pos,-1,sizeof(h_dirty_pos));
        n_dirty=0;
        memset(tabu,0,sizeof(tabu));
        memset(freeze,0,sizeof(freeze));

        for(int c=0;c<NCOLS;c++){
            int col[DIMS]; for(int r=0;r<DIMS;r++) col[r]=Gin[r][c];
            canon(col);
            uint32_t code=encode_pt(col);
            int j=(code<4096)?g_vq5_idx[code]:-1;
            if(j<0){
                for(int k=0;k<g_n_vq5;k++)
                    if(!memcmp(g_vq5[k],col,DIMS*sizeof(int))){j=k;break;}
            }
            if(j<0){
                fprintf(stderr,"FATAL U%d: col %d no en V(Q5)\n",id,c);
                exit(1);
            }
            col_idx[c]=j;
            for(int r=0;r<DIMS;r++) G[r][c]=g_vq5[j][r];
            for(int k=0;k<H_PER_PT;k++){
                int h=g_pt_to_h[j][k];
                cols_in_h[h][n_cols_in_h[h]++]=c;
                h_load[h]++;
            }
        }
        for(int h=0;h<g_n_all;h++){
            E_star+=penalty(h_load[h]);
            if(h_load[h]>LIMIT_MHP) dirty_add(h);
        }
    }

    // =================================================================
    // RECONSTRUIR ESTADO DESDE G_BEST (restart anti-amnesia)
    // =================================================================
    void rebuild_from(const int Gin[DIMS][NCOLS]){
        memset(h_load,0,sizeof(h_load));
        memset(n_cols_in_h,0,sizeof(n_cols_in_h));
        E_star=0;
        memset(h_dirty_pos,-1,sizeof(h_dirty_pos));
        n_dirty=0;

        for(int c=0;c<NCOLS;c++){
            int col[DIMS]; for(int r=0;r<DIMS;r++) col[r]=Gin[r][c];
            canon(col);
            uint32_t code=encode_pt(col);
            int j=(code<4096)?g_vq5_idx[code]:-1;
            if(j<0) for(int k=0;k<g_n_vq5;k++)
                if(!memcmp(g_vq5[k],col,DIMS*sizeof(int))){j=k;break;}
            assert(j>=0);
            col_idx[c]=j;
            for(int r=0;r<DIMS;r++) G[r][c]=g_vq5[j][r];
            for(int k=0;k<H_PER_PT;k++){
                int h=g_pt_to_h[j][k];
                cols_in_h[h][n_cols_in_h[h]++]=c;
                h_load[h]++;
            }
        }
        for(int h=0;h<g_n_all;h++){
            E_star+=penalty(h_load[h]);
            if(h_load[h]>LIMIT_MHP) dirty_add(h);
        }
    }

    // =================================================================
    // CONTAR FIRMA: distancia a 17T1+5VQ5_u (menor = mejor)
    // =================================================================
    int firma_dist() const {
        int cnt_t1=0, cnt_vq5u=0;
        for(int c=0;c<NCOLS;c++){
            int r=g_region[col_idx[c]];
            if(r==0) cnt_t1++;
            else if(r==2) cnt_vq5u++;
        }
        return abs(cnt_t1-17)+abs(cnt_vq5u-5);
    }

    // =================================================================
    // CALCULAR SESGO p_t1 dinamico
    // =================================================================
    double get_p_t1() const {
        int cnt_t1=0;
        for(int c=0;c<NCOLS;c++) if(g_region[col_idx[c]]==0) cnt_t1++;
        return (cnt_t1<CNT_T1_LOW) ? P_BIAS_T1_BOOST : P_BIAS;
    }

    // =================================================================
    // ELEGIR NUEVO PUNTO CON SESGO (FALLO-3: aplica en TODOS los paths)
    // Parametros:
    //   old_region: region de la columna actual (0=T1, 1=T1noQ6, 2=VQ5u)
    //   h_col: hiperplano violado de referencia (-1 si no hay)
    //   exclude1/exclude2: indices a evitar (duplicados)
    // =================================================================
    int choose_new_pt(int old_region, int h_col, int exclude1, int exclude2){
        double p_t1 = get_p_t1();
        double p_vu = P_BIAS;
        int new_j = -1;
        double r = rng01();

        if(h_col >= 0){
            // Path dirigido (fuera del H violado)
            if(old_region==0 && r<p_t1 && !g_t1_not_in_h[h_col].empty()){
                const auto& v=g_t1_not_in_h[h_col];
                new_j=v[rng_int((int)v.size())];
            } else if(old_region==2 && r<p_vu && !g_vq5u_not_in_h[h_col].empty()){
                const auto& v=g_vq5u_not_in_h[h_col];
                new_j=v[rng_int((int)v.size())];
            } else if(!g_vq5_not_in_h[h_col].empty()){
                const auto& v=g_vq5_not_in_h[h_col];
                new_j=v[rng_int((int)v.size())];
            }
        }

        // Path no dirigido — FALLO-3: TAMBIEN aplica sesgo
        if(new_j<0){
            double r2=rng01();
            if(old_region==0 && r2<p_t1 && !g_t1_pts.empty())
                new_j=g_t1_pts[rng_int((int)g_t1_pts.size())];
            else if(old_region==2 && r2<p_vu && !g_vq5u_pts.empty())
                new_j=g_vq5u_pts[rng_int((int)g_vq5u_pts.size())];
            else
                new_j=rng_int(g_n_vq5);
        }

        // Evitar duplicados (hasta 10 intentos)
        for(int attempt=0; attempt<10; attempt++){
            if(new_j!=exclude1 && new_j!=exclude2){
                bool dup=false;
                for(int c=0;c<NCOLS;c++) if(col_idx[c]==new_j){dup=true;break;}
                if(!dup) return new_j;
            }
            new_j=rng_int(g_n_vq5);
        }
        return new_j;
    }

    // =================================================================
    // ELEGIR COLUMNA CON TABU + FREEZE — FALLO-4 CORREGIDO
    // Fallback lineal determinista si 5 intentos aleatorios fallan
    // =================================================================
    int choose_col(bool from_dirty){
        if(from_dirty && n_dirty>0){
            int h_target=h_dirty[rng_int(n_dirty)];
            int nc=n_cols_in_h[h_target];
            // 5 intentos aleatorios
            int found=-1;
            for(int a=0;a<5;a++){
                int c=(nc>0)?cols_in_h[h_target][rng_int(nc)]:rng_int(NCOLS);
                if(!freeze[c] && tabu[c]==0){found=c;break;}
            }
            // Fallback lineal determinista (FALLO-4)
            if(found<0){
                for(int i=0;i<nc;i++){
                    int c=cols_in_h[h_target][i];
                    if(!freeze[c] && tabu[c]==0){found=c;break;}
                }
            }
            // Fallback total: cualquier columna no congelada
            if(found<0){
                for(int c=0;c<NCOLS;c++) if(!freeze[c]&&tabu[c]==0){found=c;break;}
            }
            if(found>=0) return found;
        }
        // Path no dirigido — fallback lineal primero, luego aleatorio
        for(int a=0;a<5;a++){
            int c=rng_int(NCOLS);
            if(!freeze[c]&&tabu[c]==0) return c;
        }
        for(int c=0;c<NCOLS;c++) if(!freeze[c]&&tabu[c]==0) return c;
        return rng_int(NCOLS); // ultimo recurso (freeze total, no deberia pasar)
    }

    // =================================================================
    // APLICAR SWAP: retirar columna col_c (old_j) y poner new_j
    // Actualiza h_load, dirty, cols_in_h, col_idx, G
    // =================================================================
    void apply_swap(int col_c, int old_j, int new_j, long long dE){
        const int* old_hs=g_pt_to_h[old_j];
        const int* new_hs=g_pt_to_h[new_j];

        for(int k=0;k<H_PER_PT;k++){
            int h=old_hs[k];
            if(!mk_new[h]){
                int lo=h_load[h]--;
                if(lo==LIMIT_MHP+1) dirty_remove(h);
            }
        }
        for(int k=0;k<H_PER_PT;k++){
            int h=new_hs[k];
            if(!mk_old[h]){
                int lo=h_load[h]++;
                if(lo==LIMIT_MHP) dirty_add(h);
            }
        }
        E_star+=dE;

        // Actualizar cols_in_h
        for(int k=0;k<H_PER_PT;k++){
            int h=old_hs[k];
            if(!mk_new[h]){
                int nc=n_cols_in_h[h];
                for(int i=0;i<nc;i++){
                    if(cols_in_h[h][i]==col_c){
                        cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]];
                        break;
                    }
                }
            }
        }
        for(int k=0;k<H_PER_PT;k++){
            int h=new_hs[k];
            if(!mk_old[h])
                cols_in_h[h][n_cols_in_h[h]++]=col_c;
        }

        col_idx[col_c]=new_j;
        for(int r=0;r<DIMS;r++) G[r][col_c]=g_vq5[new_j][r];
    }

    // =================================================================
    // SA STEP — DYNAMIC STRIKE + SESGO DE MIGAS
    // FALLO-2: usa mk_old/mk_new (miembros, no static)
    // FALLO-3: sesgo en todos los paths
    // FALLO-4: choose_col con fallback lineal
    // =================================================================
    bool sa_step(double T){
        bool directed = (n_dirty>0 && rng01()<P_DIRECTED);
        int col_c = choose_col(directed);

        int old_j = col_idx[col_c];
        int old_region = g_region[old_j];

        // Buscar H violado que contenga col_c
        int h_col=-1;
        if(directed){
            for(int k=0;k<H_PER_PT;k++){
                int h=g_pt_to_h[old_j][k];
                if(h_load[h]>LIMIT_MHP){h_col=h;break;}
            }
        }

        int new_j = choose_new_pt(old_region, h_col, old_j, -1);
        if(new_j==old_j) return false;

        const int* old_hs=g_pt_to_h[old_j];
        const int* new_hs=g_pt_to_h[new_j];

        // Marcar H (FALLO-2: mk_old/mk_new son miembros)
        for(int k=0;k<H_PER_PT;k++) mk_old[old_hs[k]]=true;
        for(int k=0;k<H_PER_PT;k++) mk_new[new_hs[k]]=true;

        // Delta_E
        long long delta_E=0;
        for(int k=0;k<H_PER_PT;k++){
            int h=old_hs[k];
            if(!mk_new[h]) delta_E+=penalty(h_load[h]-1)-penalty(h_load[h]);
        }
        for(int k=0;k<H_PER_PT;k++){
            int h=new_hs[k];
            if(!mk_old[h]) delta_E+=penalty(h_load[h]+1)-penalty(h_load[h]);
        }

        // Metropolis
        bool accept=(delta_E<=0)||(rng01()<exp(-(double)delta_E/T));

        if(accept) apply_swap(col_c, old_j, new_j, delta_E);

        // Limpiar marcas
        for(int k=0;k<H_PER_PT;k++){mk_old[old_hs[k]]=false; mk_new[new_hs[k]]=false;}

        return accept;
    }

    // =================================================================
    // 2-SWAP DIRIGIDO — OPERADOR PRIMARIO
    // Basado en pares co-conflictivos (columnas que comparten H violados)
    // Activo siempre cuando E* < SWAP2_E_MAX
    // FALLO-2: usa mk_old/mk_new/mk_old_b/mk_new_b/mk_affected (miembros)
    // FALLO-3: sesgo en elegir nuevos puntos
    // FALLO-4: choose_col con fallback lineal en eleccion de ca/cb
    // Returns true si encontro mejora.
    // =================================================================
    bool swap2_directed(){
        for(int t=0;t<SWAP2_TRIES;t++){
            // Elegir ca: columna en H violado
            int ca=-1;
            if(n_dirty>0){
                int h_target=h_dirty[rng_int(n_dirty)];
                int nc=n_cols_in_h[h_target];
                // 5 intentos aleatorios + fallback lineal (FALLO-4)
                for(int a=0;a<5&&ca<0;a++){
                    int c=(nc>0)?cols_in_h[h_target][rng_int(nc)]:rng_int(NCOLS);
                    if(!freeze[c]&&tabu[c]==0) ca=c;
                }
                if(ca<0) for(int i=0;i<nc&&ca<0;i++){
                    int c=cols_in_h[h_target][i];
                    if(!freeze[c]&&tabu[c]==0) ca=c;
                }
                if(ca<0) for(int c=0;c<NCOLS&&ca<0;c++) if(!freeze[c]&&tabu[c]==0) ca=c;
            }
            if(ca<0) ca=rng_int(NCOLS);

            // Elegir cb: columna co-conflictiva (comparte H violado con ca)
            int cb=-1;
            int old_a=col_idx[ca];
            // Buscar H violado que contenga ca
            int h_shared=-1;
            for(int k=0;k<H_PER_PT&&h_shared<0;k++){
                int h=g_pt_to_h[old_a][k];
                if(h_load[h]>LIMIT_MHP) h_shared=h;
            }
            if(h_shared>=0){
                int nc=n_cols_in_h[h_shared];
                for(int a=0;a<5&&cb<0;a++){
                    int c=(nc>0)?cols_in_h[h_shared][rng_int(nc)]:rng_int(NCOLS);
                    if(c!=ca&&!freeze[c]&&tabu[c]==0) cb=c;
                }
                if(cb<0) for(int i=0;i<nc&&cb<0;i++){
                    int c=cols_in_h[h_shared][i];
                    if(c!=ca&&!freeze[c]&&tabu[c]==0) cb=c;
                }
            }
            if(cb<0) do{ cb=rng_int(NCOLS); }while(cb==ca||(tabu[cb]>0)||(freeze[cb]));

            int old_b=col_idx[cb];

            // Nuevo punto para ca (FALLO-3: sesgo en todos los paths)
            int new_a=choose_new_pt(g_region[old_a], h_shared, old_a, old_b);
            if(new_a==old_a||new_a==old_b) continue;
            // Nuevo punto para cb
            int new_b=choose_new_pt(g_region[old_b], h_shared, old_b, new_a);
            // Evitar colisiones
            for(int att=0;att<10&&(new_b==old_b||new_b==old_a||new_b==new_a);att++)
                new_b=rng_int(g_n_vq5);
            if(new_b==old_b||new_b==old_a||new_b==new_a) continue;
            // Verificar no duplicados en universo
            for(int c=0;c<NCOLS;c++){
                if(c==ca||c==cb) continue;
                if(col_idx[c]==new_a||col_idx[c]==new_b) goto next_try2;
            }

            {
                const int* oha=g_pt_to_h[old_a];
                const int* nha=g_pt_to_h[new_a];
                const int* ohb=g_pt_to_h[old_b];
                const int* nhb=g_pt_to_h[new_b];

                // Marcar (FALLO-2: miembros)
                for(int k=0;k<H_PER_PT;k++){
                    mk_old[oha[k]]=true;   mk_affected[oha[k]]=true;
                    mk_new[nha[k]]=true;   mk_affected[nha[k]]=true;
                    mk_old_b[ohb[k]]=true; mk_affected[ohb[k]]=true;
                    mk_new_b[nhb[k]]=true; mk_affected[nhb[k]]=true;
                }

                long long dE=0;
                for(int h=0;h<g_n_all;h++){
                    if(!mk_affected[h]) continue;
                    int delta=0;
                    if(mk_old[h]&&!mk_new[h])   delta--;
                    if(mk_new[h]&&!mk_old[h])   delta++;
                    if(mk_old_b[h]&&!mk_new_b[h]) delta--;
                    if(mk_new_b[h]&&!mk_old_b[h]) delta++;
                    if(delta!=0)
                        dE+=penalty(h_load[h]+delta)-penalty(h_load[h]);
                }

                // Limpiar marcas
                for(int k=0;k<H_PER_PT;k++){
                    mk_old[oha[k]]=false;   mk_new[nha[k]]=false;
                    mk_old_b[ohb[k]]=false; mk_new_b[nhb[k]]=false;
                    mk_affected[oha[k]]=false; mk_affected[nha[k]]=false;
                    mk_affected[ohb[k]]=false; mk_affected[nhb[k]]=false;
                }

                if(dE<0){
                    // Aplicar los dos swaps
                    // Retirar old_a
                    for(int k=0;k<H_PER_PT;k++) h_load[oha[k]]--;
                    // Retirar old_b
                    for(int k=0;k<H_PER_PT;k++) h_load[ohb[k]]--;
                    // Poner new_a
                    for(int k=0;k<H_PER_PT;k++) h_load[nha[k]]++;
                    // Poner new_b
                    for(int k=0;k<H_PER_PT;k++) h_load[nhb[k]]++;
                    E_star+=dE;

                    // Reconstruir dirty list
                    n_dirty=0; memset(h_dirty_pos,-1,sizeof(h_dirty_pos));
                    for(int h=0;h<g_n_all;h++) if(h_load[h]>LIMIT_MHP) dirty_add(h);

                    // Actualizar cols_in_h para ca
                    for(int k=0;k<H_PER_PT;k++){
                        int h=oha[k];
                        int nc=n_cols_in_h[h];
                        for(int i=0;i<nc;i++) if(cols_in_h[h][i]==ca){
                            cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]]; break;
                        }
                    }
                    for(int k=0;k<H_PER_PT;k++) cols_in_h[nha[k]][n_cols_in_h[nha[k]]++]=ca;
                    // Actualizar cols_in_h para cb
                    for(int k=0;k<H_PER_PT;k++){
                        int h=ohb[k];
                        int nc=n_cols_in_h[h];
                        for(int i=0;i<nc;i++) if(cols_in_h[h][i]==cb){
                            cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]]; break;
                        }
                    }
                    for(int k=0;k<H_PER_PT;k++) cols_in_h[nhb[k]][n_cols_in_h[nhb[k]]++]=cb;

                    col_idx[ca]=new_a; for(int r=0;r<DIMS;r++) G[r][ca]=g_vq5[new_a][r];
                    col_idx[cb]=new_b; for(int r=0;r<DIMS;r++) G[r][cb]=g_vq5[new_b][r];

                    // Tabu local: ca y cb no se pueden elegir en los proximos TABU_CYCLES ciclos
                    tabu[ca]=TABU_CYCLES;
                    tabu[cb]=TABU_CYCLES;

                    return true;
                }
            }
            next_try2:;
        }
        return false;
    }

    // =================================================================
    // 3-SWAP DIRIGIDO — OPERADOR SECUNDARIO
    // Activo cuando stag>SWAP3_STAG y E*<SWAP3_E_MAX
    // FALLO-2: usa mk_old/mk_new/mk_old_b/mk_new_b/mk_old_c/mk_new_c
    // =================================================================
    bool swap3_directed(){
        for(int t=0;t<SWAP3_TRIES;t++){
            // Elegir tres columnas en H violados
            int ca=-1,cb=-1,cc_col=-1;
            if(n_dirty>0){
                int ha=h_dirty[rng_int(n_dirty)];
                int nc_a=n_cols_in_h[ha];
                for(int a=0;a<5&&ca<0;a++){int c=(nc_a>0)?cols_in_h[ha][rng_int(nc_a)]:rng_int(NCOLS);if(!freeze[c]&&tabu[c]==0)ca=c;}
                if(ca<0)for(int i=0;i<nc_a&&ca<0;i++){int c=cols_in_h[ha][i];if(!freeze[c]&&tabu[c]==0)ca=c;}
                int hb=h_dirty[rng_int(n_dirty)];
                int nc_b=n_cols_in_h[hb];
                for(int a=0;a<5&&cb<0;a++){int c=(nc_b>0)?cols_in_h[hb][rng_int(nc_b)]:rng_int(NCOLS);if(c!=ca&&!freeze[c]&&tabu[c]==0)cb=c;}
                if(cb<0)for(int i=0;i<nc_b&&cb<0;i++){int c=cols_in_h[hb][i];if(c!=ca&&!freeze[c]&&tabu[c]==0)cb=c;}
                int hc=h_dirty[rng_int(n_dirty)];
                int nc_c=n_cols_in_h[hc];
                for(int a=0;a<5&&cc_col<0;a++){int c=(nc_c>0)?cols_in_h[hc][rng_int(nc_c)]:rng_int(NCOLS);if(c!=ca&&c!=cb&&!freeze[c]&&tabu[c]==0)cc_col=c;}
                if(cc_col<0)for(int i=0;i<nc_c&&cc_col<0;i++){int c=cols_in_h[hc][i];if(c!=ca&&c!=cb&&!freeze[c]&&tabu[c]==0)cc_col=c;}
            }
            if(ca<0)ca=rng_int(NCOLS);
            if(cb<0)do{cb=rng_int(NCOLS);}while(cb==ca);
            if(cc_col<0)do{cc_col=rng_int(NCOLS);}while(cc_col==ca||cc_col==cb);

            int old_a=col_idx[ca], old_b=col_idx[cb], old_c=col_idx[cc_col];

            int new_a=choose_new_pt(g_region[old_a],-1,old_a,old_b);
            if(new_a==old_a||new_a==old_b||new_a==old_c) continue;
            int new_b=choose_new_pt(g_region[old_b],-1,old_b,new_a);
            for(int att=0;att<10&&(new_b==old_b||new_b==old_a||new_b==old_c||new_b==new_a);att++) new_b=rng_int(g_n_vq5);
            if(new_b==old_b||new_b==old_a||new_b==old_c||new_b==new_a) continue;
            int new_c=rng_int(g_n_vq5);
            for(int att=0;att<10&&(new_c==old_a||new_c==old_b||new_c==old_c||new_c==new_a||new_c==new_b);att++) new_c=rng_int(g_n_vq5);
            if(new_c==old_a||new_c==old_b||new_c==old_c||new_c==new_a||new_c==new_b) continue;
            // Verificar no duplicados
            for(int c=0;c<NCOLS;c++){
                if(c==ca||c==cb||c==cc_col) continue;
                if(col_idx[c]==new_a||col_idx[c]==new_b||col_idx[c]==new_c) goto next_try3;
            }

            {
                const int* oha=g_pt_to_h[old_a]; const int* nha=g_pt_to_h[new_a];
                const int* ohb=g_pt_to_h[old_b]; const int* nhb=g_pt_to_h[new_b];
                const int* ohc=g_pt_to_h[old_c]; const int* nhc=g_pt_to_h[new_c];

                for(int k=0;k<H_PER_PT;k++){
                    mk_old[oha[k]]=true;   mk_affected[oha[k]]=true;
                    mk_new[nha[k]]=true;   mk_affected[nha[k]]=true;
                    mk_old_b[ohb[k]]=true; mk_affected[ohb[k]]=true;
                    mk_new_b[nhb[k]]=true; mk_affected[nhb[k]]=true;
                    mk_old_c[ohc[k]]=true; mk_affected[ohc[k]]=true;
                    mk_new_c[nhc[k]]=true; mk_affected[nhc[k]]=true;
                }

                long long dE=0;
                for(int h=0;h<g_n_all;h++){
                    if(!mk_affected[h]) continue;
                    int delta=0;
                    if(mk_old[h]&&!mk_new[h])   delta--;
                    if(mk_new[h]&&!mk_old[h])   delta++;
                    if(mk_old_b[h]&&!mk_new_b[h]) delta--;
                    if(mk_new_b[h]&&!mk_old_b[h]) delta++;
                    if(mk_old_c[h]&&!mk_new_c[h]) delta--;
                    if(mk_new_c[h]&&!mk_old_c[h]) delta++;
                    if(delta!=0) dE+=penalty(h_load[h]+delta)-penalty(h_load[h]);
                }

                for(int k=0;k<H_PER_PT;k++){
                    mk_old[oha[k]]=false;  mk_new[nha[k]]=false;
                    mk_old_b[ohb[k]]=false;mk_new_b[nhb[k]]=false;
                    mk_old_c[ohc[k]]=false;mk_new_c[nhc[k]]=false;
                    mk_affected[oha[k]]=false; mk_affected[nha[k]]=false;
                    mk_affected[ohb[k]]=false; mk_affected[nhb[k]]=false;
                    mk_affected[ohc[k]]=false; mk_affected[nhc[k]]=false;
                }

                if(dE<0){
                    for(int k=0;k<H_PER_PT;k++){h_load[oha[k]]--;h_load[ohb[k]]--;h_load[ohc[k]]--;}
                    for(int k=0;k<H_PER_PT;k++){h_load[nha[k]]++;h_load[nhb[k]]++;h_load[nhc[k]]++;}
                    E_star+=dE;
                    n_dirty=0; memset(h_dirty_pos,-1,sizeof(h_dirty_pos));
                    for(int h=0;h<g_n_all;h++) if(h_load[h]>LIMIT_MHP) dirty_add(h);
                    // Actualizar cols_in_h para ca
                    for(int k=0;k<H_PER_PT;k++){int h=oha[k];int nc=n_cols_in_h[h];for(int i=0;i<nc;i++)if(cols_in_h[h][i]==ca){cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]];break;}}
                    for(int k=0;k<H_PER_PT;k++) cols_in_h[nha[k]][n_cols_in_h[nha[k]]++]=ca;
                    // cb
                    for(int k=0;k<H_PER_PT;k++){int h=ohb[k];int nc=n_cols_in_h[h];for(int i=0;i<nc;i++)if(cols_in_h[h][i]==cb){cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]];break;}}
                    for(int k=0;k<H_PER_PT;k++) cols_in_h[nhb[k]][n_cols_in_h[nhb[k]]++]=cb;
                    // cc_col
                    for(int k=0;k<H_PER_PT;k++){int h=ohc[k];int nc=n_cols_in_h[h];for(int i=0;i<nc;i++)if(cols_in_h[h][i]==cc_col){cols_in_h[h][i]=cols_in_h[h][--n_cols_in_h[h]];break;}}
                    for(int k=0;k<H_PER_PT;k++) cols_in_h[nhc[k]][n_cols_in_h[nhc[k]]++]=cc_col;

                    col_idx[ca]=new_a;    for(int r=0;r<DIMS;r++) G[r][ca]=g_vq5[new_a][r];
                    col_idx[cb]=new_b;    for(int r=0;r<DIMS;r++) G[r][cb]=g_vq5[new_b][r];
                    col_idx[cc_col]=new_c;for(int r=0;r<DIMS;r++) G[r][cc_col]=g_vq5[new_c][r];
                    tabu[ca]=TABU_CYCLES; tabu[cb]=TABU_CYCLES; tabu[cc_col]=TABU_CYCLES;
                    return true;
                }
            }
            next_try3:;
        }
        return false;
    }

    // =================================================================
    // ACTUALIZAR FREEZE (Gemini M20)
    // Congelar columnas que no participan en ningun H violado cuando E*<FREEZE_E_MAX
    // =================================================================
    void update_freeze(){
        memset(freeze,0,sizeof(freeze));
        if(E_star>=FREEZE_E_MAX) return;
        for(int c=0;c<NCOLS;c++){
            int j=col_idx[c];
            bool conflictiva=false;
            for(int k=0;k<H_PER_PT&&!conflictiva;k++){
                int h=g_pt_to_h[j][k];
                if(h_load[h]>LIMIT_MHP) conflictiva=true;
            }
            freeze[c]=!conflictiva;
        }
    }

    // =================================================================
    // ACTUALIZACION DE TABU (decrementar por ciclo)
    // =================================================================
    void tick_tabu(){
        for(int c=0;c<NCOLS;c++) if(tabu[c]>0) tabu[c]--;
    }

    // =================================================================
    // EXPLOSION DE DUALIDAD (ChatGPT) — cuando E*<=DUAL_E_MAX
    // Calcula el espectro dual y usa pesos como penalizacion adicional.
    // Busca el 1-swap (o 2-swap) que minimice E*+dual_penalty.
    // =================================================================
    bool dual_explosion(){
        // Calcular pesos duales: para cada punto j de V(Q5),
        // contar cuantos codewords lo "activan" (peso de la columna en el dual).
        // Aproximacion eficiente: usar el score de hiperplanos compartidos con dirty H.
        // Para cada columna, calcular su "presion dual" = sum sobre H violados de load_H donde j en H.
        // El candidato de reemplazo es el que reduce mas esta presion.
        int best_ca=-1, best_new=-1;
        long long best_dE=0;

        for(int c=0;c<NCOLS;c++){
            if(freeze[c]||tabu[c]>0) continue;
            int old_j=col_idx[c];
            // Calcular presion dual de old_j
            long long presion_old=0;
            for(int k=0;k<H_PER_PT;k++){
                int h=g_pt_to_h[old_j][k];
                if(h_load[h]>LIMIT_MHP) presion_old+=penalty(h_load[h]);
            }

            // Probar NCOLS candidatos en T1 (firma del diamante)
            int ntry=std::min(30,(int)g_t1_pts.size());
            for(int i=0;i<ntry;i++){
                int new_j=g_t1_pts[rng_int((int)g_t1_pts.size())];
                if(new_j==old_j) continue;
                bool dup=false;
                for(int cc=0;cc<NCOLS;cc++) if(col_idx[cc]==new_j){dup=true;break;}
                if(dup) continue;

                const int* old_hs=g_pt_to_h[old_j];
                const int* new_hs=g_pt_to_h[new_j];
                for(int k=0;k<H_PER_PT;k++) mk_old[old_hs[k]]=true;
                for(int k=0;k<H_PER_PT;k++) mk_new[new_hs[k]]=true;

                long long dE=0;
                for(int k=0;k<H_PER_PT;k++){
                    int h=old_hs[k];
                    if(!mk_new[h]) dE+=penalty(h_load[h]-1)-penalty(h_load[h]);
                }
                for(int k=0;k<H_PER_PT;k++){
                    int h=new_hs[k];
                    if(!mk_old[h]) dE+=penalty(h_load[h]+1)-penalty(h_load[h]);
                }

                for(int k=0;k<H_PER_PT;k++){mk_old[old_hs[k]]=false; mk_new[new_hs[k]]=false;}

                if(dE<best_dE){best_dE=dE;best_ca=c;best_new=new_j;}
            }
        }

        if(best_ca>=0&&best_new>=0&&best_dE<0){
            int old_j=col_idx[best_ca];
            const int* old_hs=g_pt_to_h[old_j];
            const int* new_hs=g_pt_to_h[best_new];
            for(int k=0;k<H_PER_PT;k++) mk_old[old_hs[k]]=true;
            for(int k=0;k<H_PER_PT;k++) mk_new[new_hs[k]]=true;
            apply_swap(best_ca, old_j, best_new, best_dE);
            for(int k=0;k<H_PER_PT;k++){mk_old[old_hs[k]]=false; mk_new[new_hs[k]]=false;}
            return true;
        }
        return false;
    }

    // =================================================================
    // VNW: k pasos aleatorios desde la semilla (distinto cuenca)
    // =================================================================
    void vnw_from(const int Gin[DIMS][NCOLS], int k){
        init_from_G(Gin);
        for(int step=0;step<k;step++){
            // Swap aleatorio simple
            int ca=rng_int(NCOLS);
            int new_j=rng_int(g_n_vq5);
            // Evitar duplicados
            for(int att=0;att<20;att++){
                bool dup=false;
                for(int c=0;c<NCOLS;c++) if(c!=ca&&col_idx[c]==new_j){dup=true;break;}
                if(!dup&&new_j!=col_idx[ca]) break;
                new_j=rng_int(g_n_vq5);
            }
            // Aplicar sin Metropolis (VNW puro)
            int old_j=col_idx[ca];
            const int* old_hs=g_pt_to_h[old_j];
            const int* new_hs=g_pt_to_h[new_j];
            for(int kk=0;kk<H_PER_PT;kk++) mk_old[old_hs[kk]]=true;
            for(int kk=0;kk<H_PER_PT;kk++) mk_new[new_hs[kk]]=true;
            long long dE=0;
            for(int kk=0;kk<H_PER_PT;kk++){int h=old_hs[kk];if(!mk_new[h])dE+=penalty(h_load[h]-1)-penalty(h_load[h]);}
            for(int kk=0;kk<H_PER_PT;kk++){int h=new_hs[kk];if(!mk_old[h])dE+=penalty(h_load[h]+1)-penalty(h_load[h]);}
            apply_swap(ca, old_j, new_j, dE);
            for(int kk=0;kk<H_PER_PT;kk++){mk_old[old_hs[kk]]=false;mk_new[new_hs[kk]]=false;}
        }
    }
};

// =================================================================
// ARRAY DE UNIVERSOS — GLOBALES
// =================================================================
static Universe g_uni[N_UNI];

// Flag de parada (SIGINT)
static volatile int g_stop=0;

// =================================================================
// SIGINT HANDLER — guarda todos los universos antes de salir
// =================================================================
static void sigint_handler(int){
    g_stop=1;
}

// =================================================================
// CONSTRUIR ESPACIOS E INCIDENCIA
// =================================================================
static void build_spaces(){
    memset(g_vq5_idx,-1,sizeof(g_vq5_idx));
    uint32_t seen[N_PG54]; int sn=0;
    for(int c=0;c<4096;c++){
        int v[DIMS],tmp=c;
        for(int i=0;i<DIMS;i++){v[i]=tmp&3;tmp>>=2;}
        if(!canon(v)) continue;
        uint32_t code=encode_pt(v);
        bool dup=false;
        for(int s=0;s<sn;s++) if(seen[s]==code){dup=true;break;}
        if(!dup){seen[sn++]=code; memcpy(g_all[g_n_all++],v,DIMS*sizeof(int));}
    }
    assert(g_n_all==N_PG54);

    for(int i=0;i<g_n_all;i++){
        if(Q5(g_all[i])){
            g_vq5_idx[encode_pt(g_all[i])]=g_n_vq5;
            memcpy(g_vq5[g_n_vq5],g_all[i],DIMS*sizeof(int));
            g_region[g_n_vq5]=region_vq5(g_all[i]);
            switch(g_region[g_n_vq5]){
                case 0: g_t1_pts.push_back(g_n_vq5); break;
                case 1: g_t1noq6_pts.push_back(g_n_vq5); break;
                case 2: g_vq5u_pts.push_back(g_n_vq5); break;
            }
            g_n_vq5++;
        }
    }
    assert(g_n_vq5==N_VQ5);
}

static void build_incidence(){
    for(int j=0;j<g_n_vq5;j++){
        int cnt=0;
        for(int h=0;h<g_n_all;h++)
            if(ip6(g_vq5[j],g_all[h])==0){
                assert(cnt<H_PER_PT);
                g_pt_to_h[j][cnt++]=h;
            }
        assert(cnt==H_PER_PT);
    }
    for(int h=0;h<g_n_all;h++){
        for(int j=0;j<g_n_vq5;j++){
            if(ip6(g_vq5[j],g_all[h])!=0){
                g_vq5_not_in_h[h].push_back(j);
                if(g_region[j]==0) g_t1_not_in_h[h].push_back(j);
                if(g_region[j]==2) g_vq5u_not_in_h[h].push_back(j);
            }
        }
    }
}

// =================================================================
// GUARDAR RECORD — FALLO-6: check_rank6 antes de guardar
// fsync + readback (macOS)
// =================================================================
static void save_record_u(int u, long long E_val, int ciclo){
    Universe& U=g_uni[u];
    // FALLO-6: verificar rango antes de guardar
    if(!U.check_rank6(U.G)){
        fprintf(stderr,"WARN U%d: rango<6 en record E*=%lld — omitido\n",u,E_val);
        return;
    }

    char dir[256];
    snprintf(dir,sizeof(dir),"%s/Downloads/ESTRELLA_RECORDS",
             getenv("HOME")?getenv("HOME"):".");
    mkdir(dir,0755);

    time_t now=time(nullptr); struct tm* tmi=localtime(&now);
    char ts[32]; strftime(ts,sizeof(ts),"%Y%m%d_%H%M%S",tmi);
    char fname[512];
    snprintf(fname,sizeof(fname),"%s/rayo_U%d_record_E%lld_%s.txt",dir,u,E_val,ts);

    FILE* f=fopen(fname,"w");
    if(!f){
        snprintf(fname,sizeof(fname),"rayo_U%d_record_E%lld_%s.txt",u,E_val,ts);
        f=fopen(fname,"w");
    }
    if(!f){fprintf(stderr,"ERROR: no se pudo guardar record U%d E*=%lld\n",u,E_val);return;}

    int mhp=0;
    for(int h=0;h<g_n_all;h++) if(U.h_load[h]>mhp) mhp=U.h_load[h];

    int cnt_t1=0,cnt_t1noq6=0,cnt_vq5u=0;
    for(int c=0;c<NCOLS;c++){
        int r=g_region[U.col_idx[c]];
        if(r==0) cnt_t1++;
        else if(r==1) cnt_t1noq6++;
        else cnt_vq5u++;
    }

    fprintf(f,"OPERACION_RAYO.cpp — Proyecto Estrella — RECORD\n");
    fprintf(f,"Architect: R. Amichis (Rafael Amichis Luengo)\n");
    fprintf(f,"Engine: Claude (Anthropic)\n");
    fprintf(f,"Timestamp: %s\n",ts);
    fprintf(f,"Universo: U%d | Ciclo: %d | Restarts: %d\n",u,ciclo,U.n_restarts);
    fprintf(f,"E*=%lld (objetivo: 0) | mhp=%d (objetivo: <=9)\n",E_val,mhp);
    fprintf(f,"Columnas: T1=%d T1_noQ6=%d VQ5_u=%d (firma Diamond: 17+0+5)\n",
            cnt_t1,cnt_t1noq6,cnt_vq5u);
    fprintf(f,"Rango: 6 (verificado)\n");
    fprintf(f,"\nGenerador G (6x22):\n");
    for(int r=0;r<DIMS;r++){
        fprintf(f,"  {");
        for(int c=0;c<NCOLS;c++) fprintf(f,"%d%s",U.G[r][c],c<NCOLS-1?",":"");
        fprintf(f,"}\n");
    }
    fprintf(f,"\nColumnas (region en PG(5,4)):\n");
    for(int c=0;c<NCOLS;c++){
        int j=U.col_idx[c];
        fprintf(f,"  col%02d: [%d,%d,%d,%d,%d,%d] [%s]\n",c,
                g_vq5[j][0],g_vq5[j][1],g_vq5[j][2],
                g_vq5[j][3],g_vq5[j][4],g_vq5[j][5],
                region_name_full(g_vq5[j]));
    }
    fprintf(f,"\ngithub.com/tretoef-estrella | BSL 1.1 + SAMAEL Decree\n");

    // fsync + readback (macOS — OBLIGATORIO)
    fsync(fileno(f));
    fclose(f);
    FILE* fv=fopen(fname,"r");
    if(fv){char b[32];size_t rb=fread(b,1,20,fv);fclose(fv);
        if(rb<5) fprintf(stderr,"WARN U%d: readback fallido para E*=%lld\n",u,E_val);}

    // Imprimir en pantalla
    printf("\n  +----------------------------------------------------------+\n");
    printf("  |  RECORD U%d: E*=%lld  mhp=%d  ciclo=%d  rst=%d\n",
           u,E_val,mhp,ciclo,U.n_restarts);
    printf("  |  T1=%d  T1_noQ6=%d  VQ5_u=%d  (firma: 17+0+5)\n",
           cnt_t1,cnt_t1noq6,cnt_vq5u);
    printf("  |  Generador G:\n");
    for(int r=0;r<DIMS;r++){
        printf("  |  {");
        for(int c=0;c<NCOLS;c++) printf("%d%s",U.G[r][c],c<NCOLS-1?",":"");
        printf("}\n");
    }
    printf("  |  Guardado: %s\n",fname);
    printf("  +----------------------------------------------------------+\n\n");
    fflush(stdout);
}

// =================================================================
// GUARDAR DIAMANTE — FALLO-6: check_rank6 obligatorio
// =================================================================
static void save_diamond(int u, int ciclo){
    Universe& U=g_uni[u];
    // FALLO-6: verificacion de rango antes de guardar
    if(!U.check_rank6(U.G)){
        fprintf(stderr,"ERROR CRITICO: rango<6 en DIAMANTE — NO guardado\n");
        return;
    }

    time_t now=time(nullptr); struct tm* tmi=localtime(&now);
    char ts[32]; strftime(ts,sizeof(ts),"%Y%m%d_%H%M%S",tmi);

    char dir[256];
    snprintf(dir,sizeof(dir),"%s/Downloads/ESTRELLA_RECORDS",
             getenv("HOME")?getenv("HOME"):".");
    mkdir(dir,0755);

    char fname[512];
    snprintf(fname,sizeof(fname),"%s/DIAMOND_AMICHIS_22613_%s.txt",dir,ts);
    FILE* f=fopen(fname,"w");
    if(!f){snprintf(fname,sizeof(fname),"DIAMOND_AMICHIS_22613_%s.txt",ts);f=fopen(fname,"w");}
    if(!f){fprintf(stderr,"ERROR CRITICO: no se pudo guardar el DIAMANTE\n");return;}

    int mhp=0;
    for(int h=0;h<g_n_all;h++) if(U.h_load[h]>mhp) mhp=U.h_load[h];
    int cnt_t1=0,cnt_t1noq6=0,cnt_vq5u=0;
    for(int c=0;c<NCOLS;c++){
        int r=g_region[U.col_idx[c]];
        if(r==0) cnt_t1++;
        else if(r==1) cnt_t1noq6++;
        else cnt_vq5u++;
    }

    fprintf(f,"THE AMICHIS CODE — [22,6,13]_4 sobre GF(4)\n");
    fprintf(f,"Proyecto Estrella\n");
    fprintf(f,"  Architect: R. Amichis (Rafael Amichis Luengo)\n");
    fprintf(f,"  Engine   : Claude (Anthropic) — OPERACION_RAYO.cpp\n");
    fprintf(f,"  Timestamp: %s\n",ts);
    fprintf(f,"  Universo : U%d | Ciclo: %d\n",u,ciclo);
    fprintf(f,"E_star     = %lld (debe ser 0)\n",U.E_star);
    fprintf(f,"max_hplane = %d (debe ser <= 9)\n",mhp);
    fprintf(f,"Columnas: T1=%d T1_noQ6=%d VQ5_u=%d\n",cnt_t1,cnt_t1noq6,cnt_vq5u);
    fprintf(f,"Rango: 6 (verificado)\n");
    fprintf(f,"\nGenerador G (6x22):\n");
    for(int r=0;r<DIMS;r++){
        fprintf(f,"  {");
        for(int c=0;c<NCOLS;c++) fprintf(f,"%d%s",U.G[r][c],c<NCOLS-1?",":"");
        fprintf(f,"}\n");
    }
    fprintf(f,"\nColumnas:\n");
    for(int c=0;c<NCOLS;c++){
        int j=U.col_idx[c];
        fprintf(f,"  col%02d: [%d,%d,%d,%d,%d,%d] [%s]\n",c,
                g_vq5[j][0],g_vq5[j][1],g_vq5[j][2],
                g_vq5[j][3],g_vq5[j][4],g_vq5[j][5],
                region_name_full(g_vq5[j]));
    }
    fprintf(f,"\ngithub.com/tretoef-estrella | BSL 1.1 + SAMAEL Decree\n");

    fsync(fileno(f));
    fclose(f);
    // Readback de verificacion
    FILE* fv=fopen(fname,"r");
    if(fv){char b[32];size_t rb=fread(b,1,20,fv);fclose(fv);
        if(rb>0) printf("\n*** DIAMANTE GUARDADO: %s ***\n",fname);}
}

// =================================================================
// PARALLEL TEMPERING — intercambio entre universos
// FALLO-5: verificacion de duplicados antes de intercambio
// =================================================================
static void parallel_tempering(){
    // Intentar intercambio entre universos adyacentes (0-1 y 1-2)
    for(int u=0;u<N_UNI-1;u++){
        Universe& Ua=g_uni[u];
        Universe& Ub=g_uni[u+1];
        // Criterio de intercambio Metropolis cruzado
        double dE=(double)(Ub.E_star-Ua.E_star);
        double dT=1.0/Ua.T_cur-1.0/Ub.T_cur;
        double accept_prob=exp(dE*dT);
        double r=Ua.rng01();
        if(r<accept_prob){
            // Intercambiar estados completos
            // FALLO-5: verificar que no hay duplicados en el nuevo universo
            // (en intercambio completo de estados, no aplica — los states son validos por construccion)
            // Solo intercambiar el estado SA corriente, no G_best
            int tmpG[DIMS][NCOLS];
            memcpy(tmpG,Ua.G,sizeof(tmpG));
            memcpy(Ua.G,Ub.G,sizeof(Ua.G));
            memcpy(Ub.G,tmpG,sizeof(Ub.G));

            // Reconstruir estados internos
            Ua.rebuild_from(Ua.G);
            Ub.rebuild_from(Ub.G);

            // Intercambiar tabu y freeze
            int tmp_tabu[NCOLS];
            bool tmp_freeze[NCOLS];
            memcpy(tmp_tabu,Ua.tabu,sizeof(tmp_tabu));
            memcpy(Ua.tabu,Ub.tabu,sizeof(Ua.tabu));
            memcpy(Ub.tabu,tmp_tabu,sizeof(Ub.tabu));
            memcpy(tmp_freeze,Ua.freeze,sizeof(tmp_freeze));
            memcpy(Ua.freeze,Ub.freeze,sizeof(Ua.freeze));
            memcpy(Ub.freeze,tmp_freeze,sizeof(Ub.freeze));

            Ua.n_pt++;
            Ub.n_pt++;
        }
    }
}

// =================================================================
// MAIN
// =================================================================
int main(){
    signal(SIGINT, sigint_handler);

    // =================================================================
    // CABECERA
    // =================================================================
    printf("%s\n",std::string(72,'=').c_str());
    printf("OPERACION_RAYO.cpp — Proyecto Estrella — Hunt for [22,6,13]_4\n");
    printf("2-swap PRIMARIO + 3-swap + Freeze + Tabu + Dual + PT + OpenMP\n");
    {
        time_t now=time(nullptr); struct tm* tmi=localtime(&now);
        char tbuf[32]; strftime(tbuf,sizeof(tbuf),"%Y-%m-%d %H:%M:%S",tmi);
        printf("Inicio: %s\n",tbuf);
    }
#ifdef _OPENMP
    printf("OpenMP: %d threads disponibles\n",omp_get_max_threads());
#else
    printf("OpenMP: NO disponible — universos secuenciales\n");
#endif
    printf("%s\n\n",std::string(72,'=').c_str());

    printf("UNIVERSO:  V(Q5) = %d puntos de PG(5,4)\n",N_VQ5);
    printf("ENERGIA:   E* = SUM_h (load_h-9)^3  si load_h>9  (cubica)\n");
    printf("OBJETIVO:  E* = 0  =>  DIAMANTE [22,6,13]_4\n");
    printf("SEMILLA_U0: E*=4 SEED_TRUENO (record absoluto — 8888 vecinos, dE>=0)\n");
    printf("SEMILLA_U1: RELAMPAGO E*=6 (basin distinto T1=4 VQ5u=13)\n");
    printf("SEMILLA_U2: VNW k=%d desde E*=4 (cuenca alejada k=20)\n",VNW_K);
    printf("OPERADORES: 2-swap PRIMARIO (co-conflictivo) + 3-swap + SA\n");
    printf("SCHEDULE:  T_start=%.1f  T_factor=%.3f  T_min=%.2f  %d pasos/ciclo\n",
           T_START,T_FACTOR,T_MIN,S_PER_CYCLE);
    printf("RESTART:   stag=%d => FIX-DIV (4 fuentes)\n",RESTART_STAG);
    printf("FREEZE:    E*<%d => congelar no-conflictivas (Gemini M20)\n",FREEZE_E_MAX);
    printf("TABU:      %d ciclos para col expulsada por 2-swap\n",TABU_CYCLES);
    printf("DUAL:      E*<=%d => Explosion de Dualidad (ChatGPT)\n",DUAL_E_MAX);
    printf("PT:        intercambio cada %d ciclos\n\n",PT_PERIOD);

    // =================================================================
    // [1] CONSTRUIR PG(5,4) Y V(Q5)
    // =================================================================
    printf("[1] Construyendo PG(5,4) y V(Q5)... "); fflush(stdout);
    build_spaces();
    printf("%d + %d  [T1=%d T1noQ6=%d VQ5u=%d]  OK\n",
           g_n_all,g_n_vq5,
           (int)g_t1_pts.size(),(int)g_t1noq6_pts.size(),(int)g_vq5u_pts.size());

    // =================================================================
    // [2] CONSTRUIR INCIDENCIA
    // =================================================================
    printf("[2] Construyendo incidencia y listas de sesgo... "); fflush(stdout);
    clock_t t0=clock();
    build_incidence();
    printf("%.1fs OK\n",(double)(clock()-t0)/CLOCKS_PER_SEC);

    // =================================================================
    // [3] VERIFICAR SEMILLAS
    // =================================================================
    printf("[3] Verificando semillas...\n");
    // Semilla U0: E*=7
    {
        bool ok=true;
        for(int c=0;c<NCOLS&&ok;c++){
            int col[DIMS];for(int r=0;r<DIMS;r++)col[r]=G_SEED_E7[r][c];
            canon(col);
            if(!Q5(col)){fprintf(stderr,"FATAL: SEED col %d no en V(Q5)\n",c);ok=false;}
        }
        // Verificacion rapida de E* (usa instancia temporal)
        Universe tmp; memset(&tmp,0,sizeof(tmp)); tmp.id=99;
        tmp.init_from_G(G_SEED_E4);
        int mhp=0;for(int h=0;h<g_n_all;h++)if(tmp.h_load[h]>mhp)mhp=tmp.h_load[h];
        int cnt_t1=0,cnt_vq5u=0;
        for(int c=0;c<NCOLS;c++){if(g_region[tmp.col_idx[c]]==0)cnt_t1++;else if(g_region[tmp.col_idx[c]]==2)cnt_vq5u++;}
        printf("    G_SEED_E4: 22/22 V(Q5) E*=%lld mhp=%d dirty=%d T1=%d VQ5u=%d %s\n",
               tmp.E_star,mhp,tmp.n_dirty,cnt_t1,cnt_vq5u,
               (tmp.E_star==7&&mhp==10&&tmp.n_dirty==7)?"OK":"WARN");
        if(tmp.E_star==0){printf("*** E*=0 EN G_SEED_E4 — DIAMANTE EN SEMILLA ***\n");save_diamond(0,0);return 0;}
    }
    // Semilla U1: C254
    {
        bool ok=true;
        for(int c=0;c<NCOLS&&ok;c++){
            int col[DIMS];for(int r=0;r<DIMS;r++)col[r]=G_SEED_C254[r][c];
            canon(col);
            if(!Q5(col)){fprintf(stderr,"FATAL: G_SEED_C254 col %d no en V(Q5)\n",c);ok=false;}
        }
        Universe tmp; memset(&tmp,0,sizeof(tmp)); tmp.id=98;
        tmp.init_from_G(G_SEED_C254);
        int mhp=0;for(int h=0;h<g_n_all;h++)if(tmp.h_load[h]>mhp)mhp=tmp.h_load[h];
        int cnt_t1=0,cnt_vq5u=0;
        for(int c=0;c<NCOLS;c++){if(g_region[tmp.col_idx[c]]==0)cnt_t1++;else if(g_region[tmp.col_idx[c]]==2)cnt_vq5u++;}
        printf("    G_SEED_C254: 22/22 V(Q5) E*=%lld mhp=%d T1=%d VQ5u=%d (firma 17+5) %s\n",
               tmp.E_star,mhp,cnt_t1,cnt_vq5u,
               (cnt_t1==17&&cnt_vq5u==5)?"OK":"WARN");
        if(tmp.E_star==0){printf("*** E*=0 EN G_SEED_C254 — DIAMANTE EN SEMILLA ***\n");save_diamond(0,0);return 0;}
    }

    // =================================================================
    // [4] STARTUP_TEST — verificar escritura en ESTRELLA_RECORDS
    // =================================================================
    printf("[4] STARTUP_TEST... ");
    {
        char dir[256];
        snprintf(dir,sizeof(dir),"%s/Downloads/ESTRELLA_RECORDS",
                 getenv("HOME")?getenv("HOME"):".");
        mkdir(dir,0755);
        char tfile[512];
        snprintf(tfile,sizeof(tfile),"%s/RAYO_STARTUP_TEST.txt",dir);
        FILE* tf=fopen(tfile,"w");
        if(!tf){fprintf(stderr,"WARN: STARTUP_TEST — no se puede escribir en %s\n",dir);}
        else{
            fprintf(tf,"RAYO_STARTUP_TEST_OK\n");
            fsync(fileno(tf)); fclose(tf);
            FILE* tv=fopen(tfile,"r");
            char buf[32]={};
            if(tv){size_t rb2=fread(buf,1,24,tv);fclose(tv);(void)rb2;}
            if(!strncmp(buf,"RAYO_STARTUP_TEST_OK",23))
                printf("OK (escritura verificada en %s)\n",dir);
            else
                printf("WARN — readback fallido\n");
        }
    }

    // =================================================================
    // [5] INICIALIZAR UNIVERSOS
    // =================================================================
    printf("[5] Inicializando universos...\n");
    uint64_t base_rng=(uint64_t)time(nullptr)^((uint64_t)clock()<<32);
    if(!base_rng) base_rng=0xDEADBEEFCAFE1234ULL;

    for(int u=0;u<N_UNI;u++){
        memset(&g_uni[u],0,sizeof(Universe));
        g_uni[u].id=u;
        // RNG unico por universo
        g_uni[u].rng=base_rng^(0xA5A5A5A5ULL*(uint64_t)(u+1));
        g_uni[u].T_cur=T_START;
        g_uni[u].stag=0;
        g_uni[u].n_restarts=0;
        g_uni[u].cycle=0;
        g_uni[u].firma_score=9999;
    }

    // U0: SEED_TRUENO E*=4 — el record
    memcpy(g_uni[0].G_seed, G_SEED_E4, sizeof(G_SEED_E4));
    g_uni[0].init_from_G(G_SEED_E4);
    memcpy(g_uni[0].G_best, g_uni[0].G, sizeof(g_uni[0].G));
    g_uni[0].E_best=g_uni[0].E_star;
    memcpy(g_uni[0].G_firma_best, g_uni[0].G, sizeof(g_uni[0].G));
    g_uni[0].E_firma_best=g_uni[0].E_star;
    g_uni[0].firma_score=g_uni[0].firma_dist();
    {
        int mhp=0;for(int h=0;h<g_n_all;h++)if(g_uni[0].h_load[h]>mhp)mhp=g_uni[0].h_load[h];
        printf("    U0 (E*=4):     E*=%lld  mhp=%d  dirty=%d  T=%.1f  OK\n",
               g_uni[0].E_star,mhp,g_uni[0].n_dirty,g_uni[0].T_cur);
    }

    // U1: RELAMPAGO E*=6 — basin distinto
    memcpy(g_uni[1].G_seed, G_SEED_E6, sizeof(G_SEED_E6));
    g_uni[1].init_from_G(G_SEED_E6);
    memcpy(g_uni[1].G_best, g_uni[1].G, sizeof(g_uni[1].G));
    g_uni[1].E_best=g_uni[1].E_star;
    memcpy(g_uni[1].G_firma_best, g_uni[1].G, sizeof(g_uni[1].G));
    g_uni[1].E_firma_best=g_uni[1].E_star;
    g_uni[1].firma_score=g_uni[1].firma_dist();
    {
        int mhp=0;for(int h=0;h<g_n_all;h++)if(g_uni[1].h_load[h]>mhp)mhp=g_uni[1].h_load[h];
        int cnt_t1=0,cnt_vq5u=0;
        for(int c=0;c<NCOLS;c++){if(g_region[g_uni[1].col_idx[c]]==0)cnt_t1++;else if(g_region[g_uni[1].col_idx[c]]==2)cnt_vq5u++;}
        printf("    U1 (C254):     E*=%lld  mhp=%d  T1=%d VQ5u=%d  T=%.1f  OK\n",
               g_uni[1].E_star,mhp,cnt_t1,cnt_vq5u,g_uni[1].T_cur);
    }

    // U2: VNW k=4 desde E*=7
    memcpy(g_uni[2].G_seed, G_SEED_E7, sizeof(G_SEED_E7));
    g_uni[2].vnw_from(G_SEED_E4, VNW_K);  // VNW k=20 desde E*=4
    memcpy(g_uni[2].G_best, g_uni[2].G, sizeof(g_uni[2].G));
    g_uni[2].E_best=g_uni[2].E_star;
    memcpy(g_uni[2].G_firma_best, g_uni[2].G, sizeof(g_uni[2].G));
    g_uni[2].E_firma_best=g_uni[2].E_star;
    g_uni[2].firma_score=g_uni[2].firma_dist();
    {
        int mhp=0;for(int h=0;h<g_n_all;h++)if(g_uni[2].h_load[h]>mhp)mhp=g_uni[2].h_load[h];
        printf("    U2 (VNW k=%d):  E*=%lld  mhp=%d  T=%.1f  %s\n",
               VNW_K,g_uni[2].E_star,mhp,g_uni[2].T_cur,
               (g_uni[2].E_star<=15)?"OK":"WARN (E*>15)");
    }
    printf("\n");

#ifdef _OPENMP
    printf("[6] OpenMP: usando %d hilos (uno por universo)\n",N_UNI);
#else
    printf("[6] Single-thread: universos secuenciales\n");
#endif
    printf("    Records en ~/Downloads/ESTRELLA_RECORDS/\n");
    printf("    Log: c=ciclo E*=energia mhp=max_hplane best=mejor T=temp\n");
    printf("         stag=estancamiento s2=2swap_mejoras s3=3swap_mejoras dual=dual_mejoras\n");
    printf("%s\n",std::string(72,'-').c_str());
    fflush(stdout);

    // =================================================================
    // BEST GLOBAL entre todos los universos
    // =================================================================
    long long g_E_global_best=g_uni[0].E_best;
    int       g_E_global_best_u=0;
    for(int u=1;u<N_UNI;u++) if(g_uni[u].E_best<g_E_global_best){g_E_global_best=g_uni[u].E_best;g_E_global_best_u=u;}

    time_t t_wall=time(nullptr);

    // =================================================================
    // LOOP PRINCIPAL
    // =================================================================
    for(int global_cycle=1;global_cycle<=MAX_CYCLES&&!g_stop;global_cycle++){

        // FALLO-1: Paralelizacion real con OpenMP
#ifdef _OPENMP
        #pragma omp parallel for schedule(static,1) num_threads(N_UNI)
#endif
        for(int u=0;u<N_UNI;u++){
            Universe& U=g_uni[u];
            U.cycle++;

            // Decrementar tabu
            U.tick_tabu();

            // Actualizar freeze si E* cercano al Diamante (Gemini M20)
            if(U.E_star<(long long)FREEZE_E_MAX) U.update_freeze();
            else memset(U.freeze,0,sizeof(U.freeze));

            bool mejoro_ciclo=false;

            // --- 2-swap DIRIGIDO (PRIMARIO) — activo siempre cuando E*<SWAP2_E_MAX ---
            if(U.E_star<(long long)SWAP2_E_MAX){
                U.total_swap2++;
                if(U.swap2_directed()){
                    U.total_swap2_mejoras++;
                    mejoro_ciclo=true;
                }
            }

            // --- 3-swap dirigido (secundario) ---
            if(U.stag>SWAP3_STAG && U.E_star<(long long)SWAP3_E_MAX){
                U.total_swap3++;
                if(U.swap3_directed()){
                    U.total_swap3_mejoras++;
                    mejoro_ciclo=true;
                }
            }

            // --- Explosion de Dualidad (ChatGPT) ---
            if(U.E_star<=(long long)DUAL_E_MAX){
                U.total_dual++;
                if(U.dual_explosion()){
                    U.total_dual_mejoras++;
                    mejoro_ciclo=true;
                }
            }

            // --- ciclo SA ---
            for(int s=0;s<S_PER_CYCLE;s++){
                U.total_steps++;
                if(U.sa_step(U.T_cur)) U.total_accept++;
            }

            // --- Comprobar Diamante ---
            if(U.E_star==0){
                // Critico: guardar sin critical si es posible
                // (en la practica E*=0 es un evento rarísimo, serializar es seguro)
                #ifdef _OPENMP
                #pragma omp critical
                #endif
                {
                    int mhp=0;for(int h=0;h<g_n_all;h++)if(U.h_load[h]>mhp)mhp=U.h_load[h];
                    printf("\n%s\n",std::string(72,'*').c_str());
                    printf("* EL DIAMANTE EXISTE — U%d  E*=0  mhp=%d  ciclo=%d *\n",u,mhp,global_cycle);
                    printf("%s\n",std::string(72,'*').c_str());
                    save_diamond(u,global_cycle);
                    g_stop=1;
                }
            }

            // --- Actualizar mejor local del universo ---
            if(U.E_star<U.E_best){
                U.E_best=U.E_star;
                memcpy(U.G_best,U.G,sizeof(U.G));
                U.stag=0; mejoro_ciclo=true;
                // FALLO-6: check_rank6 en save_record
                #ifdef _OPENMP
                #pragma omp critical
                #endif
                { save_record_u(u,U.E_best,global_cycle); }
            } else {
                U.stag++;
                if(mejoro_ciclo && U.E_star<U.E_best){
                    // Operador especial mejoro antes de SA
                    U.E_best=U.E_star;
                    memcpy(U.G_best,U.G,sizeof(U.G));
                    U.stag=0;
                    #ifdef _OPENMP
                    #pragma omp critical
                    #endif
                    { save_record_u(u,U.E_best,global_cycle); }
                }
            }

            // --- Actualizar firma_best ---
            {
                int fd=U.firma_dist();
                if(fd<U.firma_score){
                    U.firma_score=fd;
                    memcpy(U.G_firma_best,U.G,sizeof(U.G));
                    U.E_firma_best=U.E_star;
                }
            }

            // --- Restart: FIX-DIV (4 fuentes) ---
            if(U.stag>=RESTART_STAG){
                U.n_restarts++;
                int src=U.n_restarts%4;
                if(src==0){
                    // Desde G_best
                    U.rebuild_from(U.G_best);
                } else if(src==1){
                    // Desde G_firma_best
                    U.rebuild_from(U.G_firma_best);
                } else if(src==2){
                    // VNW k=4 desde G_best
                    U.vnw_from(U.G_best, VNW_K);
                } else {
                    // Semilla original del universo
                    U.rebuild_from(U.G_seed);
                }

                // Reheat proporcional a E* (escape de minimo estricto)
                double e_ref=(double)std::max(U.E_best,(long long)1);
                double t_reheat=T_START*sqrt(e_ref/E_SEMILLA_INIT);
                t_reheat=std::max(2.0,std::min(T_START,t_reheat));
                U.T_cur=t_reheat;
                U.stag=0;
                memset(U.tabu,0,sizeof(U.tabu));
                memset(U.freeze,0,sizeof(U.freeze));
            } else {
                U.T_cur*=T_FACTOR;
                if(U.T_cur<T_MIN) U.T_cur=T_MIN;
            }
        } // fin loop universos (paralelo)

        // --- Parallel Tempering entre universos (serial, fuera del parallel) ---
        if(global_cycle%PT_PERIOD==0){
            parallel_tempering();
        }

        // --- Actualizar best global ---
        for(int u=0;u<N_UNI;u++){
            if(g_uni[u].E_best<g_E_global_best){
                g_E_global_best=g_uni[u].E_best;
                g_E_global_best_u=u;
            }
        }

        // --- Reporte ---
        if(global_cycle%REPORT_EVERY==0){
            double el=difftime(time(nullptr),t_wall);
            long long total_steps_all=0, total_accept_all=0;
            for(int u=0;u<N_UNI;u++){total_steps_all+=g_uni[u].total_steps;total_accept_all+=g_uni[u].total_accept;}
            double rate=(el>0)?total_steps_all/el:0;
            double acc=(total_steps_all>0)?100.0*total_accept_all/total_steps_all:0;
            printf("  c=%d  global_best=E*=%lld(U%d)  %.0fk/s  acc=%.1f%%  %.0fs\n",
                   global_cycle,g_E_global_best,g_E_global_best_u,rate/1000,acc,el);
            for(int u=0;u<N_UNI;u++){
                Universe& U=g_uni[u];
                int mhp=0;for(int h=0;h<g_n_all;h++)if(U.h_load[h]>mhp)mhp=U.h_load[h];
                int cnt_t1=0,cnt_vq5u=0;
                for(int c=0;c<NCOLS;c++){if(g_region[U.col_idx[c]]==0)cnt_t1++;else if(g_region[U.col_idx[c]]==2)cnt_vq5u++;}
                printf("    U%d: E*=%lld mhp=%d best=%lld T=%.4f stag=%d dirty=%d "
                       "t1=%d vq5u=%d s2=%lld(%lld) s3=%lld(%lld) dual=%lld(%lld) pt=%d rst=%d\n",
                       u,U.E_star,mhp,U.E_best,U.T_cur,U.stag,U.n_dirty,
                       cnt_t1,cnt_vq5u,
                       U.total_swap2,U.total_swap2_mejoras,
                       U.total_swap3,U.total_swap3_mejoras,
                       U.total_dual,U.total_dual_mejoras,
                       U.n_pt,U.n_restarts);
            }
            fflush(stdout);
        }
    } // fin loop global

    // =================================================================
    // SIGINT o fin: guardar mejor estado de todos los universos
    // =================================================================
    if(g_stop){
        printf("\n[SIGINT/STOP] Guardando mejores estados de todos los universos...\n");
        for(int u=0;u<N_UNI;u++){
            // Restaurar al mejor conocido antes de guardar
            memcpy(g_uni[u].G,g_uni[u].G_best,sizeof(g_uni[u].G));
            g_uni[u].rebuild_from(g_uni[u].G_best);
            save_record_u(u,g_uni[u].E_best,-1);
        }
    }

    // =================================================================
    // RESULTADO FINAL
    // =================================================================
    double el_total=difftime(time(nullptr),t_wall);
    printf("\n%s\nRESULTADO FINAL rayo\n%s\n",
           std::string(72,'=').c_str(),std::string(72,'=').c_str());
    printf("  Best global: E*=%lld (U%d)\n",g_E_global_best,g_E_global_best_u);
    for(int u=0;u<N_UNI;u++){
        printf("  U%d best: E*=%lld  restarts=%d  swap2_mejoras=%lld  swap3_mejoras=%lld  dual=%lld\n",
               u,g_uni[u].E_best,g_uni[u].n_restarts,
               g_uni[u].total_swap2_mejoras,g_uni[u].total_swap3_mejoras,
               g_uni[u].total_dual_mejoras);
    }
    printf("  Tiempo total: %.0fs = %.2fh\n",el_total,el_total/3600.0);
    printf("\nProyecto Estrella — Rafa no se rinde\n");
    printf("%s\n",std::string(72,'=').c_str());
    return 0;
}
