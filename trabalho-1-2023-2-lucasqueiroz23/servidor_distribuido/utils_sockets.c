#include <pthread.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include "semaforos.h"
#include "utils_sockets.h"
#include "utils_file.h"

void notificar_servidor_central(char* mensagem) {

    // abrir socket
    char* texto_arquivo = obter_texto_arquivo();
    char* ip_servidor_central = obter_ip_servidor(texto_arquivo);

    int cliente_socket;
    struct sockaddr_in endereco_central;

    if((cliente_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Falha ao abrir socket\n");
        return;
    }

    int porta_servidor_central = obter_porta_servidor(texto_arquivo);

    memset(&endereco_central, 0, sizeof(endereco_central));
    endereco_central.sin_family = AF_INET;
    endereco_central.sin_addr.s_addr = inet_addr(ip_servidor_central);
    endereco_central.sin_port = htons(porta_servidor_central);

    if(connect(cliente_socket, (struct sockaddr*) &endereco_central, sizeof(endereco_central)) < 0){

        printf("Erro ao conectar cruzamento com o servidor central\n");
        return;
    }

    unsigned tamanho_mensagem = strlen(mensagem);

    if(send(cliente_socket, mensagem, tamanho_mensagem, 0) != tamanho_mensagem){

        printf("Erro ao enviar mensagem");
        return;
    }
    close(cliente_socket);
}

void* notificar(void* args) {
    params_not* p = (params_not*) args;
    notificar_servidor_central(p->mensagem);
    return NULL;
}

void notificar_sinal_vermelho(semaforo* s, int buzzer) {
    if(s->estado_atual == VERMELHO) {
        printf("Avançou no sinal vermelho\n");
        digitalWrite(buzzer, HIGH);
        params_not* p = malloc(sizeof(params_not));
        p->mensagem = "multa_vermelho";
        pthread_t thread;
        pthread_create(&thread, NULL, notificar, p);
        pthread_join(thread, NULL);
        delay(1000);
        digitalWrite(buzzer, LOW);
    }
}

void notificar_multa_velocidade(int buzzer) {
    params_not* p = malloc(sizeof(params_not));
    digitalWrite(buzzer, HIGH);
    p->mensagem = "multa_velocidade";
    pthread_t thread;
    pthread_create(&thread, NULL, notificar, p);
    pthread_join(thread, NULL);
    delay(1000);
    digitalWrite(buzzer, LOW);
}

char* receber_mensagem(int socket_cliente) {
    const int TAM_BUFFER = 20;

    char mensagem_recebida[TAM_BUFFER];
    int tam_recebido = 0;

    char* mensagem_tratada;

    if((tam_recebido = recv(socket_cliente, mensagem_recebida, TAM_BUFFER, 0)) < 0)
        printf("Erro no recv\n");

    mensagem_tratada = malloc(sizeof(char) * tam_recebido);
    strncpy(mensagem_tratada, mensagem_recebida, tam_recebido);

    return mensagem_tratada;
}

void notificar_passagem_carro(char* mensagem) {
    params_not* p = malloc(sizeof(params_not));
    p->mensagem = mensagem;
    pthread_t thread;
    pthread_create(&thread, NULL, notificar, p);
    pthread_join(thread, NULL);
}
