#include "semaforos.h"
#include "wiringPi.h"
#include <stdio.h>
#include <stdlib.h>

semaforo* inicializar_semaforo(short pino1, short pino2, short id, short sensor_1, short sensor_2) {
    semaforo* novo = malloc(sizeof(semaforo));

    novo->id = id;

    novo->pino_1 = pino1;
    novo->pino_2 = pino2;

    novo->sensor_1 = sensor_1;
    novo->sensor_2 = sensor_2;

    novo->estado_atual = DESLIGADO;

    novo->tempo_decorrido = 0;
    novo->tempo_minimo = 0;
    novo->tempo_maximo = 0;

    novo->pedestre_fez_requisicao = false;
    novo->modo_noturno = false;
    novo->modo_emergencia = false;

    return novo;
}

void alterar_estado_semaforo(semaforo* s, const short novo_estado) {
    s->estado_atual = novo_estado;
    s->tempo_decorrido = 0;

    switch(novo_estado) {
        case DESLIGADO:
            digitalWrite(s->pino_1, LOW);
            digitalWrite(s->pino_2, LOW);
            s->tempo_minimo = 1;
            s->tempo_maximo = 1;
            break;

        case VERDE:
            digitalWrite(s->pino_1, LOW);
            digitalWrite(s->pino_2, HIGH);
            s->tempo_minimo = s->id == ID_SEMAFORO_PRINCIPAL ?
                   TEMPO_VERDE_MIN_PRINCIPAL :
                   TEMPO_VERDE_MIN_AUXILIAR;

            s->tempo_maximo = s->id == ID_SEMAFORO_PRINCIPAL ?
                   TEMPO_VERDE_MAX_PRINCIPAL :
                   TEMPO_VERDE_MAX_AUXILIAR;

            break;

        case AMARELO:
            digitalWrite(s->pino_1, HIGH);
            digitalWrite(s->pino_2, LOW);
            s->tempo_minimo = TEMPO_AMARELO;
            s->tempo_maximo = TEMPO_AMARELO;
            break;

        case VERMELHO:
            digitalWrite(s->pino_1, HIGH);
            digitalWrite(s->pino_2, HIGH);
            s->tempo_minimo = s->id == ID_SEMAFORO_PRINCIPAL ?
                   TEMPO_VERMELHO_MIN_PRINCIPAL :
                   TEMPO_VERMELHO_MIN_AUXILIAR;

            s->tempo_maximo = s->id == ID_SEMAFORO_PRINCIPAL ?
                   TEMPO_VERMELHO_MAX_PRINCIPAL :
                   TEMPO_VERMELHO_MAX_AUXILIAR;
            break;

        default:
            break;
    }
}

void passar_tempo_minimo(semaforo* s, const short novo_estado) {
    s->pedestre_fez_requisicao = false;
    alterar_estado_semaforo(s, novo_estado);
    while(s->tempo_decorrido < s->tempo_minimo) {
        delay(UM_SEGUNDO);
        s->tempo_decorrido++;
    }
}

bool timebox_semaforo_respeitado(semaforo* s) {
    return s->tempo_decorrido < s->tempo_maximo;
}

bool tempo_minimo_respeitado(semaforo* s) {
    return s->tempo_decorrido >= s->tempo_minimo;
}

void esperar_para_amarelar(semaforo* interesse, semaforo* concorrente, int buzzer) {
    while(
            !interesse->pedestre_fez_requisicao &&
            concorrente->tempo_decorrido - concorrente->tempo_maximo >= TEMPO_AMARELO && 
            timebox_semaforo_respeitado(interesse) && 
            timebox_semaforo_respeitado(concorrente) 
         ){
        delay(UM_SEGUNDO);
        interesse->tempo_decorrido++;
    }

    while(!tempo_minimo_respeitado(concorrente)){
        delay(UM_SEGUNDO);
        interesse->tempo_decorrido++;
    }
    digitalWrite(buzzer, HIGH);
}

void intermitente(semaforo* interesse) {
    while(interesse->modo_noturno) {
        passar_tempo_minimo(interesse, DESLIGADO);
        passar_tempo_minimo(interesse, AMARELO);
    }
}

void emergencia(semaforo* interesse, semaforo* concorrente) {
    if(interesse->id == ID_SEMAFORO_AUXILIAR) {
        if(interesse->estado_atual != VERMELHO) {
            passar_tempo_minimo(interesse, AMARELO);
        }
            alterar_estado_semaforo(interesse, VERMELHO);
            while(1);
    }

    if(interesse->id == ID_SEMAFORO_PRINCIPAL) {
        while(concorrente->estado_atual != VERMELHO);
        alterar_estado_semaforo(interesse, VERDE);
        while(1);
    }
}
