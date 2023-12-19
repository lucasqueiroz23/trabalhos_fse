#include "sensores.h"
#include "semaforos.h"
#include <wiringPi.h>
#include <stdio.h>

float calcular_velocidade_carro(int pino){
    int count_tempo = 0;

    while(digitalRead(pino) == HIGH && count_tempo < 500) {
        delay(1);
        count_tempo++;
    }

    if(count_tempo >= 500) {
        // carro parou
        return 0.0;
    }

    const float tamanho_carro = 2.0;

    float velocidade_carro = 0.0;
    velocidade_carro = 3.6 * tamanho_carro/((float)count_tempo/1000.0);

    return velocidade_carro;
}

/**
 * Verificar se o andamento dos carros num semáforo está de acordo com o esperado.
 * Caso alguém avançe o sinal vermelho ou desrespeite os limites de velocidade, 
 * o servidor central será notificado.
 */
float verificar_andamento_semaforo(int pino, const int velocidade_maxima_via, semaforo* s, semaforo* concorrente) {
    float velocidade = calcular_velocidade_carro(pino);

    // carro parou
    if(velocidade == 0.0) {
        if(s->id == ID_SEMAFORO_AUXILIAR)
            concorrente->pedestre_fez_requisicao = true;
    }

    if(velocidade > velocidade_maxima_via) {
        // multa de velocidade
        printf("Carro passou a %.2f km/h\n", velocidade);
    }

    return velocidade;
}
