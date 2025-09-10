#include <stdio.h>    
#include <stdlib.h>    
#include <math.h>     
#include <pthread.h>
#include "timer.h"


// Vetores globais que serão compartilhados entre as threads
float *a = NULL;
float *b = NULL;

// Estrutura para passar argumentos para as threads
typedef struct {
    short int nthreads;  // Número total de threads em execução
    long int dim;        // Dimensão total dos vetores
    short int id;        // ID único para cada thread (0 a nthreads-1)
} t_args;

// Função executada por cada thread para calcular parte do produto interno
// args: Ponteiro para a estrutura t_args com os parâmetros da thread
// Retorna: Ponteiro para o resultado parcial (float*)
void * task (void * args) {
    t_args *arg = (t_args*) args;     
    float *soma = malloc(sizeof(float));
    *soma = 0.0;                       

    // Calcula a fatia do vetor que esta thread irá processar
    int fatia = arg->dim / arg->nthreads;  
    int ini = arg->id * fatia;             
    int fim = ini + fatia;                 
    
    if(arg->id == (arg->nthreads)-1) fim = arg->dim;

    // Calcula o produto interno apenas na fatia designada
    for (int i=ini; i<fim; i++) {
        *soma += a[i] * b[i];
    }
    free(args);
    pthread_exit((void*)soma);
}


int main(int argc, char* argv[]) {
    double start, end, delta;

    // Verifica se o número de argumentos está correto
    if (argc != 3) {
        printf("Uso: %s <numero_de_threads> <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Converte e valida o número de threads
    long int nthreads = atoi(argv[1]);
    if (nthreads <= 0) {
        printf("O numero de threads deve ser um inteiro positivo\n");
        return 1;
    }

    char *nome_arquivo = argv[2];

    // Abre o arquivo binário para leitura
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        return 1;
    }

    long int n;
    float result_seq;
    
    // Lê o tamanho dos vetores
    if (fread(&n, sizeof(long int), 1, arquivo) != 1) {
        printf("Erro ao ler o tamanho dos vetores do arquivo.\n");
        fclose(arquivo);
        return 1;
    }

    // Aloca memória para os vetores
    a = (float*)malloc(n * sizeof(float));
    b = (float*)malloc(n * sizeof(float));
    
    if (a == NULL || b == NULL) {
        printf("Erro ao alocar memória para os vetores.\n");
        fclose(arquivo);
        return 1;
    }

    if (fread(a, sizeof(float), n, arquivo) != n) {
        printf("Erro ao ler o vetor A no arquivo.");
        fclose(arquivo);
        return -1;    
    }

    if (fread(b, sizeof(float), n, arquivo) != n) {
        printf("Erro ao ler o vetor B no arquivo.");
        fclose(arquivo);
        return -1;    
    }

    if (fread(&result_seq, sizeof(float), 1, arquivo) != 1) {
        printf("Erro ao ler o resultado do produto interno no arquivo.");
        fclose(arquivo);
        return -1;    
    }

    GET_TIME(start);//começa a contagem de tempo
    //aloca espaco para o vetor de tid
    pthread_t tid[nthreads];
    for (int i=0; i<nthreads; i++) {
        t_args *args = (t_args*) malloc(sizeof(t_args));
        if(args==NULL){
           fprintf(stderr, "ERRO de alocacao de argumentos da thread %hd.\n", i);
           return 2;
        }   
        args->nthreads=nthreads;
        args->dim = n;
        args->id=i;
        if(pthread_create(&tid[i], NULL, task, (void*) args)) {
           fprintf(stderr, "ERRO de criacao da thread %hd.\n", i);
           return 3;
        }
    }

    float *result_thread;       // Ponteiro para armazenar o resultado de cada thread
    float result_conc = 0.0;    // Acumula o resultado final concorrente

    // Aguarda o término de todas as threads e coleta os resultados
    for(short int i=0; i<nthreads; i++) {
        if(pthread_join(tid[i], (void*) &result_thread)){
            fprintf(stderr, "ERRO de join da thread %hd.\n", i);
            return 4;
        }
        result_conc += *result_thread;
        free(result_thread);
    }
    GET_TIME(end); //termina a contagem de tempo
    delta = end-start; //calcula o intervalo de tempo para processamento do vetor

    free(a);
    free(b);

    printf("\n=== Resultados ===\n");
    printf("Threads utilizadas: %ld\n", nthreads);
    printf("Tamanho dos vetores: %ld\n", n);
    printf("Produto interno (concorrente): %.6f\n", result_conc);
    printf("Produto interno (sequencial):  %.6f\n", result_seq);
    
    float variacao = fabs((result_seq - result_conc) / result_seq);
    printf("Variação relativa: %.6e\n", variacao);
    printf("Tempo: %lf\n", delta);

}
