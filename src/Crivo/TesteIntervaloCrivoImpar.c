#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/resource.h>
#include <gmp.h>
#include <omp.h>

#define BLOCO 1048576LL

static long long get_mem_kb(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
#ifdef __APPLE__
    return ru.ru_maxrss / 1024;
#else
    return ru.ru_maxrss;
#endif
}

void testeCrivo(long long N) {
    if (N < 2) { printf("Nao ha primos.\n"); return; }

    long long limite = (long long)sqrt((double)N) + 1;
    long long tam_pequeno = (limite / 2) + 2;

    bool *pequeno = calloc(tam_pequeno, sizeof(bool));
    if (!pequeno) { fprintf(stderr, "Erro de alocacao.\n"); return; }

    /* Usamos mpz_t para acumular contadores que podem ultrapassar 2^63 */
    mpz_t total_primos, total_marcacoes;
    mpz_init_set_ui(total_primos,   1); /* conta o 2 */
    mpz_init_set_ui(total_marcacoes, 0);

    /* Fase pequena: sequencial */
    for (long long i = 1; 2*i+1 <= limite; i++) {
        if (!pequeno[i]) {
            long long p = 2*i + 1;
            for (long long j = (p*p - 1)/2; j < tam_pequeno; j += p) {
                if (!pequeno[j]) { pequeno[j] = true; mpz_add_ui(total_marcacoes, total_marcacoes, 1); }
            }
        }
    }

    long long n_pp = 0;
    for (long long i = 1; i < tam_pequeno; i++)
        if (!pequeno[i]) n_pp++;

    long long *primos_p = malloc(n_pp * sizeof(long long));
    if (!primos_p) { free(pequeno); fprintf(stderr, "Erro de alocacao.\n"); return; }

    long long idx = 0;
    for (long long i = 1; i < tam_pequeno; i++)
        if (!pequeno[i]) primos_p[idx++] = 2*i + 1;
    free(pequeno);

    for (long long i = 0; i < n_pp; i++)
        if (primos_p[i] <= N) mpz_add_ui(total_primos, total_primos, 1);

    /* Fase segmentada: paralela com OpenMP */
    long long base_global = limite + 1;
    if (base_global % 2 == 0) base_global++;

    long long n_segmentos = 0;
    {
        long long b = base_global;
        while (b <= N) {
            n_segmentos++;
            long long topo = b + 2*BLOCO - 2;
            if (topo > N) topo = N;
            if (topo % 2 == 0) topo--;
            b = topo + 2;
        }
    }

    #pragma omp parallel
    {
        bool *bloco_local = malloc(BLOCO * sizeof(bool));
        if (!bloco_local) {
            #pragma omp cancel parallel
        }

        long long loc_primos = 0, loc_marcacoes = 0;

        #pragma omp for schedule(dynamic, 1)
        for (long long seg = 0; seg < n_segmentos; seg++) {
            long long base = base_global + seg * 2 * BLOCO;
            if (base % 2 == 0) base++;

            long long topo = base + 2*BLOCO - 2;
            if (topo > N) topo = N;
            if (topo % 2 == 0) topo--;

            if (base > N) continue;

            long long tam_seg = (topo - base) / 2 + 1;
            for (long long k = 0; k < tam_seg; k++) bloco_local[k] = false;

            for (long long pi = 0; pi < n_pp; pi++) {
                long long p = primos_p[pi];
                long long primeiro = ((base + p - 1) / p) * p;
                if (primeiro % 2 == 0) primeiro += p;
                /* Usa __int128 para evitar overflow em p*p quando p > ~3e9 */
                __int128 pp = (__int128)p * p;
                if ((__int128)primeiro < pp) {
                    if (pp > (__int128)topo) continue; /* p*p além do segmento, nada a marcar */
                    primeiro = (long long)pp;
                }

                for (long long mult = primeiro; mult <= topo; mult += 2*p) {
                    long long k = (mult - base) / 2;
                    if (!bloco_local[k]) { bloco_local[k] = true; loc_marcacoes++; }
                }
            }

            for (long long k = 0; k < tam_seg; k++)
                if (!bloco_local[k]) loc_primos++;
        }

        free(bloco_local);

        #pragma omp critical
        {
            mpz_add_ui(total_primos,    total_primos,    (unsigned long)loc_primos);
            mpz_add_ui(total_marcacoes, total_marcacoes, (unsigned long)loc_marcacoes);
        }
    }

    long long mem_kb = get_mem_kb();

    gmp_printf("PRIMOS=%Zd\n",    total_primos);
    gmp_printf("MARCACOES=%Zd\n", total_marcacoes);
    printf("MEM_KB=%lld\n", mem_kb);

    mpz_clear(total_primos);
    mpz_clear(total_marcacoes);
    free(primos_p);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Uso: %s <N>\n", argv[0]); return 1; }

    /* Lê N como mpz para suportar valores até 10^14 e além */
    mpz_t N_big;
    mpz_init_set_str(N_big, argv[1], 10);

    /* Para a lógica interna ainda usamos long long (suficiente até ~9.2×10^18) */
    long long N = mpz_get_si(N_big);
    mpz_clear(N_big);

    testeCrivo(N);
    return 0;
}

// Compilar: gcc -O3 -fopenmp TesteIntervaloCrivoImpar.c -o TesteIntervaloCrivoImpar -lm -lgmp
// Rodar:    OMP_NUM_THREADS=8 ./TesteIntervaloCrivoImpar 100000000000000
