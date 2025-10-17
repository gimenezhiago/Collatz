#include <stdio.h>
#include <stdbool.h> //Para usar booleano
#include <stdlib.h> //Para usar atoi

bool ehPrimo(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) { 
        if (n % i == 0) return false;
    }
    return true;
}

int somaAlgarismo(int n) {
    int soma = 0;
    for (; n > 0; n /= 10) { //n = n / 10
        soma += n % 10;
    }
    return soma;
}

int aplicarRegras(int n) {
    if (ehPrimo(n)) {
        return n + (n + 1);
    } else if (n % 5 == 0) {
        return n / 5;
    } else if (n % 9 == 0) {
        return n + 1;
    } else if (n % 2 == 0) {
        return n / 2;
    } else {
        return somaAlgarismo(n) + n;
    }
}

void testeIntervalo(int fim) {
    const int LIMITE = 100000;
    for (int i = 1; i <= fim; i++) {
        int atual = i;
        int contador = 0;
        bool convergiu = false;

        while (contador < LIMITE) {
            if (atual == 3 || atual == 7 || atual == 15 || 
                atual == 1 || atual == 2 || atual == 5) {
                convergiu = true;
                break;
            }

            atual = aplicarRegras(atual);
            contador++;
        }

        if (!convergiu) {
            printf("O numero %d nao convergiu (>%d iteracoes). ultimo valor = %d\n", i, LIMITE, atual);
            break;
        }

        if (i % 1000 == 0) {
            printf("Testados %d numeros\n", i);
        }
    }
    printf("Teste concluido ate %d\n", fim);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <fim>\n", argv[0]);
        return 1;
    }

    int fim = atoi(argv[1]);
    if (fim <= 0) { 
        printf("Parametro invalido: %s\n", argv[1]);
        return 1;
    }

    printf("Testando ate %d\n", fim);
    testeIntervalo(fim);

    return 0;
}

// Para compilar: gcc -O3 TesteIntervaloManso.c -o TesteIntervaloManso
// Para rodar: ./TesteIntervaloManso 100