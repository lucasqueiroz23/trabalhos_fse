#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/portmacro.h"
#include "alarme.h"
#include "gpio_setup.h"
#include "buzzer.h"


void ativar_alarme() {
    digitalWrite(ALARME_BLUE, 0);
    digitalWrite(ALARME_RED, 1);

    digitalWrite(FOTO_BLUE, 0);
    digitalWrite(FOTO_RED, 0);
    digitalWrite(FOTO_GREEN, 0);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    digitalWrite(FOTO_BLUE, 1);
    digitalWrite(FOTO_RED, 1);
    digitalWrite(FOTO_GREEN, 1);

    vTaskDelay(400 / portTICK_PERIOD_MS);

    digitalWrite(ALARME_BLUE, 1);
    digitalWrite(ALARME_RED, 0);
    
    digitalWrite(FOTO_BLUE, 0);
    digitalWrite(FOTO_RED, 0);
    digitalWrite(FOTO_GREEN, 0);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    digitalWrite(FOTO_BLUE, 1);
    digitalWrite(FOTO_RED, 1);
    digitalWrite(FOTO_GREEN, 1);

    vTaskDelay(400 / portTICK_PERIOD_MS);
}

void desativar_alarme() {
    digitalWrite(FOTO_BLUE, 1);
    digitalWrite(FOTO_RED, 1);
    digitalWrite(FOTO_GREEN, 1);

    digitalWrite(ALARME_BLUE, 1);
    digitalWrite(ALARME_GREEN, 1);
    digitalWrite(ALARME_RED, 1);
}

/**
 * Ativar o sistema de segurança, ou seja, ativar o buzzer e manter o alarme piscando.
 */
void ativar_sistema_de_seguranca(void* params) {
    ativar_buzzer();
    while(1) {
        ativar_alarme();
    }
}

