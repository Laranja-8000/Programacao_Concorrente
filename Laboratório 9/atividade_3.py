# Programação Concorrente - Laboratório 9
# Aluno: Lucas Cavalcante Nascimento Silva
# DRE: 121056863
# Atividade 3

N = 1000 # Limite da sequência de 1 a N

import math
import concurrent.futures

# Função de verificação de primalidade a ser paralelizada (usando FUTUROS):
def ehPrimo(n):
    if n<=1: return 0
    if n==2: return 1
    if n%2==0: return 0
    for i in range(3,int(math.sqrt(n)+1),2):
        if n%i == 0: return 0
    return 1

# Thread principal:
if __name__ == '__main__':

    #cria um pool de threads OU de processos com no maximo 5 intancias 
    with concurrent.futures.ProcessPoolExecutor(max_workers=5) as executor:
        futures = []
        
        #submete a tarefa para execucao assincrona
        for aux in range(N):
            futures.append(executor.submit(ehPrimo, aux))

        #recebe os resultados
        result = 0
        for future in futures:
            result += future.result()
        print("Primos de 1 a "+str(N)+": "+str(result))
