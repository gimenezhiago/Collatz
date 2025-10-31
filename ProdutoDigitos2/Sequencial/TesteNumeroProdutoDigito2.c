#include <stdio.h>
#include <stdlib.h>

long long substituirZeros(long long n) {
    if (n == 0) {
        return 1;
    }

    long long resultado = 0;
    long long multiplicador = 1;

    while (n > 0) {
        long long digito = n % 10;

        if (digito == 0) {
            digito = 1;
        }

        resultado = digito * multiplicador + resultado;
        multiplicador *= 10;
        n /= 10;
    }
    return resultado;
}

long long produtoDigitos(long long n) {
    long long produto = 1;
    for (; n > 0; n /= 10) {
        long long digito = n % 10;
        produto = produto * digito;
    }
    return produto;
}

long long aplicarRegras(long long n) {
    long long numeroModificado = substituirZeros(n);

    long long produto = produtoDigitos(numeroModificado);

    if (produto == 1) {
        return numeroModificado + 1;
    }

    long long soma = numeroModificado + produto;

    if (numeroModificado % produto == 0) {
        return numeroModificado/produto;
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

//Para compilar: gcc -O3 TesteNumeroProdutoDigito2.c -o TesteNumeroProdutoDigito2
//Para rodar: ./TesteNumeroProdutoDigito2 100

