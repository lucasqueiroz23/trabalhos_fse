#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "softPwm.h"
#include "utils_raspberry.h"
#include "modbus.h"
#include "pid.h"
#include "fila.h"
#include "control_lcd_16x2.h"

// variável para comunicação com file descriptor da uart
int fd_uart;

// variável para comunicação com file descriptor da i2c
int fd_i2c;

// variável que guarda a temperatura do elevador.
float temperatura = 24.0;

// mutexes para controlar acesso a uart
struct Mutexes {
    bool lendo_encoder;
    bool enviando_sinal_pwm;
    bool enviando_sinal_temperatura;
    bool lendo_registradores;
    bool escrevendo_registradores;
};
typedef struct Mutexes Mutexes;
Mutexes mut = {false, false, false, false, false};

/**
 * Verificar se a transação de leitura do encoder 
 * na UART pode ser realizada.
 */
bool proibicao_leitura_encoder() {
    return (
            mut.enviando_sinal_pwm ||
            mut.enviando_sinal_temperatura ||
            mut.lendo_registradores ||
            mut.escrevendo_registradores
    );
}

/**
 * Verificar se a transação de envio do sinal de pwm
 * na UART pode ser realizada.
 */
bool proibicao_envio_sinal_pwm() {
    return (
            mut.lendo_encoder ||
            mut.enviando_sinal_temperatura ||
            mut.lendo_registradores ||
            mut.escrevendo_registradores
    );
}

/**
 * Verificar se a transação de envio do sinal de temperatura
 * na UART pode ser realizada.
 */
bool proibicao_envio_sinal_temperatura() {
    return (
            mut.lendo_encoder ||
            mut.enviando_sinal_pwm ||
            mut.lendo_registradores ||
            mut.escrevendo_registradores
    );
}

/**
 * Verificar se a transação de leitura dos registradores
 * na UART pode ser realizada.
 */
bool proibicao_leitura_registradores() {
    return (
            mut.lendo_encoder ||
            mut.enviando_sinal_pwm ||
            mut.enviando_sinal_temperatura ||
            mut.escrevendo_registradores
    );
}

/**
 * Verificar se a transação de escrita dos registradores
 * na UART pode ser realizada.
 */
bool proibicao_escrita_registradores() {
    return (
            mut.lendo_encoder ||
            mut.enviando_sinal_pwm ||
            mut.enviando_sinal_temperatura ||
            mut.lendo_registradores
    );
}

// definição das threads usadas
pthread_t thread_escuta_enderecos;

Fila* requisicoes;

// variáveis que guardam as posições dos andares.
int posicao_terreo = 0;
int posicao_andar_1 = 0;
int posicao_andar_2 = 0;
int posicao_andar_3 = 0;
int posicao_atual = 0;

/**
 * Comunicar com o encoder e atualizar a posição atual.
 */
void atualizar_posicao_atual() {
    while(proibicao_leitura_encoder())
        ;

    mut.lendo_encoder = true;
    posicao_atual = ler_encoder(fd_uart);
    mut.lendo_encoder = false;
    delay(200);
}

/**
 * Comunicar com a UART e atualizar a temperatura da dashboard.
 */
void atualizar_temperatura_dashboard() {
    while(proibicao_envio_sinal_temperatura())
        ;
    mut.enviando_sinal_temperatura = true;
    temperatura = ((int)temperatura + 1) % 30;
    enviar_sinal_temperatura(temperatura);
    mut.enviando_sinal_temperatura = false;
}


// pinos dos sensores
#define SENSOR_T 18
#define SENSOR_1 23
#define SENSOR_2 24
#define SENSOR_3 25

// endereço dos registradores para leitura na UART
#define TAM_ARRAY_ENDERECOS 11
const unsigned char BOTAO_TERREO_SOBE = 0x00;
const unsigned char BOTAO_1_ANDAR_SOBE = 0x01;
const unsigned char BOTAO_1_ANDAR_DESCE = 0x02;
const unsigned char BOTAO_2_ANDAR_SOBE = 0x03;
const unsigned char BOTAO_2_ANDAR_DESCE = 0x04;
const unsigned char BOTAO_3_ANDAR_DESCE = 0x05;
const unsigned char BOTAO_ELEVADOR_EMERGENCIA = 0x06;
const unsigned char BOTAO_ELEVADOR_T = 0x07;
const unsigned char BOTAO_ELEVADOR_1 = 0x08;
const unsigned char BOTAO_ELEVADOR_2 = 0x09;
const unsigned char BOTAO_ELEVADOR_3 = 0x0A;

// vetor que indica quais botões apertados foram requisitados.
bool requisicoes_enderecos_sensores_uart[TAM_ARRAY_ENDERECOS] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
};

// mutex para o vetor acima
bool acessando_requisicoes = false;

/**
 * Sinalizar que uma requisição de usuário foi atendida, ou seja,
 * apagar o botão que o usuário apertou.
 */
void sinalizar_requisicao_atendida(int endereco){
    while(acessando_requisicoes)
        ;
    acessando_requisicoes = true;

    while(proibicao_escrita_registradores())
        ;
    mut.escrevendo_registradores = true;

    requisicoes_enderecos_sensores_uart[endereco] = false;
    escrever_registrador(fd_uart, endereco);

    acessando_requisicoes = false;
    mut.escrevendo_registradores = false;
}
/**
 * Verifica se existe requisição de usuário não atendida.
 */
bool existe_requisicao_nao_atendida() {
    while(acessando_requisicoes) 
        ;
    acessando_requisicoes = true;
    for(int i = 0; i < TAM_ARRAY_ENDERECOS; i++) {
        if(requisicoes_enderecos_sensores_uart[i] == true) {
            acessando_requisicoes = false;
            return true;
        }
    }
    acessando_requisicoes = false;
    return false;
}

/**
 * Obter a posição vertical do destino.
 * @params int endereco - Endereço do botão que foi apertado.
 */
int obter_destino(int endereco) {
    switch(endereco) {
        case 0x00:
        case 0x07:
            return posicao_terreo;
            break;
        case 0x01:
        case 0x02:
        case 0x08:
            return posicao_andar_1;
            break;
        case 0x03:
        case 0x04:
        case 0x09:
            return posicao_andar_2;
            break;
        case 0x05:
        case 0x0A:
            return posicao_andar_3;
            break;
        case 0x06:
            // usuário acionou botão de emergência
            //emergencia();
            return -1;
            break;
        default:
            // entra aqui caso, por algum motivo, a fila esteja vazia mas
            // a função foi chamada
            return -1;
            break;
    }
}

// definição dos pinos de direção (GPIO OUT)
#define DIR1 20
#define DIR2 21
#define POTM 12
int saidas[3] = {DIR1, DIR2, POTM};

// definição dos pinos dos sensores (GPIO IN)
int entradas[4] = {SENSOR_T, SENSOR_1, SENSOR_2, SENSOR_3};

// struct para controle da direção do elevador
struct DirecaoElevador {
    int dir1;
    int dir2;
};
typedef struct DirecaoElevador DirecaoElevador;

// structs com as direções
DirecaoElevador D_LIVRE = {0, 0};
DirecaoElevador D_SOBE = {1, 0};
DirecaoElevador D_DESCE = {0, 1};
DirecaoElevador D_FREIO = {1, 1};

/**
 * Setar os pinos de direção do elevador com uma direção nova.
 * @params DirecaoElevador dir - Nova direção do elevador.
 */
void mudar_direcao(DirecaoElevador dir) {
    digitalWrite(DIR1, dir.dir1 == 1 ? HIGH : LOW);
    digitalWrite(DIR2, dir.dir2 == 1 ? HIGH : LOW);
}

/**
 * Encontrar e retornar o inteiro que guarda
 * a posição de um andar do sistema.
 * @params int sensor_andar - O pino da GPIO que corresponde ao sensor do andar de interesse.
 */
int encontrar_posicao_andar(int sensor_andar, int potencia) {
    // nesse acesso à uart, não vamos colocar o mutex, pois neste momento
    // do processo, apenas a thread main está acessando ela.

    while(!digitalRead(sensor_andar)) {
        enviar_sinal_pwm(potencia);
    }

    int posicao = ler_encoder(fd_uart);
    return posicao;
}

/**
 * Alterar o valor do sinal pro pino pwm e notificar via UART.
 * @params int potencia - A potência a ser enviada como sinal.
 */
void alterar_sinal_pwm(int potencia) {
    softPwmWrite(POTM, abs(potencia));

    while(proibicao_envio_sinal_pwm())
        ;

    mut.enviando_sinal_pwm = true;
    enviar_sinal_pwm(potencia);
    mut.enviando_sinal_pwm = false;

}

/**
 * Calibra o elevador, ou seja, vai até os 3 andares e guarda
 * suas posições em variáveis globais (definidas no começo do arquivo).
 */
void calibrar_elevador() {
    // velocidade arbitrária para que o elevador passe lentamente 
    // e obtenha os andares desejados
    int velocidade = 20;
    atualiza_lcd(&temperatura, "Calibrando...");
    alterar_sinal_pwm(velocidade);
    mudar_direcao(D_SOBE);

    posicao_terreo = encontrar_posicao_andar(SENSOR_T, velocidade);
    printf("Posição do andar terreo = %d\n", posicao_terreo);

    posicao_andar_1 = encontrar_posicao_andar(SENSOR_1, velocidade);
    printf("Posição do 1o andar = %d\n", posicao_andar_1);

    posicao_andar_2 = encontrar_posicao_andar(SENSOR_2, velocidade);
    printf("Posição do 2o andar = %d\n", posicao_andar_2);

    posicao_andar_3 = encontrar_posicao_andar(SENSOR_3, velocidade);
    printf("Posição do 3o andar = %d\n", posicao_andar_3);
    printf("Foram encontradas as posições dos 3 andares!\n\n");

    // freiar elevador
    delay(50);
    mudar_direcao(D_FREIO);
    mudar_direcao(D_LIVRE);

    atualiza_lcd(&temperatura, "Parado A3");
}

/**
 * Escutar, dentro de um loop, todos os registradores 
 */
void* escutar_requisicoes_usuario(void* args) {
    while(true) {

        while(proibicao_leitura_registradores())
            ;
        mut.lendo_registradores = true;
        InteracaoRegistradores reg = ler_registrador(BOTAO_TERREO_SOBE, 11);
        mut.lendo_registradores = false;

        acessando_requisicoes = true;
        // O(1), sempre serão 11 iterações.
        for(int i = 0 ; i < (int) reg.qtd_bytes; i++) {
            if((int) reg.bytes[i] == 1 && requisicoes_enderecos_sensores_uart[i] == false) {
                requisicoes_enderecos_sensores_uart[i] = true;
                int endereco = i;

                // caso de uart tx error
                if(endereco < 0) continue;

                // debug
                printf("Chegou requisição do endereço %d\n", endereco);

                enfileira(requisicoes, endereco);
            }
        }
        acessando_requisicoes = false;
        // esperar 50ms para ler novamente
        delay(50);
    }

    return NULL;
}

/**
 * Procedimento para lidar com o ctrl+c.
 */
void fechar_processo(int sig) {
    printf("\n");
    printf("Fechando processo de controle do elevador\n");

    printf("Fechando threads\n");
    pthread_cancel(thread_escuta_enderecos);

    printf("Fechando comunicação com a UART\n");
    close(fd_uart);

    printf("Fechando comunicação com a i2c\n");
    ClrLcd();
    close(fd_i2c);

    printf("Resetando os pinos da GPIO utiilzados\n");
    resetar_placa();

    exit(0);
}


int main() {
    // configurações iniciais
    signal(SIGINT, fechar_processo);
    inicializar_placa(entradas, saidas, 4, 3, POTM);
    fd_i2c = inicia_i2c();
    pid_configura_constantes(0.5, 0.05, 40.0);
    requisicoes = cria_fila();

    // rotina para descobrir as posições verticais dos andares
    calibrar_elevador();

    // criação de thread que escuta os apertos dos botões
    pthread_create(&thread_escuta_enderecos, NULL, &escutar_requisicoes_usuario, NULL);

    atualizar_temperatura_dashboard();

    // a thread main vai lidar com o movimento em si do elevador
    while(true) {
        // polling (espera requisição do usuário)
        while(!existe_requisicao_nao_atendida()) {
            alterar_sinal_pwm(0);
            delay(200);
            continue;
        }

        atualizar_posicao_atual();
        
        int endereco = -1;
        desenfileira(requisicoes, &endereco);

        int destino = obter_destino(endereco);

        // caso o endereço não exista ou seja de emergência, pare por aqui
        if(destino == -1) continue;

        pid_atualiza_referencia(destino);

        if(pid_controle(posicao_atual) < 0) {
            mudar_direcao(D_DESCE);
            atualiza_lcd(&temperatura, "Descendo...");
        }
        else{
            mudar_direcao(D_SOBE);
            atualiza_lcd(&temperatura, "Subindo...");
        } 

        int distancia;

        // condição: veja se não está no valor de tolerância
        while((distancia = abs(posicao_atual - destino)) > 250) {
            pid_atualiza_referencia(destino);
            atualizar_posicao_atual();
            int potencia = pid_controle(posicao_atual);
            alterar_sinal_pwm(potencia);
        }


        // freiar elevador e atualizar LCD e dashboard com temperatura
        mudar_direcao(D_FREIO);
        char* msg_lcd;
        if(destino == posicao_terreo) msg_lcd = "Parado Terreo";
        else if(destino == posicao_andar_1) msg_lcd = "Parado A1";
        else if(destino == posicao_andar_2) msg_lcd = "Parado A2";
        else if(destino == posicao_andar_3) msg_lcd = "Parado A3";
        atualiza_lcd(&temperatura, msg_lcd);
        atualizar_temperatura_dashboard();

        // desligar os botões que o usuário apertou 
        sinalizar_requisicao_atendida(endereco);
        mudar_direcao(D_LIVRE);
    }

    exit(0);
}
