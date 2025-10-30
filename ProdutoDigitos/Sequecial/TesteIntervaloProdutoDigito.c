#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

long long produtoDigitos(long long n) {
    long long produto = 1;
    for (; n > 0; n /= 10) {
        long long digito = n % 10;
        if (digito == 0) {
            digito = 1;
        }
        produto = produto * digito;
    }
    return produto;
}

long long aplicarRegras(long long n) {
    long long produto = produtoDigitos(n);

    if (produto == 1) {
        return n + 1;
    }

    long long soma = n + produto;

    if (n % produto == 0) {
        return n/produto;
    } else {
        return soma;
    }
}

void testeIntervalo (long long fim) {
    const long long LIMITE = 1000000000000;

    for (long long i = 1; i <= fim; i++) {
        long long atual = i;
        long long contador = 0;
        bool convergiu = false;

        while (contador < LIMITE) {
            if (atual == 1 || atual == 2) {
                convergiu = true;
                break;
            }

            atual = aplicarRegras(atual);
            contador++;

            if (atual <= 0) {
                printf("O numero %lld gerou valor invalido em %lld iteracoes. ultimo valor = %lld\n", i, contador, atual);
                break;
            }
        }

        if (!convergiu) {
            printf("O numero %lld nao convergiu (>%lld iteracoes). ultimo valor = %lld\n", i, LIMITE, atual);
            break;
        }

        if (i % 100000 == 0) {
            printf("Testados %lld numeros\n", i);
        }
    }

    printf("Teste concluido ate %lld\n", fim);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <fim>\n", argv[0]);
        return 1;
    }

    long long fim = atoll(argv[1]);
    if (fim <= 0) {
        printf("Parametro invalido: %s\n", argv[1]);
        return 1;
    }

    printf("Testando ate %lld\n", fim);
    testeIntervalo(fim);

    return 0;
}

//Para compilar: gcc -O3 TesteIntervaloProdutoDigito.c -o TesteIntervaloProdutoDigito
//Para rodar: ./TesteIntervaloProdutoDigito 100
