#include <stdio.h>
#include <stdlib.h>

unsigned long long substituirZeros(unsigned long long n) {
    if (n == 0) {
        return 1;
    }

    unsigned long long resultado = 0;
    unsigned long long multiplicador = 1;

    while (n > 0) {
        unsigned long long digito = n % 10;

        if (digito == 0) {
            digito = 1;
        }

        resultado = digito * multiplicador + resultado;
        multiplicador *= 10;
        n /= 10;
    }
    return resultado;
}

unsigned long long produtoDigitos(unsigned long long n) {
    unsigned long long produto = 1;
    for (; n > 0; n /= 10) {
        unsigned long long digito = n % 10;
        produto = produto * digito;
    }
    return produto;
}

unsigned long long aplicarRegras(unsigned long long n) {
    unsigned long long numeroModificado = substituirZeros(n);

    unsigned long long produto = produtoDigitos(numeroModificado);

    if (produto == 1) {
        return numeroModificado + 1;
    }

    unsigned long long soma = numeroModificado + produto;

    if (numeroModificado % produto == 0) {
        return numeroModificado/produto;
    } else {
        return soma;
    }
}

void testarNumero(unsigned long long n) {
    const unsigned long long LIMITE = 1000000000000ULL;
    unsigned long long atual = n;
    unsigned long long contador = 0;

    while (contador <= LIMITE) {
        if (atual == 1 || atual == 2) {
            printf("Convergiu em %llu passos\n", contador);
            return;
        }

        atual = aplicarRegras(atual);
        contador++;

        printf("%llu - ", atual);

        if (contador % 20 == 0) {
            printf("\n");
        }
    }

    printf("\nO numero %llu nao convergiu (>%llu iteracoes). ultimo valor = %llu\n", n, LIMITE, atual);

}

int main(int argc, char *argv[]) {
    if (argc < 2) { //verifica se tem o parametro
        printf("Uso: %s <numero>\n", argv[0]);
        return 1;
    }

    unsigned long long numero = strtoull(argv[1], NULL, 10); //converte string para inteiro
    if (numero <= 0) {
        printf("Parametro invalido: %s\n", argv[1]);
        return 1;
    }

    testarNumero(numero);

    return 0;
}

//Para compilar: gcc -O3 TesteNumeroProdutoDigito2.c -o TesteNumeroProdutoDigito2
//Para rodar: ./TesteNumeroProdutoDigito2 100

