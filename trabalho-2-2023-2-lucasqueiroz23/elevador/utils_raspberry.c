#include "utils_raspberry.h"
#include "wiringPi.h"
#include "softPwm.h"
#include <stdlib.h>
#include <stdbool.h>

// GPIO OUT
#define DIR1 20
#define DIR2 21
#define POTM 12

// GPIO IN
#define SENSOR_T 18
#define SENSOR_1 23
#define SENSOR_2 24
#define SENSOR_3 25


void inicializar_placa(int* entradas, int* saidas, int size_entradas, int size_saidas, int pino_pwm){
    // neste projeto, vamos utilizar a numeração da BCM. 
    wiringPiSetupGpio();

    for(int i = 0; i < size_entradas; i++) {
        pinMode(entradas[i], INPUT);
        pullUpDnControl(entradas[i], PUD_DOWN);
    }
    

    for(int i = 0; i < size_saidas; i++) 
        pinMode(saidas[i], OUTPUT);

    // obs: pino_pwm deve estar no array de saidas.
    softPwmCreate(pino_pwm, -100, 100);
}

void resetar_placa() {
    int saidas[3] = {DIR1, DIR2, POTM};
    int entradas[4] = {SENSOR_T, SENSOR_1, SENSOR_2, SENSOR_3};

    for(int i = 0; i < 3; i++) {
        pinMode(saidas[i], OUTPUT);
        pinMode(entradas[i], OUTPUT);
        digitalWrite(saidas[i], LOW);
        digitalWrite(entradas[i], LOW);
    }

    pinMode(entradas[3], OUTPUT);
    digitalWrite(entradas[3], LOW);

    exit(0);
}
