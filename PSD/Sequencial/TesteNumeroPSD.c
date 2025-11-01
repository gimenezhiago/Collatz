#include <stdio.h>
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

void testarNumero(long long n) {
    const long long LIMITE = 1000000000000;
    long long atual = n;
    long long contador = 0;

    while (contador <= LIMITE) {
        if (atual == 1 || atual == 2) {
            printf("Convergiu em %lld passos\n", contador);
            return;
        }

        atual = aplicarRegras(atual);
        contador++;

        printf("%lld - ", atual);

        if (contador % 20 == 0) {
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

//Para compilar: gcc -O3 TesteNumeroProdutoDigito.c -o TesteNumeroProdutoDigito
//Para rodar: ./TesteNumeroProdutoDigito 100

