#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/**
 * @brief Gera um número float aleatório entre min e max
 * @param min Valor mínimo (inclusivo)
 * @param max Valor máximo (inclusivo)
 * @return Número aleatório no intervalo [min, max]
 */
float random_float(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

int main() {
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));
    
    // Tamanhos dos vetores de teste
    long int tamanhos[] = {5, 10, 100, 1000, 10000, 100000};
    int num_testes = sizeof(tamanhos) / sizeof(tamanhos[0]);
    
    // Intervalo para geração dos números aleatórios
    const float MIN_VAL = -1000.0f;
    const float MAX_VAL = 1000.0f;
    
    printf("Gerando arquivos de teste...\n");
    
    for (int i = 0; i < num_testes; i++) {
        long int n = tamanhos[i];
        
        // Cria o nome do arquivo
        char nome_arquivo[100];
        sprintf(nome_arquivo, "arquivos/vetores_%ld.bin", n);
        
        // Abre o arquivo para escrita binária
        FILE* arquivo = fopen(nome_arquivo, "wb");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo %s\n", nome_arquivo);
            continue;
        }
        
        // Escreve o tamanho dos vetores
        if (fwrite(&n, sizeof(long int), 1, arquivo) != 1) {
            printf("Erro ao escrever o tamanho no arquivo %s\n", nome_arquivo);
            fclose(arquivo);
            continue;
        }
        
        // Aloca memória para os vetores
        float* a = (float*)malloc(n * sizeof(float));
        float* b = (float*)malloc(n * sizeof(float));
        
        if (a == NULL || b == NULL) {
            printf("Erro ao alocar memória para os vetores de tamanho %ld\n", n);
            fclose(arquivo);
            free(a);
            free(b);
            continue;
        }
        
        // Preenche os vetores com valores aleatórios
        for (long int j = 0; j < n; j++) {
            a[j] = random_float(MIN_VAL, MAX_VAL);
            b[j] = random_float(MIN_VAL, MAX_VAL);
        }
        
        // Calcula o produto interno sequencialmente
        float resultado = 0.0f;
        for (long int j = 0; j < n; j++) {
            resultado += a[j] * b[j];
        }
        
        // Escreve os vetores e o resultado no arquivo
        if (fwrite(a, sizeof(float), n, arquivo) != (size_t)n ||
            fwrite(b, sizeof(float), n, arquivo) != (size_t)n ||
            fwrite(&resultado, sizeof(float), 1, arquivo) != 1) {
            printf("Erro ao escrever os dados no arquivo %s\n", nome_arquivo);
        } else {
            printf("Arquivo gerado: %s (n = %ld)\n", nome_arquivo, n);
        }
        
        // Libera memória e fecha o arquivo
        free(a);
        free(b);
        fclose(arquivo);
    }
    
    printf("\nTodos os arquivos de teste foram gerados com sucesso!\n");
    return 0;
}
