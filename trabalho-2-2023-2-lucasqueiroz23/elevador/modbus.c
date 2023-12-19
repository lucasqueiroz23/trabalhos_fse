#include <stdio.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "crc16.h"
#include "modbus.h"
#include "wiringPi.h"


// últimos 4 dígitos da minha matrícula
char matricula[4] = {1, 7, 0, 3};

const unsigned char endereco_uart = 0x01;

const unsigned char CODIGO_SOLICITACAO_ENCODER = 0x23;
const unsigned char SUBCODIGO_SOLICITACAO_ENCODER = 0xc1;

const unsigned char CODIGO_ENVIO_PWM = 0x16;
const unsigned char SUBCODIGO_ENVIO_PWM = 0xc2;

const unsigned char CODIGO_ENVIO_TEMPERATURA = 0x16;
const unsigned char SUBCODIGO_ENVIO_TEMPERATURA = 0xd1;

const unsigned char CODIGO_LEITURA_REGISTRADORES = 0x03;
const unsigned char CODIGO_ESCRITA_REGISTRADORES = 0x06;


int abrir_comunicacao_uart() {
    int uart0_filestream = -1;

    // abrir arquivo que aponta para a porta serial 
    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non blocking read/write mode
    if (uart0_filestream == -1) {
        printf("Erro - Não foi possível iniciar a UART.\n");
        exit(1);
    }

    // setar options de mensagem
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    // salvar tudo 
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;
}

void enviar_dados(int uart, unsigned char codigo, unsigned char subcodigo, void* dados) {
    unsigned char tx_buffer[30];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];

    *p_tx_buffer++ = endereco_uart;
    *p_tx_buffer++ = codigo;

    // no caso da leitura nos registradores, o subcódigo
    // é o próprio endereço do registrador
    *p_tx_buffer++ = subcodigo;

    unsigned int size_dado = 0;
    unsigned int pos_inicial = 3;

    if(subcodigo == SUBCODIGO_ENVIO_PWM) {
        int dado = *((int*) dados);
        memcpy(&tx_buffer[pos_inicial], &dado, sizeof(dado));
        p_tx_buffer += sizeof(dado);
        size_dado = sizeof(dado);
    }

    if(subcodigo == SUBCODIGO_ENVIO_TEMPERATURA) {
        float dado = *((float*) dados);
        memcpy(&tx_buffer[pos_inicial], &dado, sizeof(dado));
        p_tx_buffer += sizeof(dado);
        size_dado = sizeof(dado);
    }

    if(codigo == CODIGO_LEITURA_REGISTRADORES) {
        InteracaoRegistradores reg = *((InteracaoRegistradores*) dados);
        *p_tx_buffer++ = reg.qtd_bytes;
        size_dado = sizeof(reg.qtd_bytes);
    }

    // colocar os últimos 4 dígitos da matrícula 
    memcpy(&tx_buffer[pos_inicial + size_dado], &matricula, sizeof(matricula));
    p_tx_buffer += sizeof(matricula);

    // calcular CRC
    short crc = calcula_CRC(tx_buffer, (p_tx_buffer - &tx_buffer[0]));
    memcpy(&tx_buffer[pos_inicial + sizeof(matricula) + size_dado], &crc, sizeof(crc));
    p_tx_buffer += sizeof(crc);

    delay(20);
    // escrever na uart
    int count = write(uart, &tx_buffer, (p_tx_buffer - &tx_buffer[0]));

    // avisar em caso de erro na escrita
    if (count <= 0) {
        printf("UART TX error\n");
    }
}

int ler_encoder(int fd) {
    fd = abrir_comunicacao_uart();
    // buffer de bytes que será enviado na porta tx
    unsigned char tx_buffer[20];

    // ponteiro para o buffer de bytes
    unsigned char *p_tx_buffer = &tx_buffer[0];

    *p_tx_buffer++ = endereco_uart;
    *p_tx_buffer++ = CODIGO_SOLICITACAO_ENCODER;
    *p_tx_buffer++ = SUBCODIGO_SOLICITACAO_ENCODER;

    // colocar os últimos 4 dígitos da matrícula 
    memcpy(&tx_buffer[3], &matricula, sizeof(matricula));
    p_tx_buffer += sizeof(matricula);

    // calcular crc
    short crc = calcula_CRC(tx_buffer, (p_tx_buffer - &tx_buffer[0]));
    memcpy(&tx_buffer[3 + sizeof(matricula)], &crc, sizeof(crc));
    p_tx_buffer += sizeof(crc); 

    int count = write(fd, &tx_buffer, (p_tx_buffer - &tx_buffer[0]));
    // avisar em caso de erro
    if (count <= 0) {
        printf("UART TX error - leitura do encoder\n");
    }

    // sleep para ter certeza de que a rx só vai ser lida quando estiver escrita de fato
    // trocar pra microssegundos? 
    delay(50);

    // o vetor de bytes abaixo vai guardar os valores lidos serialmente
    unsigned const int TAM_RESPOSTA = 8;
    unsigned char rx_buffer[TAM_RESPOSTA];

    // ler na uart
    int rx_length = read(fd, (void *)rx_buffer, TAM_RESPOSTA);

    int dado = 0;
    // erro na leitura
    if (rx_length < 0) {
        printf("Erro na leitura\n");
        dado = -1;
    }
    // nenhum dado disponível
    else if (rx_length == 0) {
        printf("nenhum dado disponível\n");
        dado = 0;
    }
    // dados disponíveis, vamos retorná-los
    else {
        rx_buffer[rx_length] = '\0';
        // copiar os bytes do vetor para a variável 'dado'
        memcpy(&dado, &rx_buffer[3], sizeof(dado));
    }

    close(fd);
    
    return dado;
}


void interagir_UART(unsigned char codigo, unsigned  char subcodigo, void* dados, void(*callback)(int, unsigned char, unsigned char, void*)) {
    int fd = abrir_comunicacao_uart();
    callback(fd, codigo, subcodigo, dados);

    if(codigo == CODIGO_LEITURA_REGISTRADORES){
        InteracaoRegistradores reg = *((InteracaoRegistradores*) dados);
        int qtd = reg.qtd_bytes;

        // o vetor de bytes abaixo vai guardar os valores lidos serialmente
        unsigned int TAM_RESPOSTA = 4 + qtd;
        unsigned char rx_buffer[TAM_RESPOSTA];

        // ler na uart
        delay(100);
        int rx_length = read(fd, (void *)rx_buffer, TAM_RESPOSTA);

        // erro na leitura
        if (rx_length < 0) {
            printf("Erro na leitura\n");
        }
        // nenhum dado disponível
        else if (rx_length == 0) {
            printf("nenhum dado disponível\n");
        }
        // dados disponíveis, vamos retorná-los
        else {
            // copiar os bytes do vetor para a variável 'dado'
            for(int i = 0, j = 2; j < rx_length - 1; i++, j++) {
                reg.bytes[i] = rx_buffer[j];
            }
        }
    }
    close(fd);
}

void enviar_sinal_pwm(int sinal) {
    interagir_UART(CODIGO_ENVIO_PWM, SUBCODIGO_ENVIO_PWM, (void*) (&sinal), enviar_dados);
}

void enviar_sinal_temperatura(float sinal) {
    interagir_UART(CODIGO_ENVIO_TEMPERATURA, SUBCODIGO_ENVIO_TEMPERATURA, (void*) (&sinal), enviar_dados);
}

InteracaoRegistradores ler_registrador(unsigned char endereco, unsigned char qtd) {
    unsigned char* bytes = malloc(sizeof(unsigned char) * qtd);
    InteracaoRegistradores reg = {endereco, qtd, bytes};
    interagir_UART(CODIGO_LEITURA_REGISTRADORES, endereco, (void*) (&reg), enviar_dados);

    return reg;
}

void escrever_registrador(int fd, unsigned char endereco) {
    fd = abrir_comunicacao_uart();
    // buffer de bytes que será enviado na porta tx
    unsigned char tx_buffer[20] = {
        endereco_uart,
        CODIGO_ESCRITA_REGISTRADORES,
        endereco,
        1, // apenas um byte será escrito
        0, // o byte escrito será para desligar um botão
    };

    memcpy(&tx_buffer[5], &matricula, sizeof(matricula));

    // calcular crc
    short crc = calcula_CRC(tx_buffer, 9);
    memcpy(&tx_buffer[9], &crc, sizeof(crc));

    // escrever na UART
    int count = write(fd, &tx_buffer, 12);
    // avisar em caso de erro
    if (count <= 0) {
        printf("UART TX error - leitura do encoder\n");
    }

    close(fd);
}
