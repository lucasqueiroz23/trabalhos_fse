#ifndef __UTILS_RASPBERRY_H__
#define __UTILS_RASPBERRY_H__


/**
 * Inicializar a RaspberryPi.
 * @params int* entradas Vetor com os pinos de entrada.
 * @params int* saidas Vetor com os pinos de saida.
 * @params int size_entradas Tamanho do vetor de entradas.
 * @params int size_saidas Tamanho do vetor de saidas.
 * @params int pino_pwm Pino da gpio para PWM. Esse pino deve estar, também, no vetor de saídas.
 */
void inicializar_placa(int* entradas, int* saidas, int size_entradas, int size_saidas, int pino_pwm);

/**
 * Resetar a RaspberryPi. No caso, esta função simplesmente coloca os pinos
 * de entrada e/ou saída em nível baixo.
 */
void resetar_placa();
#endif
