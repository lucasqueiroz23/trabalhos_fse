#ifndef __FILA_H__
#define __FILA_H__
#include <stdbool.h>

/**
 * Struct que define uma fila.
 */
struct Fila {
    int* f; // vetor da fila em si.
    int head; // cabeça da fila
    int tail; // rabo da fila
    int size; // tamanho da fila
};
typedef struct Fila Fila;

/**
 * Cria e retorna uma fila. Nesse caso, o tamanho da fila é
 * fixo em 11, que é o conjunto de endereços lidos.
 */
Fila* cria_fila();

/**
 * Enfileira um item na fila.
 * @params Fila* f - Ponteiro para a fila em questão.
 * @params int val - Valor a ser inserido na fila.
 */
int enfileira(Fila* f, int val);

/**
 * Desenfileira a fila e guarda o valor numa variável.
 * @params Fila* f - Ponteiro para a fila em questão.
 * @params int* buffer - Ponteiro para a variável que guarda o valor do head.
 */
int desenfileira(Fila* f, int* buffer);

/**
 * Verifica se uma fila está cheia.
 * @params Fila* f - Ponteiro para a fila em questão.
 */
bool fila_esta_vazia(Fila* f);

/**
 * Verifica se uma fila está vazia.
 * @params Fila* f - Ponteiro para a fila em questão.
 */
bool fila_esta_cheia(Fila* f);

#endif

