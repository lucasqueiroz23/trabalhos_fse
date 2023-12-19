#ifndef __SENSORES_H__
#define __SENSORES_H__

#include "semaforos.h"

/**
 * Calcular a velocidade de um carro.
 * @param int pino O pino que identifica qual a via
 * que o carro passou.
 */
float calcular_velocidade_carro(int pino);

/**
 * Verificar se o andamento dos carros num semáforo está de acordo com o esperado.
 * Caso alguém avançe o sinal vermelho ou desrespeite os limites de velocidade, 
 * o servidor central será notificado.
 */
float verificar_andamento_semaforo(int pino, const int velocidade_maxima_via, semaforo* s, semaforo* concorrente);

#endif

