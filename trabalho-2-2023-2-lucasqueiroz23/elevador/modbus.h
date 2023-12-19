#ifndef __MODBUS_H__
#define __MODBUS_H__


struct InteracaoRegistradores {
    const unsigned char endereco_reg;
    const unsigned char qtd_bytes;
    unsigned char* bytes;
};

typedef struct InteracaoRegistradores InteracaoRegistradores;

/**
 * Abrir comunicação, via open(), com a UART.
 * Retorna o inteiro que aponta para a UART.
 */
int abrir_comunicacao_uart();

/**
 * Enviar dados na Tx da UART. No caso, essa função deve ser usada para envio de
 * sinal de controle PWM, temperatura ambiente e leitura e escrita nos registradores. 
 * @params int uart File descriptor da UART.
 * @params char codigo Codigo de envio de dados.
 * @params char subcodigo Subcodigo de envio de dados.
 * @params void* dados Os dados em si. Deve ser um endereço de um inteiro ou de um float.
 */
void enviar_dados(int uart, unsigned char codigo, unsigned char subcodigo, void* dados);

/**
 * Ler o valor do encoder.
 * @params int fd_uart - O file descriptor da UART.
 */
int ler_encoder(int fd_uart);


/**
 * Interagir com a UART.
 * @params unsigned char codigo - Codigo do envio da mensagem MODBUS.
 * @params unsigned char subcodigo - Subcodigo do envio da mensagem MODBUS.
 * @params dados - Dados de envio da mensagem MODBUS.
 * @params callback - Função chamada na interação da UART.
 */
void interagir_UART(unsigned char codigo, unsigned char subcodigo, void* dados, void(*callback)(int, unsigned char, unsigned char, void*));

/**
 * Envia, pela uart, um sinal de pwm.
 * @params int sinal - O sinal desejado.
 */
void enviar_sinal_pwm(int sinal);

/**
 * Envia, pela uart, um sinal de temperatura.
 * @params int sinal - O sinal desejado.
 */
void enviar_sinal_temperatura(float sinal);

/**
 * Lê, pela uart, o valor de um registrador.
 * @params unsigned char endereco - O endereço do registrador.
 * @params unsigned char qtd - A quantidade de bytes a ser lida.
 */
InteracaoRegistradores ler_registrador(unsigned char endereco, unsigned char qtd);

/**
 * Escrever num registrador. Nesse caso, obrigatoriamente, o que está sendo
 * escrito vai colocar o valor do endereço em nível lógico baixo.
 * Ou seja: essa função desliga um botão.
 * @params int fd - File descriptor da UART.
 * @params unsigned char endereco - Endereço do registrador que será desligado.
 */
void escrever_registrador(int fd, unsigned char endereco);
#endif
