// implementação de uma fila. Obtida das minhas anotações de EDA1, quando
// peguei a matéria com o professor John Lenon, há anos.
#include "fila.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

Fila* cria_fila() {
    Fila* f = malloc(sizeof(Fila));

    f->size = 11;
    f->f = malloc(sizeof(int) * f->size);
    f->head = f->tail = 0;

    return f;
}

bool fila_esta_vazia(Fila* f) {
    return f->head == f->tail;
}

bool fila_esta_cheia(Fila* f) {
    return ((f->head == 0 && f->tail == f->size - 1) || (f->tail == f->head - 1));
}

int desenfileira(Fila* f, int* buffer) {
    if(fila_esta_vazia(f)){
     printf("fila vazia\n");
     *buffer = -1;
     return 1; // erro, fila vazia
    }
    *buffer = f->f[f->head];

    f->head = (f->head + 1) % f->size;
    return 0; // tudo certo
}


int enfileira(Fila* f, int val) {
    if(fila_esta_cheia(f)){
        printf("fila cheia\n");
        printf("impossível enfileirar %d\n", val);
        return 1; // fila tá cheia
    }
    
    f->f[f->tail] = val;
    f->tail = (f->tail + 1) % f->size;
    return 0; // tudo certo
}
