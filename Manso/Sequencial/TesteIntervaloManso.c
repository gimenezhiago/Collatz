#include <stdio.h>
#include <stdbool.h> //para booleano
#include <stdlib.h> //serve para pegar o atoi

bool ehPrimo(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) { 
        if (n % i == 0) return false;
    }
    return true;
}

int somaAlgarismo (int n) {
    int soma = 0;
    for (; n > 0; n /= 10) { // n = n/10
        soma += n % 10;
    }
    return soma;
}

int aplicarRegras (int n) {
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

void testeIntervalo (int fim) {
    const int LIMITE = 100000;
    for (int i = 1; i <= fim; i++) {
        int atual = i;
        int contador = 0;
        int historico[4] = {0, 0, 0, 0};
        bool convergiu = false;

        while (contador <= LIMITE) {
            atual = aplicarRegras(atual);
            contador++;

            historico[0] = historico[1];
            historico[1] = historico[2];
            historico[2] = historico[3];
            historico[3] = atual;

            if (contador >= 4 && historico[0] == 3 && historico[1] == 15 && historico[2] == 7 && historico[3] == 3) {
                convergiu = true;
                break;
            }
        }

        if (!convergiu) {
            printf("O numero %d nao convergiu (>%d iteracoes). ultimo valor = %d\n", i, LIMITE, atual);
            // continue para testar o próximo número em vez de sair na primeira falha
            continue;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { //verifica se tem o parametro
        printf("Uso: %s <fim>\n", argv[0]); //
        return 1;
    }

    int fim = atoi(argv[1]); //converte string para inteiro
    if (fim <= 0) { 
        printf("Parametro invalido para 'fim': %s\n", argv[1]);
        return 1;
    }

    printf("Testando ate %d\n", fim);

    testeIntervalo(fim);

    return 0;
}

//Para compilar: gcc TesteIntervaloManso.c -O3 TesteIntervaloManso
//Para rodar: ./TesteIntervaloManso 100