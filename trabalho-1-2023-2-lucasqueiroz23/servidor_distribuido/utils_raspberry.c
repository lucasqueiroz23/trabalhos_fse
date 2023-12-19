#include "utils_raspberry.h"
#include "wiringPi.h"
#include <stdlib.h>
#include <stdbool.h>

#define CRUZAMENTO_1 1
#define CRUZAMENTO_2 2
#define SIZE_ENTRADAS 2
#define SIZE_SAIDAS 5


void inicializar_placa(int* entradas, int* saidas, int size_entradas, int size_saidas){
    // neste projeto, vamos utilizar a numeração da BCM. 
    wiringPiSetupGpio();

    for(int i = 0; i < size_entradas; i++) {
        pinMode(entradas[i], INPUT);
        pullUpDnControl(entradas[i], PUD_DOWN);
    }
    

    for(int i = 0; i < size_saidas; i++) 
        pinMode(saidas[i], OUTPUT);
    
}

void resetar_cruzamento(int cruzamento) {
    int* saidas;
    int* entradas;

    int saidas_cruzamento1[SIZE_SAIDAS] = {9, 11, 5, 6, 17};
    int saidas_cruzamento2[SIZE_SAIDAS] = {10, 8, 1, 18, 21};

    int entradas_cruzamento_1[SIZE_ENTRADAS] = {13, 19};
    int entradas_cruzamento_2[SIZE_ENTRADAS] = {23, 24};

    if(cruzamento == CRUZAMENTO_1) {
        saidas = saidas_cruzamento1;
        entradas = entradas_cruzamento_1;
    } else if(cruzamento == CRUZAMENTO_2) {
        saidas = saidas_cruzamento2;
        entradas = entradas_cruzamento_2;
    }

    for(int i = 0; i < SIZE_SAIDAS; i++) {
        pinMode(saidas[i], OUTPUT);
        digitalWrite(saidas[i], LOW);
    }

    for(int i = 0; i < SIZE_ENTRADAS; i++){
        pinMode(entradas[i], OUTPUT);
        digitalWrite(entradas[i], LOW);
    }
}

void resetar_placa() {
    int pinos[28] = {2, 3, 4, 17, 27, 22, 10, 9, 11, 0, 5, 6, 13, 19, 26, 14, 15, 18, 23, 24, 25, 8, 7, 1, 12, 16, 20, 21};

    for(int i = 0; i < 28; i++) {
        pinMode(pinos[i], OUTPUT);
        digitalWrite(pinos[i], LOW);
    }

    exit(0);
}
