/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <stdbool.h>

long int soma = 0; //variavel compartilhada entre as threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
pthread_cond_t cond_multiple; // sinaliza que um multiplo de 1000 foi alcancado
pthread_cond_t cond_resume;   // sinaliza que as threads podem continuar apos impressao
bool waiting_print = false;   // indica que ha um multiplo de 1000 a ser impresso
bool finished = false;        // indica que as threads de trabalho terminaram

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--aguarda ate que nao haja impressao pendente
     while (waiting_print) {
        pthread_cond_wait(&cond_resume, &mutex);
     }
     //--SC (seção critica)
     soma++; //incrementa a variavel compartilhada 
     if (soma % 1000 == 0) {
        waiting_print = true;
        pthread_cond_signal(&cond_multiple);
        while (waiting_print) {
           pthread_cond_wait(&cond_resume, &mutex);
        }
     }
     //--saida da SC
     pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");
  pthread_mutex_lock(&mutex);
  // Executa enquanto houver trabalho a ser feito ou alguma impressao pendente
  while (!finished || waiting_print) {
     while (!waiting_print && !finished) {
        pthread_cond_wait(&cond_multiple, &mutex);
     }
     if (waiting_print) {
        // 'soma' aqui e multiplo de 1000
        printf("soma = %ld \n", soma);
        waiting_print = false;
        // libera as threads de trabalho para continuar
        pthread_cond_broadcast(&cond_resume);
     }
     // se 'finished' ficou true e nao ha impressao pendente, sai do loop
  }
  pthread_mutex_unlock(&mutex);
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex e as variaveis de condicao
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond_multiple, NULL);
   pthread_cond_init(&cond_resume, NULL);

   //--cria thread de log primeiro
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--cria as threads de trabalho
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--espera as threads de trabalho terminarem
   for (int t=0; t<nthreads; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   }

   //--sinaliza para a thread extra que terminou
   pthread_mutex_lock(&mutex);
   finished = true;
   pthread_mutex_unlock(&mutex);
   pthread_cond_broadcast(&cond_multiple);

   //--espera a thread extra terminar
   if (pthread_join(tid[nthreads], NULL)) {
       printf("--ERRO: pthread_join() \n"); exit(-1); 
   }

   //--finaliza as variaveis de sincronizacao
   pthread_cond_destroy(&cond_multiple);
   pthread_cond_destroy(&cond_resume);
   pthread_mutex_destroy(&mutex);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
