#ifndef __UTIL_SOCKETS_H__
#define __UTIL_SOCKETS_H__

#include "semaforos.h"

struct params_notificacao {
    char* mensagem;
};

typedef struct params_notificacao params_not;

/**
 * Notificar o servidor central.
 * @param char* mensagem A mensagem da notificação.
 */
void notificar_servidor_central(char* mensagem);

/**
 * Função para criação de thread que vai chamar a 
 * "notificar_servidor_central".
 */
void* notificar(void* args);

/**
 * Notificar o servidor central de que um carro passou 
 * no sinal vermelho.
 * @param semaforo* s O semáforo cujo qual o carro avançou o sinal.
 * @param int buzzer O buzzer.
 */
void notificar_sinal_vermelho(semaforo* s, int buzzer);

/**
 * Notificar o servidor central de um carro desrespeitou o
 * limite de velocidade.
 * @param int buzzer O buzzer, a ser acionado.
 */
void notificar_multa_velocidade(int buzzer);


/**
 * Receber a mensagem de um cliente.
 * @param int socket_cliente O socket do cliente.
 * @returns A mensagem recebida.
 */
char* receber_mensagem(int socket_cliente);


/**
 * Notificar o servidor central de que um carro passou.
 */
void notificar_passagem_carro(char* mensagem);
#endif
