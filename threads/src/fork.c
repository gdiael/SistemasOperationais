#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid;
    /* criar outro processo */
    pid = fork();
    if (pid < 0) { /* ocorreu um erro */
        fprintf(stderr, "Criação Falhou");
        exit(-1);
    } else if (pid == 0) {
        /* processo filho */
        printf("\nprocesso filho\n\n");
    } else {
        /* processo pai */
        printf("processo pai\n");
        printf("esperando pelo processo filho\n");
        wait(NULL);
        printf("fim do processo filho\n");
    }
}