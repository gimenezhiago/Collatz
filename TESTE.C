#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Definindo constantes para número de cidades, iterações e parâmetros do SA
#define NUM_CITIES 10
#define ITERATIONS 10000
#define INITIAL_TEMP 10000.0
#define COOLING_RATE 0.999

// Matriz de distâncias entre as cidades
double distanceMatrix[NUM_CITIES][NUM_CITIES];

// Função que gera distâncias aleatórias entre cidades
void generateDistanceMatrix() {
    for (int i = 0; i < NUM_CITIES; i++) {
        for (int j = 0; j < NUM_CITIES; j++) {
            if (i == j)
                distanceMatrix[i][j] = 0; // distância da cidade para si mesma é 0
            else
                distanceMatrix[i][j] = rand() % 100 + 1; // valores entre 1 e 100
        }
    }
}

// Calcula a distância total da rota passando por todas as cidades
double calculateDistance(int route[]) {
    double total = 0;
    for (int i = 0; i < NUM_CITIES - 1; i++) {
        total += distanceMatrix[route[i]][route[i+1]]; // soma distância entre cada par consecutivo
    }
    total += distanceMatrix[route[NUM_CITIES - 1]][route[0]]; // retorna à cidade inicial
    return total;
}

// Troca duas cidades aleatórias na rota para gerar uma vizinha
void swapCities(int route[]) {
    int a = rand() % NUM_CITIES;
    int b = rand() % NUM_CITIES;
    int temp = route[a];
    route[a] = route[b];
    route[b] = temp;
}

// Copia uma rota de um vetor para outro
void copyRoute(int source[], int dest[]) {
    for (int i = 0; i < NUM_CITIES; i++) {
        dest[i] = source[i];
    }
}

int main() {
    srand(time(NULL)); // Semente aleatória para gerar diferentes resultados
    generateDistanceMatrix(); // Gera a matriz de distâncias aleatórias

    // Cria rota inicial simples: [0, 1, 2, ..., NUM_CITIES - 1]
    int currentRoute[NUM_CITIES];
    for (int i = 0; i < NUM_CITIES; i++) currentRoute[i] = i;

    int bestRoute[NUM_CITIES]; // Melhor rota encontrada
    copyRoute(currentRoute, bestRoute); // Inicialmente, melhor = atual
    double bestDistance = calculateDistance(bestRoute); // Calcula distância inicial
    double temp = INITIAL_TEMP; // Temperatura inicial

    // Loop principal do Simulated Annealing
    for (int i = 0; i < ITERATIONS; i++) {
        int newRoute[NUM_CITIES];
        copyRoute(currentRoute, newRoute); // Faz cópia da rota atual
        swapCities(newRoute); // Aplica uma pequena perturbação

        double currentDistance = calculateDistance(currentRoute); // Distância atual
        double newDistance = calculateDistance(newRoute); // Distância da vizinha
        double delta = newDistance - currentDistance; // Diferença entre as soluções

        // Aceita nova solução se for melhor, ou probabilisticamente se for pior
        if (delta < 0 || exp(-delta / temp) > ((double)rand() / RAND_MAX)) {
            copyRoute(newRoute, currentRoute); // Atualiza a rota atual
            if (newDistance < bestDistance) {
                bestDistance = newDistance; // Atualiza a melhor distância
                copyRoute(newRoute, bestRoute); // Atualiza a melhor rota
            }
        }

        temp *= COOLING_RATE; // Reduz gradualmente a temperatura
    }

    // Exibe o resultado final
    printf("Melhor distância encontrada: %.2f\n", bestDistance);
    printf("Melhor rota: ");
    for (int i = 0; i < NUM_CITIES; i++) {
        printf("%d ", bestRoute[i]);
    }
    printf("\n");

    return 0;
}
