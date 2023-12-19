#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

pid_t pid_cruzamento_1;
pid_t pid_cruzamento_2;

void handler(int sig) {
    printf("fechando processo\n");
    kill(pid_cruzamento_1, SIGINT);
    kill(pid_cruzamento_2, SIGINT);
    exit(0);
}

int main() {

    signal(SIGINT, &handler);



    printf("abrindo cruzamento 1\n");
    if((pid_cruzamento_1 = fork()) == 0){
        execlp("./cruzamento", "./cruzamento","--c1", NULL);

    } else {
        printf("abrindo cruzamento 2\n");
        if((pid_cruzamento_2 = fork()) == 0) {
            execlp("./cruzamento", "./cruzamento", "--c2", NULL);
        }

        while(1);
    }

    exit(0);
}
