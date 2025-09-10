#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float produto_interno (float *a, float *b, long int n) {
    float soma=0.0;
    for (long int i = 0; i < n; i++) {
        soma+=a[i]*b[i];
    }
    return soma;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_dos_vetores>\n", argv[0]);
        return 1;
    }

    long int n = atoi(argv[1]);
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
    for (long int i = 0; i < n; i++) {
        a[i] = (float)rand() / RAND_MAX * 100.0f;
        printf("%.2f ", a[i]);
    }
    printf("\n");

    // Gera e imprime o vetor B
    printf("Vetor B: ");
    for (long int i = 0; i < n; i++) {
        b[i] = (float)rand() / RAND_MAX * 100.0f;
        printf("%.2f ", b[i]);
    }
    printf("\n");

    FILE *arquivo;
    arquivo = fopen("vetores.bin", "wb");

    // Escrita do N e verificação se foi armazenado corretamente
    if (fwrite(&n, sizeof(long int), 1, arquivo) != 1) {
        printf("Erro ao inserir o N no arquivo.");
        fclose(arquivo);
        return -1;
    }

    // Escrita do vetor a no arquivo binário e verificação se foi armazenados todas as posições de a
    if (fwrite(a, sizeof(float), n, arquivo) != n) {
        printf("Erro ao inserir o vetor A.");
        fclose(arquivo);
        return -1;
    }

    // Escrita do vetor b no arquivo binário e verificação se foi armazenados todas as posições de b
    if (fwrite(b, sizeof(float), n, arquivo) != n) {
        printf("Erro ao inserir o vetor B.");
        fclose(arquivo);
        return -1;
    }

    // Cálculo do produto interno
    float result = produto_interno(a, b, n);
    printf("Produto interno: %f\n", result);

    if (fwrite(&result, sizeof(float), 1, arquivo) != 1) {
        printf("Erro ao inserir o produto interno.");
        fclose(arquivo);
        return -1;
    }

    // Fechamento do arquivo
    fclose(arquivo);

    // Libera a memória alocada
    free(a);
    free(b);

    return 0;
}
