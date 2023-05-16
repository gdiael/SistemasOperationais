#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int* read_nums(int count);

int read_int_from(int pipe_files[2]);
void send_int_to(int value, int pipe_files[2]);

int read_int(int file_desc);
void write_int(int value, int file_desc);


int pipe_filho[2];
int pipe_pai[2];

int main(int argc, char * argv[])
{

    int N=0;

    scanf("%d", &N);
    
    int * valores = read_nums(N);

    /**
        1. Inicializar pipes
    */
    if (pipe(pipe_filho) == -1) {
        perror("Erro ao criar o pipe do filho");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe_pai) == -1) {
        perror("Erro ao criar o pipe do pai");
        exit(EXIT_FAILURE);
    }

    /*
        2. Criar processo filho
    */
    pid_t pid = fork();

    if (pid == -1) {
        perror("Erro ao criar o processo filho");
        exit(EXIT_FAILURE);
    }
    
    /* 3. Se for filho. */
    if (pid == 0){//filho
        int value = -2;
        
        /* 4. Ler valores do pipe até receber -1. */
        while (value != -1) {
            value = read_int_from(pipe_filho);
            printf("FILHO: %d\n", value);
            
            //Flush garante que saída vai ser impressa na tela antes da próxima instrução
            fflush(stdout);

            if (value >= 0) { // Não é finalizador
                //retorna valor incrementado
                send_int_to(value + 1, pipe_pai);
            }
        }

        // finaliza processo filho
        exit(0);
    }

    //processo pai ----------

    /* 5. Enviar os N valores para o processo filho. */
    for (int i = 0; i < N; ++i) {
        // Envia valor para o filho
        send_int_to(valores[i], pipe_filho);
        
        // Recebe e imprime resultado do filho
        int resultado = read_int_from(pipe_pai);
        printf("PAI: %d\n", resultado);

        //Flush garante que saída vai ser impressa na tela antes da próxima instrução
        fflush(stdout);
    }

    /* 6. Finalizar processo filho (enviar -1). */
    send_int_to(-1, pipe_filho);

    /* 7. Esperar filho finalizar. */
    wait(NULL);

    // libera array alocado por read_nums
    free(valores);

    return 0;
}



//-------------------------------------------------


void send_int_to(int value, int pipe_files[2]){
    /* Escrever valor no pipe (resultado será lido na outra extremidade do pipe). */
    write_int(value, pipe_files[1]);
}

int read_int_from(int pipe_files[2]){
    /* Ler valor do pipe (escrito por outro processo na outra extremidade oposta do pipe). */
    return read_int(pipe_files[0]);
}


int read_int(int file_desc){
    int num = -1;    
    while (read(file_desc, &num, sizeof(num)) <= 0);

    return num;
}

void write_int(int value, int file_desc){
    write(file_desc, &value, sizeof(value));
}


// Auxiliares --------------------------------------------

/**
    Lê N valores da entrada padrão e retorna um array com os N valores.
*/
int* read_nums(int N) {
    int num = -1;
    int* outNums = (int*)malloc(N * sizeof(num));

    for (int i = 0; i < N; ++i) {
        scanf("%d", &outNums[i]);
    }

    return outNums;
}