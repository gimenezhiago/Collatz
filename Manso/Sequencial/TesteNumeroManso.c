#include <stdio.h>
#include <stdbool.h> //para booleano
#include <stdlib.h> //serve para pegar o atoi

bool ehPrimo(long long n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

long long somaAlgarismo (long long n) {
    long long soma = 0;
    for (; n > 0; n /= 10) { // n = n/10
        soma += n % 10;
    }
    return soma;
}

long long aplicarRegras (long long n) {
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

void testarNumero (long long n) {
    const long long LIMITE = 1000000000000; //1 trilhão;
    long long atual = n;
    long long contador = 0;

    while (contador <= LIMITE) {
        if (atual == 3 || atual == 7 || atual == 15) {
            printf("Convergiu em %lld passos\n", contador);
            return;
        }

        atual = aplicarRegras(atual);
        contador++;

        printf("%lld - ", atual);

        if (contador % 10 == 0) {
            printf("\n");
        }
    }

    printf("\nO numero %lld nao convergiu (>%lld iteracoes). ultimo valor = %lld\n", n, LIMITE, atual);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { //verifica se tem o parametro
        printf("Uso: %s <numero>\n", argv[0]);
        return 1;
    }

    long long numero = atoll(argv[1]); //converte string para inteiro
    if (numero <= 0) {
        printf("Parametro invalido: %s\n", argv[1]);
        return 1;
    }

    testarNumero(numero);

    return 0;
}

//Para compilar: gcc -O3 TesteNumerosManso.c -o TesteNumerosManso
//Para rodar: ./TesteNumerosManso 100
