#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_dos_vetores>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("O tamanho dos vetores deve ser um inteiro positivo\n");
        return 1;
    }

    // Aloca memória para os vetores
    float* a = (float*)malloc(n * sizeof(float));
    float* b = (float*)malloc(n * sizeof(float));

    // Inicializa a semente para números aleatórios
    // Garante que a semente será diferente a cada execução.
    srand(time(NULL));

    // Gera e imprime o vetor A
    printf("Vetor A: ");
    for (int i = 0; i < n; i++) {
        a[i] = (float)rand() / RAND_MAX * 100.0f;
        printf("%.2f ", a[i]);
    }
    printf("\n");

    // Gera e imprime o vetor B
    printf("Vetor B: ");
    for (int i = 0; i < n; i++) {
        b[i] = (float)rand() / RAND_MAX * 100.0f;
        printf("%.2f ", b[i]);
    }
    printf("\n");

    // Libera a memória alocada
    free(a);
    free(b);

    return 0;
}
