/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.ArrayList;
import java.util.List;

// Classe para verificar primos de forma sequencial
class PrimoSequencial {
  private int numero_final;
  
  PrimoSequencial(int numero_final) {
    this.numero_final = numero_final;
  }
  
  // Verifica se um numero eh primo
  private int  ehPrimo(int numero) {
    if (numero <= 1) return 0;
    if (numero == 2) return 1;
    if (numero % 2 == 0) return 0;
    
    for (int i = 3; i < Math.sqrt(numero) + 1; i += 2) {
      if (numero % i == 0) return 0;
    }
    return 1;
  }
  
  // Conta primos sequencialmente
  public int contaPrimos() {
    int quantidade = 0;
    for (int i = 1; i <= numero_final; i++) {
      quantidade +=  ehPrimo(i);
    }
    return quantidade;
  }
}

// Classe callable para contar primos em um intervalo
class ContadorPrimos implements Callable<Long> {
  private int inicio;
  private int fim;
  
  ContadorPrimos(int inicio, int fim) {
    this.inicio = inicio;
    this.fim = fim;
  }
  
  // Verifica se um numero eh primo
  private int  ehPrimo(int numero) {
    if (numero <= 1) return 0;
    if (numero == 2) return 1;
    if (numero % 2 == 0) return 0;
    
    for (int i = 3; i < Math.sqrt(numero) + 1; i += 2) {
      if (numero % i == 0) return 0;
    }
    return 1;
  }
  
  // Conta primos no intervalo
  public Long call() throws Exception {
    long quantidade = 0;
    for (int numero = inicio; numero <= fim; numero++) {
      quantidade +=  ehPrimo(numero);
    }
    return quantidade;
  }
}

public class ativ3 {
  public static void main(String[] args) {
    
    if (args.length < 2) {
      System.err.println("Para rodar: java ativ3 <N> <NTHREADS>");
      return;
    }
    
    // Le parametros
    int N = Integer.parseInt(args[0]);
    int NTHREADS = Integer.parseInt(args[1]);
    
    // Cria objeto para versao sequencial
    PrimoSequencial primoSeq = new PrimoSequencial(N);
    
    // Cria pool de threads
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    List<Future<Long>> lista = new ArrayList<Future<Long>>();
    
    // Divide trabalho entre threads
    int tamanho_pedaco = N / NTHREADS;
    int inicio, fim;
    
    for (int i = 0; i < NTHREADS; i++) {
      inicio = i * tamanho_pedaco + 1;
      fim = (i == NTHREADS - 1) ? N : (i + 1) * tamanho_pedaco;
      
      Callable<Long> worker = new ContadorPrimos(inicio, fim);
      Future<Long> resultado = executor.submit(worker);
      lista.add(resultado);
    }
    
    // Coleta resultados
    long total_primos = 0;
    for (Future<Long> future : lista) {
      try {
        total_primos += future.get();
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    
    executor.shutdown();
    
    // Compara com versao sequencial
    int primos_sequencial = primoSeq.contaPrimos();
    
    System.out.println("Sequencial: Entre 1 e " + N + " ha " + primos_sequencial + " numeros primos");
    System.out.println("Concorrente: Entre 1 e " + N + " ha " + total_primos + " numeros primos");
  }
}
