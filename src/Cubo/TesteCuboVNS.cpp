#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>

#define U 0
#define D 1
#define F 2
#define B 3
#define R 4
#define L 5
#define NUM_MOV         18
#define MAX_ESTAG       80       /* mesmo critério de parada do GA          */
#define FIT_MAX         100.0f
#define MAX_CROM_MULT   2        /* solução pode ter até 2× o embaralhamento */
#define KMAX            5        /* número de estruturas de vizinhança      */
#define LS_MAX_PASSES   5        /* passes máximos da busca local (VND)     */

/* -----------------------------------------------------------------------
 * Pesos da melhor configuração encontrada (artigo SICITE + relatório IC)
 * — idênticos aos usados no GA, para manter a mesma paisagem de fitness
 *   entre os algoritmos e permitir comparação justa de qualidade.
 * ----------------------------------------------------------------------- */
static const int FC[4] = {-1, -5, -9, -6};
static const int FB[4] = {-96, -78, 35, -6};

/* Coordenadas físicas das 12 bordas e seus centros esperados (face, lin, col) */
static const int BORDAS[12][2][3] = {
    {{U,2,1},{F,0,1}}, {{U,1,2},{R,0,1}}, {{U,0,1},{B,0,1}}, {{U,1,0},{L,0,1}},
    {{F,1,2},{R,1,0}}, {{F,1,0},{L,1,2}}, {{B,1,0},{R,1,2}}, {{B,1,2},{L,1,0}},
    {{D,0,1},{F,2,1}}, {{D,1,2},{R,2,1}}, {{D,2,1},{B,2,1}}, {{D,1,0},{L,2,1}}
};
static const int BCOR[12][2] = {
    {U,F},{U,R},{U,B},{U,L},{F,R},{F,L},{B,R},{B,L},{D,F},{D,R},{D,B},{D,L}
};

/* Coordenadas físicas dos 8 cantos e seus centros esperados */
static const int CANTOS[8][3][3] = {
    {{U,2,2},{F,0,2},{R,0,0}}, {{U,2,0},{F,0,0},{L,0,2}},
    {{U,0,2},{B,0,0},{R,0,2}}, {{U,0,0},{B,0,2},{L,0,0}},
    {{D,0,2},{F,2,2},{R,2,0}}, {{D,0,0},{F,2,0},{L,2,2}},
    {{D,2,2},{B,2,0},{R,2,2}}, {{D,2,0},{B,2,2},{L,2,0}}
};
static const int CCOR[8][3] = {
    {U,F,R},{U,F,L},{U,B,R},{U,B,L},{D,F,R},{D,F,L},{D,B,R},{D,B,L}
};

struct Cubo { int face[6][3][3]; };

void cubo_init(Cubo &c) {
    for(int f=0;f<6;f++) for(int i=0;i<3;i++) for(int j=0;j<3;j++) c.face[f][i][j]=f;
}
bool resolvido(const Cubo &c) {
    for(int f=0;f<6;f++) for(int i=0;i<3;i++) for(int j=0;j<3;j++) if(c.face[f][i][j]!=f) return false;
    return true;
}
static void rot(Cubo&c,int f,int cw){int t[3][3];for(int i=0;i<3;i++)for(int j=0;j<3;j++)t[i][j]=c.face[f][i][j];if(cw){for(int i=0;i<3;i++)for(int j=0;j<3;j++)c.face[f][j][2-i]=t[i][j];}else{for(int i=0;i<3;i++)for(int j=0;j<3;j++)c.face[f][2-j][i]=t[i][j];}}
static void mU(Cubo&c){rot(c,U,1);int t[3];for(int j=0;j<3;j++)t[j]=c.face[F][0][j];for(int j=0;j<3;j++)c.face[F][0][j]=c.face[R][0][j];for(int j=0;j<3;j++)c.face[R][0][j]=c.face[B][0][j];for(int j=0;j<3;j++)c.face[B][0][j]=c.face[L][0][j];for(int j=0;j<3;j++)c.face[L][0][j]=t[j];}
static void mD(Cubo&c){rot(c,D,1);int t[3];for(int j=0;j<3;j++)t[j]=c.face[F][2][j];for(int j=0;j<3;j++)c.face[F][2][j]=c.face[L][2][j];for(int j=0;j<3;j++)c.face[L][2][j]=c.face[B][2][j];for(int j=0;j<3;j++)c.face[B][2][j]=c.face[R][2][j];for(int j=0;j<3;j++)c.face[R][2][j]=t[j];}
static void mF(Cubo&c){rot(c,F,1);int t[3];for(int j=0;j<3;j++)t[j]=c.face[U][2][j];for(int j=0;j<3;j++)c.face[U][2][j]=c.face[L][2-j][2];for(int j=0;j<3;j++)c.face[L][j][2]=c.face[D][0][j];for(int j=0;j<3;j++)c.face[D][0][j]=c.face[R][2-j][0];for(int j=0;j<3;j++)c.face[R][j][0]=t[j];}
static void mB(Cubo&c){rot(c,B,1);int t[3];for(int j=0;j<3;j++)t[j]=c.face[U][0][j];for(int j=0;j<3;j++)c.face[U][0][j]=c.face[R][j][2];for(int j=0;j<3;j++)c.face[R][j][2]=c.face[D][2][2-j];for(int j=0;j<3;j++)c.face[D][2][j]=c.face[L][2-j][0];for(int j=0;j<3;j++)c.face[L][j][0]=t[2-j];}
static void mR(Cubo&c){rot(c,R,1);int t[3];for(int i=0;i<3;i++)t[i]=c.face[U][i][2];for(int i=0;i<3;i++)c.face[U][i][2]=c.face[F][i][2];for(int i=0;i<3;i++)c.face[F][i][2]=c.face[D][i][2];for(int i=0;i<3;i++)c.face[D][i][2]=c.face[B][2-i][0];for(int i=0;i<3;i++)c.face[B][2-i][0]=t[i];}
static void mL(Cubo&c){rot(c,L,1);int t[3];for(int i=0;i<3;i++)t[i]=c.face[U][i][0];for(int i=0;i<3;i++)c.face[U][i][0]=c.face[B][2-i][2];for(int i=0;i<3;i++)c.face[B][2-i][2]=c.face[D][i][0];for(int i=0;i<3;i++)c.face[D][i][0]=c.face[F][i][0];for(int i=0;i<3;i++)c.face[F][i][0]=t[i];}

void aplicar_mov(Cubo&c,int m){void(*tab[6])(Cubo&)={mU,mD,mF,mB,mR,mL};int b=m%6,tp=m/6;if(tp==0)tab[b](c);else if(tp==1){tab[b](c);tab[b](c);tab[b](c);}else{tab[b](c);tab[b](c);}}
void aplicar(Cubo&c,const std::vector<int>&v){for(int m:v)aplicar_mov(c,m);}

/* -----------------------------------------------------------------------
 * Estado de borda i  →  0=correto  1=orientado  2=permutado  3=incorreto
 * ----------------------------------------------------------------------- */
static int eb(const Cubo &c, int i) {
    int c0=c.face[BORDAS[i][0][0]][BORDAS[i][0][1]][BORDAS[i][0][2]];
    int c1=c.face[BORDAS[i][1][0]][BORDAS[i][1][1]][BORDAS[i][1][2]];
    int e0=BCOR[i][0], e1=BCOR[i][1];
    if(c0==e0 && c1==e1) return 0;
    if(c0==e1 && c1==e0) return 2;
    if(c0==e0 || c1==e1) return 1;
    return 3;
}

/* -----------------------------------------------------------------------
 * Estado de canto i  →  0=correto  1=orientado  2=permutado  3=incorreto
 * ----------------------------------------------------------------------- */
static int ec(const Cubo &c, int i) {
    int co[3], es[3];
    for(int k=0;k<3;k++){
        co[k]=c.face[CANTOS[i][k][0]][CANTOS[i][k][1]][CANTOS[i][k][2]];
        es[k]=CCOR[i][k];
    }
    int ct=0;
    for(int k=0;k<3;k++) if(co[k]==es[k]) ct++;
    if(ct==3) return 0;
    int tm=0;
    for(int k=0;k<3;k++) for(int m=0;m<3;m++) if(co[k]==es[m]){tm++;break;}
    if(tm==3 && ct==0) return 2;
    if(ct==1)          return 1;
    return 3;
}

/* Mesma fitness() (já corrigida) usada no GA — garante paisagem idêntica
 * entre os algoritmos, para que a comparação seja de método, não de métrica. */
float fitness(const Cubo &orig, const std::vector<int> &v) {
    Cubo c = orig;
    aplicar(c, v);
    if(resolvido(c)) return FIT_MAX;
    int s = 0;
    for(int i=0;i<12;i++) s += FB[eb(c,i)];
    for(int i=0;i<8;i++)  s += FC[ec(c,i)];

    int fb_min = std::min({FB[0],FB[1],FB[2],FB[3]});
    int fb_max = std::max({FB[0],FB[1],FB[2],FB[3]});
    int fc_min = std::min({FC[0],FC[1],FC[2],FC[3]});
    int fc_max = std::max({FC[0],FC[1],FC[2],FC[3]});
    int s_min = 12*fb_min + 8*fc_min;
    int s_max = 12*fb_max + 8*fc_max;

    float r = ((float)(s_max - s) / (float)(s_max - s_min)) * 100.f;
    return r < 0 ? 0 : r > 99 ? 99 : r;
}

void embaralhar(Cubo &c, int n, unsigned seed) {
    std::mt19937 r(seed);
    static const char *nm[18] = {
        "U","D","F","B","R","L","U'","D'","F'","B'","R'","L'","U2","D2","F2","B2","R2","L2"
    };
    fprintf(stderr, "Embaralhamento (%d mov): ", n);
    for(int i=0;i<n;i++){int m=r()%NUM_MOV;aplicar_mov(c,m);fprintf(stderr,"%s ",nm[m]);}
    fprintf(stderr, "\n");
}

/* =========================================================================
 * VNS — Variable Neighborhood Search
 *
 * Solução  = sequência de movimentos (mesma representação do cromossomo
 *            do GA), tamanho entre n_embaralha e max_crom.
 *
 * Vizinhanças N1..N5 (aumentam a intensidade da perturbação):
 *   N1: troca 1 posição por um movimento aleatório
 *   N2: troca 2 posições
 *   N3: troca 3 posições
 *   N4: insere um movimento aleatório (cresce a solução, até max_crom)
 *   N5: remove um movimento aleatório (encolhe a solução, mínimo 1)
 *
 * Busca local (VND com N1): para cada posição, testa os 18 movimentos e
 * fixa o melhor — repete até não haver mais melhora ou atingir o limite
 * de passes.
 * ========================================================================= */

typedef std::vector<int> Sol;

void shake(Sol &x, int k, int max_crom, std::mt19937 &rng) {
    int n = (int)x.size();
    switch(k) {
        case 1:
            x[rng() % n] = rng() % NUM_MOV;
            break;
        case 2:
            for(int i=0;i<2 && i<n;i++) x[rng() % n] = rng() % NUM_MOV;
            break;
        case 3:
            for(int i=0;i<3 && i<n;i++) x[rng() % n] = rng() % NUM_MOV;
            break;
        case 4:
            if(n < max_crom) {
                int pos = rng() % (n + 1);
                x.insert(x.begin() + pos, rng() % NUM_MOV);
            } else {
                x[rng() % n] = rng() % NUM_MOV;   /* já no teto: cai para N1 */
            }
            break;
        case 5:
        default:
            if(n > 1) {
                x.erase(x.begin() + (rng() % n));
            } else {
                x[0] = rng() % NUM_MOV;           /* já no piso: cai para N1 */
            }
            break;
    }
}

/* Um pass de busca local sobre N1: para cada posição (ordem aleatória),
 * fixa o melhor dos 18 movimentos possíveis. Retorna true se melhorou. */
bool local_search_pass(Sol &x, float &fx, const Cubo &cubo, std::mt19937 &rng) {
    bool melhorou = false;
    std::vector<int> ordem(x.size());
    std::iota(ordem.begin(), ordem.end(), 0);
    std::shuffle(ordem.begin(), ordem.end(), rng);

    for(int pos : ordem) {
        int original = x[pos];
        int melhor_mov = original;
        float melhor_f = fx;
        for(int m=0;m<NUM_MOV;m++) {
            x[pos] = m;
            float f = fitness(cubo, x);
            if(f > melhor_f) { melhor_f = f; melhor_mov = m; }
        }
        x[pos] = melhor_mov;
        if(melhor_f > fx) { fx = melhor_f; melhorou = true; }
        if(fx >= FIT_MAX) break;
    }
    return melhorou;
}

void local_search(Sol &x, float &fx, const Cubo &cubo, std::mt19937 &rng) {
    for(int p=0; p<LS_MAX_PASSES; p++) {
        if(fx >= FIT_MAX) break;
        if(!local_search_pass(x, fx, cubo, rng)) break;
    }
}

Sol solucao_aleatoria(int n_embaralha, int max_crom, std::mt19937 &rng) {
    int tam = n_embaralha + (int)(rng() % (n_embaralha + 1));
    if(tam > max_crom) tam = max_crom;
    Sol v(tam);
    for(auto &m : v) m = rng() % NUM_MOV;
    return v;
}

int main(int argc, char **argv) {
    /* Uso: ./TesteCuboVNS <n_embaralha> [seed] */
    int n_embaralha = (argc > 1) ? atoi(argv[1]) : 20;
    unsigned seed   = (argc > 2) ? (unsigned)atoi(argv[2]) : 42;

    if(n_embaralha < 1 || n_embaralha > 30) {
        fprintf(stderr, "ERRO: n_embaralha deve ser entre 1 e 30\n");
        return 1;
    }

    int max_crom = n_embaralha * MAX_CROM_MULT;

    fprintf(stderr, "=== VNS (Variable Neighborhood Search) — SEQUENCIAL ===\n");
    fprintf(stderr, "Threads : 1 (sem paralelismo)\n");
    fprintf(stderr, "Cromo: %d..%d | Embaralha: %d | Seed: %u | kmax=%d\n\n",
            n_embaralha, max_crom, n_embaralha, seed, KMAX);

    Cubo cubo; cubo_init(cubo); embaralhar(cubo, n_embaralha, seed);

    /* RNG da busca deriva do seed (mesmo esquema de reprodutibilidade do GA) */
    std::mt19937 rng(seed * 2654435761u + 1u);

    Sol x = solucao_aleatoria(n_embaralha, max_crom, rng);
    float fx = fitness(cubo, x);

    Sol best = x; float fbest = fx;   /* melhor já encontrada — nunca piora,
                                        * análogo ao indivíduo elite do GA */
    float mg = -1;
    int estag = 0;
    int g_conv = 0;

    for(int g = 1; estag < MAX_ESTAG; g++) {
        if(fbest >= FIT_MAX) { g_conv = g; break; }

        int k = 1;
        while(k <= KMAX) {
            Sol xl = x;
            shake(xl, k, max_crom, rng);
            float fl = fitness(cubo, xl);
            local_search(xl, fl, cubo, rng);

            if(fl > fx) {
                x = std::move(xl);
                fx = fl;
                k = 1;               /* melhora: volta pra vizinhança mais leve */
            } else {
                k++;                 /* sem melhora: intensifica a perturbação */
            }
        }

        if(fx > fbest) { fbest = fx; best = x; }

        if(fbest > mg) {
            mg = fbest; estag = 0; g_conv = g;
        } else {
            estag++;
            /* Diversificação a cada 20 iterações sem melhora — análogo à
             * reinicialização parcial do GA. 'x' pode piorar (exploração),
             * mas 'best'/'fbest' preservam o melhor já visto. */
            if(estag % 20 == 0) {
                x = solucao_aleatoria(n_embaralha, max_crom, rng);
                fx = fitness(cubo, x);
            }
        }
    }

    printf("RESULTADO,VNS,1,%d,%.4f,%s,%d\n",
           n_embaralha, fbest,
           fbest >= FIT_MAX ? "SIM" : "NAO",
           g_conv);

    fprintf(stderr, "\nMelhor fitness : %.2f/100\n", fbest);
    fprintf(stderr, "Iterações      : %d\n", g_conv);
    fprintf(stderr, "Resolvido      : %s\n", fbest >= FIT_MAX ? "SIM" : "NAO");
    return 0;
}
/* Compilar: g++ -O3 -o TesteCuboVNS TesteCuboVNS.cpp
   Rodar:    ./TesteCuboVNS <n_mov> [seed]
   Tempo:    medido externamente via perf no script .sh               */
