#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "freertos/portmacro.h"
#include "gpio_setup.h"
#include "dht11.h"
#include "alarme.h"
#include "buzzer.h"

// pino do botão que ativa/desativa o sistema
#define BOTAO 19

// pino do sensor de efeito HALL
#define HALL 35

// pino para conversa com sensor de temperatura DHT11
#define TEMP 4

void ler_temperatura(void* params) {
    DHT11_init(TEMP);
    while(true) {
        printf("Temperatura: %d\n", DHT11_read().temperature);
        printf("Umidade: %d\n", DHT11_read().humidity);
        printf("Status: %d\n", DHT11_read().status);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t t_ativacao_sistema;
bool task_criada = false;


/**
 * Inicializar os pinos de GPIO para conversa com sensores.
 */
void inicializar_gpio() {
    // pino para o sensor de efeito hall
    pinMode(HALL, GPIO_INPUT);

    // pino do buzzer
    pinMode(BUZZER, GPIO_OUTPUT);

    // pinos do alarme
    pinMode(ALARME_GREEN, GPIO_OUTPUT);
    pinMode(ALARME_RED, GPIO_OUTPUT);
    pinMode(ALARME_BLUE, GPIO_OUTPUT);

    // pinos da foto
    pinMode(FOTO_RED, GPIO_OUTPUT);
    pinMode(FOTO_GREEN, GPIO_OUTPUT);
    pinMode(FOTO_BLUE, GPIO_OUTPUT);

    // inicializar os pinos de alarme e foto como desligados.
    digitalWrite(ALARME_GREEN, 1);
    digitalWrite(ALARME_BLUE, 1);
    digitalWrite(ALARME_RED, 1);
    digitalWrite(FOTO_BLUE, 1);
    digitalWrite(FOTO_GREEN, 1);
    digitalWrite(FOTO_RED, 1);

    // inicializar o pino do botão de GPIO como pullup.
    pinMode(BOTAO, GPIO_INPUT_PULLUP);
}

void app_main() {
    xTaskCreate(ler_temperatura, "leitura de temperatura", 3000, "task leitura temp", 1, NULL);
    inicializar_gpio();

    bool button_pressed = false;

    int switch_ativacao_sistema = 0;
      // Testa o Botão utilizando polling
    while (true) {
        if(!digitalRead(BOTAO)) {
            button_pressed = true;
            // tratar debounce
            vTaskDelay(200 / portTICK_PERIOD_MS);
        } else button_pressed = false;

        if(button_pressed){
            switch_ativacao_sistema++;
            printf("botao foi apertado %d vezes \n", switch_ativacao_sistema);
        } 

        if(switch_ativacao_sistema % 2 == 1){
            if(task_criada)
                vTaskDelete(t_ativacao_sistema);
            task_criada = false;
            desativar_buzzer();
            desativar_alarme();
            continue;   
        }

        if(digitalRead(HALL)) {
            if(!task_criada) {
                task_criada = true;
                xTaskCreate(ativar_sistema_de_seguranca, "ativacao de sistema de seguranca", 3000, NULL, 1, &t_ativacao_sistema);
            }
        } else {
            if(task_criada)
                vTaskDelete(t_ativacao_sistema);
            task_criada = false;
            desativar_buzzer();
            desativar_alarme();
        }
    }  
}

