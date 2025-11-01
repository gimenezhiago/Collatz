//Código GMP tamanho maior de variavel

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h> //biblioteca do GMP

#define LIMITE 1000000000000ULL //1 trilhão ULL-unsigned long long

//oq tem ui é long long

void substituirZeros(mpz_t resultado, const mpz_t n) { //saida-entrada
    mpz_t temp_n, digito, multiplicador;
    mpz_inits(temp_n, digito, multiplicador, NULL);
    mpz_set(temp_n, n); //n = temp_n
    mpz_set_ui(resultado, 0); //resultado = 0
    mpz_set_ui(multiplicador, 1); //multiplicador = 1

    if (mpz_cmp_ui(n, 0) == 0) {
        mpz_set_ui(resultado, 1); //resultado = 1
        mpz_clears(temp_n, digito, multiplicador, NULL); //return
        return;
    }

    while (mpz_cmp_ui(temp_n, 0) > 0) {
        mpz_mod_ui(digito, temp_n, 10); //digito = temp_n % 10
        long int d = mpz_get_ui(digito); //pega como long int

        if (d == 0) {
            d = 1;
        }

        mpz_set_ui(digito, d); //digito = d
        mpz_mul(digito, digito, multiplicador); //digito = digito * multiplicador
        mpz_add(resultado, resultado, digito); //resultado += digito 

        mpz_div_ui(temp_n, temp_n, 10); //temp_n = temp_n / 10
        mpz_mul_ui(multiplicador, multiplicador, 10); //multiplicador = multiplicador * 10
    }
    mpz_clears(temp_n, digito, multiplicador, NULL);

}

void produtoDigitos(mpz_t produto, const mpz_t n) {
    mpz_t temp_n, digito;
    mpz_inits(temp_n, digito, NULL);
    mpz_set_ui(produto, 1); //produto = 1
    mpz_set(temp_n, n); //n = temp_n

    while (mpz_cmp_ui(temp_n, 0) > 0) {
        mpz_mod_ui(digito, temp_n, 10); //digito = temp_n % 10
        mpz_mul(produto, produto, digito); //produto = produto * digito
        mpz_div_ui(temp_n, temp_n, 10); //temp_n = temp_n / 10
    }
    mpz_clears(temp_n, digito, NULL); //Memoria do computador libera
}

void aplicarRegras(mpz_t resultado, const mpz_t n) {
    mpz_t numeroModificado, produto, soma, resto;
    mpz_inits(numeroModificado, produto, soma, resto, NULL);

    substituirZeros(numeroModificado, n); //numeroModificado = substituirZeros(n)

    produtoDigitos(produto, numeroModificado); //produto = produtoDigitos(numeroModificado)

    if (mpz_cmp_ui(produto, 1) == 0) { //produto == 1
        mpz_add_ui(resultado, numeroModificado, 1); //resultado = numeroModificado + 1
    } else {
        mpz_mod(resto, numeroModificado, produto); //resto = numeroModificado % produto

        if (mpz_cmp_ui(resto, 0) == 0) { //resto == 0
             mpz_div(resultado, numeroModificado, produto); //numeroModificado/produto
        } else {
            mpz_add(resultado, numeroModificado, produto); //numeroModificado + produto
        }
    }
    mpz_clears(numeroModificado, produto, soma, resto, NULL);
}

void testarNumero(const mpz_t n) {
    mpz_t atual, um, dois, temp_proximo;
    unsigned long long contador = 0;

    mpz_inits(atual, um, dois, temp_proximo, NULL);
    mpz_set(atual, n); //atual = n
    mpz_set_ui(um, 1); //um = 1
    mpz_set_ui(dois, 2); //dois = 2

    while (contador <= LIMITE) {
        if (mpz_cmp(atual, um) == 0 || mpz_cmp(atual, dois) == 0) {
            printf("\nConvergiu em %llu passos\n", contador);
            mpz_clears(atual, um, dois, temp_proximo, NULL);
            return;
        }

        aplicarRegras(temp_proximo, atual); //temp_proximo = atual
        mpz_set(atual, temp_proximo); //atual = temp_proximo
        contador++;

        mpz_out_str(stdout, 10, atual); //10-decimal
        printf(" - ");

        if (contador % 20 == 0) {
            printf("\n");
        }
    }

    //Caso não convergir
    printf("\n\nO numero inicial ");
    mpz_out_str(stdout, 10, n);
    printf(" nao convergiu (>%llu iteracoes).\n", LIMITE);
    printf("Último valor = ");
    mpz_out_str(stdout, 10, atual);
    printf("\n");

    mpz_clears(atual, um, dois, temp_proximo, NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <numero_muito_grande>\n", argv[0]);
        return 1;
    }

    mpz_t numero;
    mpz_init(numero);

    if (mpz_set_str(numero, argv[1], 10) != 0) {
        printf("Uso: %s <numero_muito_grande>\n", argv[0]);
        mpz_clear(numero);
        return 1;
    }

    if (mpz_cmp_ui(numero, 0) <= 0) {
        printf("Erro: O número deve ser positivo.\n");
        mpz_clear(numero);
        return 1;
    }

    testarNumero(numero);
    mpz_clear(numero);
    return 0;

}

//Para compilar: gcc -O3 TesteNumeroPD.c -o TesteNumeroPD -lgmp
//Para rodar:./TesteNumeroPD 100