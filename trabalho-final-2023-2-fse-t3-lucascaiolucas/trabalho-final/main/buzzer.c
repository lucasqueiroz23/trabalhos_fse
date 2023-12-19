#include "buzzer.h"
#include "freertos/portmacro.h"
#include "gpio_setup.h"
void ativar_buzzer() {
    digitalWrite(BUZZER, 1);
}

void desativar_buzzer() {
    digitalWrite(BUZZER, 0);
}
