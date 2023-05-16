#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

void executarBusca(int shared_mem_id, int X, int N, int * valores);
void filho_busca(int shmid, int filhoIdx, int X, int startIdx, int endIdx, int * valores);
void imprimirResultados(int shared_mem_id);

int* read_nums(int count);
int busca(int X, int startIdx, int endIdx, int * valores);

int main(){
    int X, N;

    scanf("%d %d", &X, &N);

    //Lê valores da entrada padrão
    int * valores = read_nums(N);

    int result_size = sizeof(int) * 2;

    /**
        1. Crie o espaço de memória para escrita dos resultados.
    */
    int shmid = shmget(IPC_PRIVATE, result_size, IPC_CREAT | 0666);

    /**
        2. Complete busca com subprocessos
    */
    executarBusca(shmid, X, N, valores);

    /**
        3. Espere pelos filhos terminarem
    */
    wait(NULL);
    wait(NULL);

    /**
        4. Imprima resultados dos filhos
    */
    imprimirResultados(shmid);

    //Marca memória compartilhada para ser desalocada
    shmctl(shmid, IPC_RMID, NULL);
    // delete[] valores;

    return 0;
}

void executarBusca(int shared_mem_id, int X, int N, int * valores){
    /**
        - Crie filho 1
    */
    pid_t pid1 = fork();

    /**
        - Se for filho 1
    */
    if(pid1 == 0){//filho 1
        /**
            - Faça busca na primeira metade dos valores
        */
        filho_busca(shared_mem_id, 0, X, 0, N/2, valores);
        exit(0);
    }

    /**
        - Crie filho 2
    */
    pid_t pid2 = fork();

    /**
        - Se for filho 2
    */
    if(pid2 == 0){//filho 2
        /**
            - Faça busca na segunda metade dos valores
        */
        filho_busca(shared_mem_id, 1, X, N/2 + 1, N, valores);
        exit(0);
    }
}

void filho_busca(int shmid, int filhoIdx, int X, int startIdx, int endIdx, int * valores){
    /**
        Obter acesso a memória compartilhada (anexar)
    */
    int *out = (int *)shmat(shmid, NULL, 0);

    //busca e escreve resultado
    int found = busca(X, startIdx, endIdx, valores);
    out[filhoIdx] = found;

    /**
        Liberar memória compartilhada (desanexar)
    */
    shmdt(out);
}

void imprimirResultados(int shared_mem_id){
    /**
        Obter acesso a memória compartilhada (anexar)
    */
    int *out = (int *)shmat(shared_mem_id, NULL, 0);

    int foundIdx = -1;

    for(int i = 0; i < 2; ++i){
        printf("Filho %d: %d\n", i+1, out[i]);

        if(foundIdx < 0 && out[i] >= 0){
            foundIdx = out[i];
        }
    }

    printf("PAI: %d\n", foundIdx);

    /**
        Liberar memória compartilhada (desanexar)
    */
    shmdt(out);
}

int* read_nums(int count){
    int *outNums = (int *)malloc(sizeof(int) * count);

    for(int i = 0; i < count; ++i){
        scanf("%d", &outNums[i]);
    }

    return outNums;
}


int busca(int X, int startIdx, int endIdx, int * valores){
    for(int i=startIdx; i <= endIdx; ++i){
        if(valores[i] == X){
            return i;
        }
    }
    
    return -1;
}
