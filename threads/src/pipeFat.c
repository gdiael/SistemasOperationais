#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int read_int(int file_desc);
void write_int(int value, int file_desc);
int factorial(int n);

int calculator_pipe[2];
int client_pipe[2];

void read_factorials(int *A, int *B) {
    scanf("%d %d", A, B);
}

void init_pipes() {
    pipe(calculator_pipe);
    pipe(client_pipe);
}

void start_factorial_calculator() {
    if (fork() == 0) {
        // Processo filho
        close(calculator_pipe[1]);
        close(client_pipe[0]);

        int num;
        while ((num = read_int(calculator_pipe[0])) >= 0) {
            int result = factorial(num);
            write_int(result, client_pipe[1]);
        }

        close(calculator_pipe[0]);
        close(client_pipe[1]);
        exit(0);
    } else {
        // Processo pai
        close(calculator_pipe[0]);
        close(client_pipe[1]);
    }
}

int request_factorial(int num) {
    write_int(num, calculator_pipe[1]);
    return read_int(client_pipe[0]);
}

void finish_factorial_calculator() {
    write_int(-1, calculator_pipe[1]);
    close(calculator_pipe[1]);
    close(client_pipe[0]);
    wait(NULL);
}

void print_result(int A, int B, int fact_A, int fact_B) {
    printf("FILHO: !%d = %d\n", A, fact_A);
    printf("FILHO: !%d = %d\n", B, fact_B);
    printf("PAI: !%d + !%d = %d\n", A, B, fact_A + fact_B);
}

int main() {
    int A, B;

    read_factorials(&A, &B);
    init_pipes();
    start_factorial_calculator();

    int fact_A = request_factorial(A);
    int fact_B = request_factorial(B);

    finish_factorial_calculator();

    print_result(A, B, fact_A, fact_B);

    return 0;
}

int read_int(int file_desc) {
    int num = -1;
    while (read(file_desc, &num, sizeof(num)) <= 0);
    return num;
}

void write_int(int value, int file_desc) {
    write(file_desc, &value, sizeof(value));
}

int factorial(int n) {
    if (n <= 1)
        return 1;
    else
        return n * factorial(n - 1);
}
