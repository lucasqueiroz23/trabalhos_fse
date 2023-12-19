#ifndef __SEMAFOROS_H__
#define __SEMAFOROS_H__

#include <stdbool.h>

// definição de variáveis para controle de semáforos 
#define DESLIGADO 0
#define VERDE 1
#define AMARELO 2
#define VERMELHO 3

// definição da duração, em segundos, de
// cada estado de um semáforo
#define TEMPO_VERDE_MIN_PRINCIPAL 10
#define TEMPO_VERDE_MAX_PRINCIPAL 20
#define TEMPO_VERMELHO_MIN_PRINCIPAL 5
#define TEMPO_VERMELHO_MAX_PRINCIPAL 10

#define TEMPO_VERDE_MIN_AUXILIAR 5
#define TEMPO_VERDE_MAX_AUXILIAR 10
#define TEMPO_VERMELHO_MIN_AUXILIAR 10
#define TEMPO_VERMELHO_MAX_AUXILIAR 20

#define TEMPO_AMARELO 2

// tempo de um segundo em milisec
#define UM_SEGUNDO 1000

// definição de identificadores dos semáforos
#define ID_SEMAFORO_PRINCIPAL 1
#define ID_SEMAFORO_AUXILIAR 2

#define BUZZER_1 17
#define BUZZER_2 21

/*´
 * Struct para controle de um semáforo.
 */
struct semaforo {
    short id;

    short pino_1;
    short pino_2;

    short sensor_1;
    short sensor_2;

    short estado_atual;

    unsigned tempo_decorrido;
    short tempo_minimo;
    short tempo_maximo;

    bool pedestre_fez_requisicao;
    bool modo_noturno;
    bool modo_emergencia;

    short buzzer;
};

typedef struct semaforo semaforo;

/**
 * Inicializar uma struct para controle de semáforo.
 * @params short pino1 O pino1, da GPIO, do semáforo.
 * @params short pino2 O pino2, da GPIO, do semáforo.
 * @params short id O identificador do semáforo, i.e., se ele é o 
 * da via principal ou da auxiliar.
 * @params short sensor_1 O sensor 1 da via.
 * @params short sensor_2 O sensor 2 da via.
 */
semaforo* inicializar_semaforo(short pino1, short pino2, short id, short sensor_1, short sensor_2);

/**
 * Alterar o estado de um semáforo.
 * @param struct semaforo* s O semáforo de interesse.
 * @param const short novo_estado O estado que o semáforo passará a ter.
 */
void alterar_estado_semaforo(semaforo* s, const short novo_estado);

/**
 * Passar o tempo mínimo em um dos três estados.
 * Se o estado mudou, então o pedestre não fez nenhuma requisição.
 * @param semaforo* s O semáforo que está sendo controlado pela thread.
 * @param const short novo_estado O estado que o semáforo passará a ter.
 */
void passar_tempo_minimo(semaforo* s, const short novo_estado);

/*
 * Espera, por polling, quanto tempo falta para amarelar, i.e., 
 * mudar o estado de AMARELO para VERDE.
 * Para que isso ocorra, basta que a diferença do tempo decorrido
 * e do tempo máximo no estado atual do concorrente seja maior do que
 * o tempo em que o estado atual fica amarelado.
 * @param semaforo* interesse O semáforo de interesse.
 * @param semaforo* concorrente O semáforo concorrente.
 * @param int buzzer O pino do buzzer.
 */
void esperar_para_amarelar(semaforo* interesse, semaforo* concorrente, int buzzer);

/**
 * Verificar se o timebox de um semáforo está sendo respeitado,
 * ou seja, se ele não passou do tempo máximo possível em um
 * estado.
 * @param semaforo* s O semáforo de interesse.
 */
bool timebox_semaforo_respeitado(semaforo* s);

/**
 * Verificar se o tempo mínimo em um estado do semáforo está
 * sendo respeitado.
 * @param semaforo* s O semáforo de interesse.
 */
bool tempo_minimo_respeitado(semaforo* s);

/**
 * Deixar um semáforo intermitente.
 * @param semaforo* interesse O semáforo de interesse.
 */
void intermitente(semaforo* interesse);

/**
 * Colocar um semáforo no modo de emergência.
 */
void emergencia(semaforo* interesse, semaforo* concorrente);

#endif

