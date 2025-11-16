/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Pool de threads com verificacao de primos */

import java.util.LinkedList;

//-------------------------------------------------------------------------------
// Contador thread-safe para acumular resultados
class Contador {
    private int total;

    public Contador() {
        this.total = 0;
    }

    public synchronized void adicionar(int valor) {
        total += valor;
    }

    public synchronized int getTotal() {
        return total;
    }
}

//-------------------------------------------------------------------------------
/**
 * Classe que implementa um pool de threads para execução de tarefas.
 * Gerencia uma fila de tarefas (Runnable) e um conjunto fixo de threads
 * que consomem e executam essas tarefas de forma concorrente.
 */
class FilaTarefas {
    private final int nThreads;              // Número de threads no pool
    private final MyPoolThreads[] threads;   // Array de threads trabalhadoras
    private final LinkedList<Runnable> queue; // Fila de tarefas a executar
    private boolean shutdown;                 // Flag para encerramento do pool

    /**
     * Construtor que inicializa o pool com um número fixo de threads.
     * @param nThreads número de threads no pool
     */
    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    /**
     * Adiciona uma tarefa à fila para execução.
     * @param r tarefa a ser executada
     */
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    /**
     * Encerra o pool de threads, aguardando a conclusão de todas as tarefas.
     */
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    /**
     * Classe interna que representa uma thread trabalhadora do pool.
     * Consome tarefas da fila e as executa.
     */
    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty()) return;   
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class Hello implements Runnable {
   String msg;
   public Hello(String m) { msg = m; }

   //--metodo executado pela thread
   public void run() {
      System.out.println(msg);
   }
}

class Primo implements Runnable {
   private int numero;
   private Contador contador;
   
   public Primo(int n, Contador c) { 
      this.numero = n;
      this.contador = c;
   }

   private int ehPrimo(int n) {
      if(n <= 1) return 0;
      if(n == 2) return 1;
      if(n % 2 == 0) return 0;
      for(int i = 3; i < Math.sqrt(n) + 1; i += 2) {
         if(n % i == 0) return 0;
      }
      return 1;
   }

   public void run() {
      int resultado = ehPrimo(numero);
      contador.adicionar(resultado);
   }
}

//-------------------------------------------------------------------------------
// Versao sequencial para comparacao
class Sequencial {
   private int ehPrimo(int n) {
      if(n <= 1) return 0;
      if(n == 2) return 1;
      if(n % 2 == 0) return 0;
      for(int i = 3; i < Math.sqrt(n) + 1; i += 2) {
         if(n % i == 0) return 0;
      }
      return 1;
   }

   public int contar(int limite) {
      int total = 0;
      for(int i = 0; i <= limite; i++) {
         total += ehPrimo(i);
      }
      return total;
   }
}

//Classe da aplicação (método main)
class ativ1 {
    public static void main (String[] args) {
      if(args.length < 2) {
         System.err.println("Uso: java ativ1 <num_threads> <limite>");
         System.exit(1);
      }

      int numThreads = Integer.parseInt(args[0]);
      int limite = Integer.parseInt(args[1]);

      // Versao concorrente
      long inicioConcorrente = System.currentTimeMillis();
      FilaTarefas pool = new FilaTarefas(numThreads);
      Contador contador = new Contador();
      
      for (int i = 0; i <= limite; i++) {
        pool.execute(new Primo(i, contador));
      }
      
      pool.shutdown();
      long fimConcorrente = System.currentTimeMillis();
      int totalConcorrente = contador.getTotal();

      // Versao sequencial
      long inicioSequencial = System.currentTimeMillis();
      Sequencial seq = new Sequencial();
      int totalSequencial = seq.contar(limite);
      long fimSequencial = System.currentTimeMillis();

      // Resultados
      System.out.println("\n=== RESULTADOS ===");
      System.out.println("Intervalo: 0 a " + limite);
      System.out.println("Threads: " + numThreads);
      System.out.println("\nSequencial: " + totalSequencial + " primos (" + (fimSequencial - inicioSequencial) + " ms)");
      System.out.println("Concorrente: " + totalConcorrente + " primos (" + (fimConcorrente - inicioConcorrente) + " ms)");
      
      // Verificacao de corretude
      if(totalSequencial == totalConcorrente) {
         System.out.println("\n[OK] Correto!");
         double speedup = (double)(fimSequencial - inicioSequencial) / (fimConcorrente - inicioConcorrente);
         System.out.printf("Speedup: %.2fx\n", speedup);
      } else {
         System.out.println("\n[ERRO] Divergente!");
         System.exit(1);
      }
   }
}
