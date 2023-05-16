#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int read_int_from(int pipe_files[2]);
void send_int_to(int value, int pipe_files[2]);

int read_int(int file_desc);
void write_int(int value, int file_desc);


int pipe_filho[2];
int pipe_pai[2];

int main(int argc, char * argv[])
{

    int A=0;

    scanf("%d", &A);
    
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
    if (pid == 0) {//filho
        //lê valor
        int value = read_int_from(pipe_filho);
        printf("FILHO: %d\n", value);

        //Flush garante que saída vai ser impressa na tela antes da próxima instrução
        fflush(stdout);

        //retorna valor incrementado
        send_int_to(value + 1, pipe_pai);
        exit(0);
    }

    //processo pai ----------

    // Envia valor para o filho
    send_int_to(A, pipe_filho);

    // Recebe e imprime resultado do filho
    int resultado = read_int_from(pipe_pai);
    printf("PAI: %d\n", resultado);

    // Aguarda o término do filho
    wait(NULL);

    return 0;
}



//-------------------------------------------------


void send_int_to(int value, int pipe_files[2]){
    write_int(value, pipe_files[1]);
}

int read_int_from(int pipe_files[2]){
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
