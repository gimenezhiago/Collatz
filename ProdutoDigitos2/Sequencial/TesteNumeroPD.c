//Código GMP tamanho maior de variavel

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h> //biblioteca do GMP

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