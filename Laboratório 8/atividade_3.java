/* 
Programação Concorrente - Laboratório 8
Aluno: Lucas Cavalcante Nascimento Silva
DRE: 121056863
Atividade 3

Calcula e exibe a quantidade de primos encontrados de 1 a N
Comando pra utilização: "java MyPool <N>"

*/

import java.util.LinkedList;
import java.lang.Math;
import java.util.concurrent.atomic.*;

//-------------------------------------------------------------------------------

// Classe da estrutura de dados (recurso) compartilhado entre as threads (por atomicidade)
class Sa {
  //recurso compartilhado
  private AtomicLong r;
  //construtor
  public Sa(long init) { 
     this.r = new AtomicLong(init); 
  }

  //operacao sobre o recurso compartilhado
  public long get() {
    return r.get();
  }

  //operacao sobre o recurso compartilhado
  public void inc(){
    this.r.getAndIncrement();  
  }
}

//-------------------------------------------------------------------------------
// Definindo o pool de Threads:
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

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

    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty() && shutdown) return;  
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------
// Classe de verificação de primalidade 
class ehPrimo implements Runnable {
    int n;
    private Sa s;

    // Construtor
    public ehPrimo(int i, Sa s){
        this.n = i;
        this.s = s;
    }

    // Função de verificação do primo (paralelizada):
    public int verifica(int n){
        int i;
        if(n<=1) return 0;
        if(n==2) return 1;
        if(n%2==0) return 0;
        for (i=3; i<Math.sqrt(n)+1; i+=2)
            if(n%i==0) return 0;
        return 1;
    }

    // Implementação do método run():
    public void run(){
        if(verifica(n)==1)  this.s.inc();
    }
}

//-------------------------------------------------------------------------------
// Classe Principal:
class PoolPrimo{

    private static final int NTHREADS = 10;

    public static void main(String args[]){

        int N, i;
        N = Integer.parseInt(args[0]);

        // Criando instância do recurso compartilhado entre as threads:
        Sa s = new Sa(0);

        // Criando pool de threads:
        FilaTarefas pool = new FilaTarefas(NTHREADS);

        for(i=0;i<=N;i++){
            Runnable r = new ehPrimo(i, s);
            pool.execute(r);
        }

        pool.shutdown();
        System.out.println(s.get());
    }
}