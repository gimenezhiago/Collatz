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

void testarNumero (int n) {
    const int LIMITE = 100000;
    int atual = n;
    int contador = 0;

    while (contador <= LIMITE) {
        if (atual == 3 || atual == 7 || atual == 15) {
            printf("Convergiu em %d passos\n", contador);
            return;
        }

        atual = aplicarRegras(atual);
        contador++;

        printf("%d - ", atual);

        if (contador % 15 == 0) {
            printf("\n");
        }
    }

    printf("\nO numero %d nao convergiu (>%d iteracoes). ultimo valor = %d\n", n, LIMITE, atual);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { //verifica se tem o parametro
        printf("Uso: %s <numero>\n", argv[0]);
        return 1;
    }

    int numero = atoi(argv[1]); //converte string para inteiro
    if (numero <= 0) {
        printf("Parametro invalido: %s\n", argv[1]);
        return 1;
    }

    testarNumero(numero);

    return 0;
}

//Para compilar: gcc -O3 TesteNumerosManso.c -o TesteNumerosManso
//Para rodar: ./TesteNumerosManso 100
