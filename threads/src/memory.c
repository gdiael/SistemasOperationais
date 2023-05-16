#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void fazerSoma(int A, int B, int shared_mem_id);
void fazerMultiplicacao(int A, int B, int shared_mem_id);
int calcularResultadoFinal(int shared_mem_id);

int main(){
    printf("DIGITE UM PAR DE INTEIROS: ");
    int A, B;
    scanf("%d %d", &A, &B);

    size_t size_result = sizeof(int) * 2;

    /**
        1. Crie o espaço de memória compartilhado para escrita dos resultados.
    */
    int shared_mem_id = shmget(IPC_PRIVATE, size_result, IPC_CREAT | 0666);

    /**
        2. Crie filho 1
    */
    pid_t pid_filho1 = fork();

    /**
        3. Se for filho 1
    */
    if(pid_filho1 == 0){
        /**
            4. Implemente a função do filho 1
        */
        fazerSoma(A, B, shared_mem_id);
        return 0; //finaliza filho 1
    }
    
    //processo pai ---------

    /**
        5. Espere pelo filho 1 terminar
    */
    wait(NULL);

    /**
        6. Crie filho 2
    */
    pid_t pid_filho2 = fork();
    
    /**
        7. Se for filho 2
    */    
    if(pid_filho2 == 0){
        /**
            8. Implemente a função do filho 2
        */        
        fazerMultiplicacao(A, B, shared_mem_id);
        return 0; //finaliza filho 2
    }
    
    //processo pai ---------

    /**
        9. Espere pelo filho 2 terminar
    */
    wait(NULL);

    /**
        10. Faça a leitura dos resultados dos filhos e imprima resultado
    */
    int resultado = calcularResultadoFinal(shared_mem_id);
    printf("RESULTADO: %d\n", resultado);

    //Marca memória compartilhada para ser desalocada
    shmctl(shared_mem_id, IPC_RMID, NULL);

    return 0;
}


void fazerSoma(int A, int B, int shared_mem_id){
    /**
        Obter acesso a memória compartilhada (anexar)
    */
    int *resultado = (int *)shmat(shared_mem_id, NULL, 0);

    //calcula o resultado
    resultado[0] = A + B;
    printf("FILHO 1: %d + %d = %d\n", A, B, *resultado);

    /**
        Liberar acesso a memória compartilhada (desanexar)
    */
    shmdt(resultado);
}


void fazerMultiplicacao(int A, int B, int shared_mem_id){
    /**
        Obter acesso a memória compartilhada (anexar)
    */
    int *resultado = (int *)shmat(shared_mem_id, NULL, 1);

    //calcula o resultado
    resultado[1] = A * B;
    printf("FILHO 2: %d * %d = %d\n", A, B, resultado[1]);

    /**
        Liberar acesso a memória compartilhada (desanexar)
    */
    shmdt(resultado);
}

int calcularResultadoFinal(int shared_mem_id){
    /**
        Obter acesso a memória compartilhada (anexar)
    */
    int *resultado = (int *)shmat(shared_mem_id, NULL, 0);

    /** Receber resultados dos filhos*/  	
    int filho1 = resultado[0];
    int filho2 = resultado[1];

    int resultado_final = filho1 + filho2;
    printf("PAI: %d + %d = %d\n", filho1, filho2, resultado_final);

    /**
        Liberar acesso a memória compartilhada (desanexar)
    */
    shmdt(resultado);

    return resultado_final;
}