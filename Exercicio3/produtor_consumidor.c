/*
 * Laboratório 8 - Atividade 1
 * Padrão Produtor/Consumidor com Semáforos
 * Autor: Gabriel Martins
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

// Estrutura de dados da thread consumidora
typedef struct {
    int thread_id;
    int contador_primos;
} DadosConsumidor;

// Estado compartilhado do buffer
typedef struct {
    long long int *buffer;
    int capacidade;
    int count;
    int in;
    int out;
    int producao_finalizada;
} BufferCircular;

// Variáveis globais
BufferCircular canal;
long long int n;
int nthreads;
long long int proximo = 0;
long long int consumidos = 0;

// Semáforos para sincronização
sem_t mutex;
sem_t empty;
sem_t full;

// Função para verificar se um número é primo
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Função sequencial para validação de corretude
int sequencial() {
    int qtd_primos = 0;
    for (long long int i = 0; i < n; i++) {
        if (ehPrimo(i)) qtd_primos++;
    }
    return qtd_primos;
}

// Função da thread produtora
void* funcao_produtora(void* arg) {
    printf("[PRODUTOR] Thread iniciada\n");
    
    while (proximo < n) {
        // Aguarda espaço disponível no buffer
        sem_wait(&empty);
        sem_wait(&mutex);
        
        // Produz um lote completo de M elementos
        int quantidade_produzida = 0;
        long long int inicio_lote = proximo;
        
        while (quantidade_produzida < canal.capacidade && 
               proximo < n) {
            
            canal.buffer[canal.in] = proximo;
            canal.in = (canal.in + 1) % canal.capacidade;
            
            proximo++;
            quantidade_produzida++;
        }
        
        canal.count = quantidade_produzida;
        
        printf("[PRODUTOR] Lote produzido: %d elementos [%lld-%lld]\n", 
               quantidade_produzida, inicio_lote, proximo - 1);
        
        sem_post(&mutex);
        
        // Libera sinais para as consumidoras (um por elemento)
        for (int i = 0; i < quantidade_produzida; i++) {
            sem_post(&full);
        }
    }
    
    // Marca produção como finalizada
    sem_wait(&mutex);
    canal.producao_finalizada = 1;
    sem_post(&mutex);
    
    // Acorda todas as consumidoras para finalizarem
    for (int i = 0; i < nthreads; i++) {
        sem_post(&full);
    }
    
    printf("[PRODUTOR] Produção finalizada\n");
    pthread_exit(NULL);
}

// Função da thread consumidora
void* funcao_consumidora(void* arg) {
    DadosConsumidor* meus_dados = (DadosConsumidor*)arg;
    meus_dados->contador_primos = 0;
    
    printf("[CONSUMIDOR %d] Thread iniciada\n", meus_dados->thread_id);
    
    while (1) {
        // Aguarda item disponível
        sem_wait(&full);
        sem_wait(&mutex);
        
        // Verifica condição de parada
        if (consumidos >= n) {
            sem_post(&mutex);
            break;
        }
        
        // Verifica se produção terminou e buffer está vazio
        if (canal.producao_finalizada && canal.count == 0) {
            sem_post(&mutex);
            break;
        }
        
        // Consome um elemento
        long long int item = canal.buffer[canal.out];
        canal.out = (canal.out + 1) % canal.capacidade;
        canal.count--;
        consumidos++;
        
        // Se esvaziou o buffer, libera o produtor
        if (canal.count == 0 && consumidos < n) {
            sem_post(&empty);
        }
        
        sem_post(&mutex);
        
        // Processa fora da região crítica (paralelismo)
        if (ehPrimo(item)) {
            meus_dados->contador_primos++;
            printf("[CONSUMIDOR %d] Encontrou primo: %lld (total: %d)\n", 
                   meus_dados->thread_id, item, meus_dados->contador_primos);
        }
    }
    
    printf("[CONSUMIDOR %d] Finalizou com %d primos\n", 
           meus_dados->thread_id, meus_dados->contador_primos);
    
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <N> <M> <threads_consumidoras>\n", argv[0]);
        fprintf(stderr, "  N: total de números a processar\n");
        fprintf(stderr, "  M: capacidade do buffer (M << N)\n");
        fprintf(stderr, "  threads_consumidoras: quantidade de threads consumidoras\n");
        return EXIT_FAILURE;
    }
    
    // Leitura dos parâmetros
    n = atoll(argv[1]);
    int m = atoi(argv[2]);
    nthreads = atoi(argv[3]);
    
    // Validações
    if (m >= n) {
        fprintf(stderr, "ERRO: Buffer muito grande! M deve ser << N\n");
        return EXIT_FAILURE;
    }
    
    printf("\n================================================\n");
    printf("  PRODUTOR/CONSUMIDOR - PRIMALIDADE\n");
    printf("================================================\n\n");
    printf("Configuracao:\n");
    printf("  Numeros a processar: %lld\n", n);
    printf("  Tamanho do buffer: %d\n", m);
    printf("  Threads consumidoras: %d\n\n", nthreads);
    
    // Inicializa buffer circular
    canal.buffer = (long long int*)malloc(m * sizeof(long long int));
    canal.capacidade = m;
    canal.count = 0;
    canal.in = 0;
    canal.out = 0;
    canal.producao_finalizada = 0;
    
    if (!canal.buffer) {
        fprintf(stderr, "ERRO: Falha ao alocar buffer\n");
        return EXIT_FAILURE;
    }
    
    // Inicializa semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);
    
    // Aloca estruturas das threads
    pthread_t tid_produtora;
    pthread_t* tids_consumidoras = 
        (pthread_t*)malloc(nthreads * sizeof(pthread_t));
    DadosConsumidor* dados_consumidores = 
        (DadosConsumidor*)malloc(nthreads * sizeof(DadosConsumidor));
    
    if (!tids_consumidoras || !dados_consumidores) {
        fprintf(stderr, "ERRO: Falha ao alocar estruturas das threads\n");
        return EXIT_FAILURE;
    }
    
    // Marca tempo de início
    clock_t tempo_inicio = clock();
    
    // Cria thread produtora
    if (pthread_create(&tid_produtora, NULL, funcao_produtora, NULL) != 0) {
        fprintf(stderr, "ERRO: Falha ao criar thread produtora\n");
        return EXIT_FAILURE;
    }
    
    // Cria threads consumidoras
    for (int i = 0; i < nthreads; i++) {
        dados_consumidores[i].thread_id = i + 1;
        dados_consumidores[i].contador_primos = 0;
        
        if (pthread_create(&tids_consumidoras[i], NULL, funcao_consumidora, 
                          &dados_consumidores[i]) != 0) {
            fprintf(stderr, "ERRO: Falha ao criar thread consumidora %d\n", i + 1);
            return EXIT_FAILURE;
        }
    }
    
    // Aguarda finalização das threads
    pthread_join(tid_produtora, NULL);
    
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tids_consumidoras[i], NULL);
    }
    
    clock_t tempo_fim = clock();
    double tempo_decorrido = ((double)(tempo_fim - tempo_inicio)) / CLOCKS_PER_SEC;
    
    // Processa resultados
    int total_primos_concorrente = 0;
    int id_vencedora = 1;
    int max_primos_encontrados = dados_consumidores[0].contador_primos;
    
    printf("\n================================================\n");
    printf("           RESULTADOS FINAIS\n");
    printf("================================================\n\n");
    
    for (int i = 0; i < nthreads; i++) {
        printf("  Thread %d: %d primos encontrados\n", 
               dados_consumidores[i].thread_id, dados_consumidores[i].contador_primos);
        
        total_primos_concorrente += dados_consumidores[i].contador_primos;
        
        if (dados_consumidores[i].contador_primos > max_primos_encontrados) {
            max_primos_encontrados = dados_consumidores[i].contador_primos;
            id_vencedora = dados_consumidores[i].thread_id;
        }
    }
    
    // Validacao sequencial
    printf("\nValidando corretude...\n");
    int total_primos_sequencial = sequencial();
    
    printf("\n================================================\n");
    printf("              ESTATISTICAS\n");
    printf("================================================\n\n");
    printf("  Primos (sequencial):   %d\n", total_primos_sequencial);
    printf("  Primos (concorrente):  %d\n", total_primos_concorrente);
    printf("  Thread vencedora:      #%d (%d primos)\n", 
           id_vencedora, max_primos_encontrados);
    printf("  Tempo de execucao:     %.4f segundos\n", tempo_decorrido);
    
    if (total_primos_sequencial == total_primos_concorrente) {
        printf("\n  CORRETUDE VALIDADA\n\n");
    } else {
        printf("\n  ERRO: Resultados divergentes\n\n");
    }
    
    // Limpeza
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    free(canal.buffer);
    free(tids_consumidoras);
    free(dados_consumidores);
    
    return EXIT_SUCCESS;
}
