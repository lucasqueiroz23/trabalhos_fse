#ifndef __UTILS_RASPBERRY_H__
#define __UTILS_RASPBERRY_H__


/**
 * Inicializar a RaspberryPi.
 * @params int* entradas Vetor com os pinos de entrada.
 * @params int* saidas Vetor com os pinos de saida.
 * @params int size_entradas Tamanho do vetor de entradas.
 * @params int size_saidas Tamanho do vetor de saidas.
 */
void inicializar_placa(int* entradas, int* saidas, int size_entradas, int size_saidas);

/**
 * Resetar a RaspberryPi. No caso, esta função simplesmente coloca os pinos
 * de entrada e/ou saída em nível baixo.
 */
void resetar_placa();

/**
 * Resetar um cruzamento.
 * @param int cruzamento O cruzamento em questão.
 */
void resetar_cruzamento(int cruzamento);
#endif
