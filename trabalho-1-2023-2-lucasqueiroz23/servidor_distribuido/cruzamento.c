#include <wiringPi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include "semaforos.h"
#include "utils_raspberry.h"
#include "utils_sockets.h"
#include "sensores.h"
#include "utils_file.h"

#define CRUZAMENTO_1 1
#define CRUZAMENTO_2 2
#define SIZE_ENTRADAS 6
#define SIZE_SAIDAS 5


int saidas_cruzamento1[SIZE_SAIDAS] = {9, 11, 5, 6, 17};
int saidas_cruzamento2[SIZE_SAIDAS] = {10, 8, 1, 18, 21};

int entradas_cruzamento_1[SIZE_ENTRADAS] = {13, 19, 0, 27, 26, 22};
int entradas_cruzamento_2[SIZE_ENTRADAS] = {23, 24, 16, 20, 25, 12};

int* sensores;
int sensores_cruzamento_1[4] = {26, 22, 0, 27};
int sensores_cruzamento_2[4] = {25, 12, 16, 20};

int buzzer;

int socket_distribuido;


int cruzamento;
int porta;

struct semaforo* principal;
struct semaforo* auxiliar;

/**
 * Struct para controle da ordem da mudança dos estados.
 */
struct ordem_estados {
    int primeiro;
    int segundo;
    int terceiro;
};

typedef struct ordem_estados ordem_estados;

// Ordem do semáforo principal
ordem_estados ordem_principal = {VERDE, AMARELO, VERMELHO};

// Ordem do semáforo auxiliar
ordem_estados ordem_aux = {VERMELHO, VERDE, AMARELO};

/**
 * Struct para passar os argumentos de uma thread. O "id" indica se o semáforo
 * é o principal ou o auxiliar.
 */
struct params {
    int id;
};
typedef struct params params;

pthread_t t_principal;
pthread_t t_auxiliar;

params* args_aux;
params* args_principal;


/**
 * Função chamada toda vez que o botão de pedestre do semáforo
 * principal é acionado.
 */
void verificar_botao_ped_principal() {
    principal->pedestre_fez_requisicao = true;
}

/**
 * Função chamada toda vez que o botão de pedestre do semáforo
 * auxiliar é acionado.
 */
void verificar_botao_ped_aux() {
    auxiliar->pedestre_fez_requisicao = true;
}


void verificar_sensor(int pino, const int velocidade_maxima_via, semaforo* s, semaforo* concorrente, char* mensagem) {
    float velocidade = verificar_andamento_semaforo(pino, 80, s, concorrente);
    if (velocidade > 0) notificar_sinal_vermelho(s, buzzer);
    if(velocidade > velocidade_maxima_via) {
        notificar_multa_velocidade(buzzer);
    }
    notificar_passagem_carro(mensagem);
}

void verificar_sensor_principal_1(){
    char* mensagem;
    if(cruzamento == CRUZAMENTO_1) 
        mensagem = "c1_principal_1";
    else mensagem = "c2_principal_1";

    verificar_sensor(sensores[2], 80, principal, auxiliar, mensagem);
}

void verificar_sensor_principal_2(){
    char* mensagem;
    if(cruzamento == CRUZAMENTO_1) 
        mensagem = "c1_principal_2";
    else mensagem = "c2_principal_2";

    verificar_sensor(sensores[3], 80, principal, auxiliar, mensagem);
}

void verificar_sensor_aux_1(){
    char* mensagem;
    if(cruzamento == CRUZAMENTO_1) 
        mensagem = "c1_aux_1";
    else mensagem = "c2_aux_1";

    verificar_sensor(sensores[0], 60, auxiliar, principal, mensagem);
}

void verificar_sensor_aux_2(){
    char* mensagem;
    if(cruzamento == CRUZAMENTO_1) 
        mensagem = "c1_aux_2";
    else mensagem = "c2_aux_2";

    verificar_sensor(sensores[1], 60, auxiliar, principal, mensagem);
}

/**
 * Função que, numa thread própria, realiza o controle do semáforo auxiliar.
 * A ordem é: VERMELHO => VERDE => AMARELO.
 * O semáforo auxiliar só fica verde quando o principal está vermelho.
 */
void controlar_semaforo_auxiliar() {
    while(1) {
        // auxiliar começa vermelho, principal começa verde
        passar_tempo_minimo(auxiliar, ordem_aux.primeiro);

        // esperar o principal fechar
        while(principal->estado_atual != VERMELHO) {
            delay(UM_SEGUNDO);
            auxiliar->tempo_decorrido++;
        }
        
        // trocar de vermelho pra verde
        passar_tempo_minimo(auxiliar, ordem_aux.segundo);

        // agora, eu devo verificar quanto tempo falta pro estado do principal mudar
        esperar_para_amarelar(auxiliar, principal, buzzer);

        // trocar de verde pra amarelo
        passar_tempo_minimo(auxiliar, ordem_aux.terceiro);
        digitalWrite(buzzer, LOW);
    }
}

/**
 * Função que, numa thread própria, realiza o controle do semáforo principal.
 * A ordem é: VERDE => AMARELO => VERMELHO.
 * O semáforo principal só fica verde quando o auxiliar está vermelho.
 */
void controlar_semaforo_principal() {
    while(1) {
        // principal começa verde, auxiliar começa vermelho
        passar_tempo_minimo(principal, ordem_principal.primeiro);

        // aqui, eu devo verificar quanto tempo falta pro estado do auxiliar mudar...
        esperar_para_amarelar(principal, auxiliar, buzzer);

        // trocar de verde para amarelo
        passar_tempo_minimo(principal, ordem_principal.segundo);
        digitalWrite(buzzer, LOW);

        // trocar de amarelo para vermelho
        passar_tempo_minimo(principal, ordem_principal.terceiro);

        // esperar o auxiliar fechar
        while(auxiliar->estado_atual != VERMELHO) {
            delay(UM_SEGUNDO);
            principal->tempo_decorrido++;
        }
    }
}

/**
 * Definir qual semáforo será controlado pela thread. Essa função
 * pressupõe que o cruzamento não está nem no modo noturno e nem no modo
 * de emergência.
 * @param void* args Os argumentos da função. No caso, essa função recebe
 * args do tipo params (definido numa struct acima).
 */
void* definir_semaforo_a_controlar(void* args) {
    params *p = (params*) args;
    int id_semaforo = p->id;

    if(id_semaforo == ID_SEMAFORO_AUXILIAR) 
        controlar_semaforo_auxiliar();

    if(id_semaforo == ID_SEMAFORO_PRINCIPAL)
        controlar_semaforo_principal();

    return NULL;
}

/**
 * Colocar um semáforo no estado intermitente.
 */
void* estado_intermitente(void* args) {
    params *p = (params*) args;
    int id_semaforo = p->id;

    if(id_semaforo == ID_SEMAFORO_AUXILIAR) 
        intermitente(auxiliar);

    if(id_semaforo == ID_SEMAFORO_PRINCIPAL)
        intermitente(principal);

    return NULL;
}


/**
 * Colocar um semáforo no estado de emergencia.
 */
void* estado_emergencia(void* args) {
    params *p = (params*) args;
    int id_semaforo = p->id;

    if(id_semaforo == ID_SEMAFORO_AUXILIAR) 
        emergencia(auxiliar, principal);

    if(id_semaforo == ID_SEMAFORO_PRINCIPAL)
        emergencia(principal, auxiliar);

    return NULL;
}

/**
 * Procedimento para lidar com o Ctrl+C.
 */
void fechar_processo(int sig) {
    printf("\nFechando cruzamento %d\n", cruzamento);
    resetar_cruzamento(cruzamento);
    close(socket_distribuido);
    exit(0);
}


/**
 * Entrar ou sair do modo de emergência.
 */
void modo_emergencia() {
    principal->modo_noturno = false;
    auxiliar->modo_noturno = false;

    principal->modo_emergencia = !principal->modo_emergencia;
    auxiliar->modo_emergencia = !auxiliar->modo_emergencia;

    principal->modo_emergencia ? 
        printf("Entrando no modo de emergência\n\n") :
        printf("Saindo do modo de emergência\n\n");

    // fechar as threads abruptamente, estando elas
    // no modo normal, noturno ou outro.
    pthread_cancel(t_principal);
    pthread_cancel(t_auxiliar);

    void* callback = principal->modo_emergencia ? 
        &estado_emergencia : 
        &definir_semaforo_a_controlar;

    pthread_create(&t_principal, NULL, callback, args_principal);
    pthread_create(&t_auxiliar, NULL, callback, args_aux);

}

/**
 * Entrar ou sair do modo noturno.
 */
void modo_noturno() {
    principal->modo_emergencia = false;
    auxiliar->modo_emergencia = false;

    principal->modo_noturno = !principal->modo_noturno;
    auxiliar->modo_noturno = !auxiliar->modo_noturno;

    principal->modo_noturno ?
        printf("Entrando no modo noturno\n\n") :
        printf("Saindo do modo noturno\n\n");

    // fechar as threads abruptamente, estando elas
    // no modo normal, noturno ou outro.
    pthread_cancel(t_principal);
    pthread_cancel(t_auxiliar);

    void* callback = principal->modo_noturno ? 
        &estado_intermitente : 
        &definir_semaforo_a_controlar;

    pthread_create(&t_principal, NULL, callback, args_principal);
    pthread_create(&t_auxiliar, NULL, callback, args_aux);
}

/**
 * Função que roda na thread main. Vai escutar notificações
 * vindas do servidor central.
 */
void escutar_servidor_central() {
    // abrir socket
    struct sockaddr_in endereco_distribuido;
    if((socket_distribuido = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Falha ao abrir socket\n");

    memset(&endereco_distribuido, 0, sizeof(endereco_distribuido));

    endereco_distribuido.sin_family = AF_INET;
    endereco_distribuido.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco_distribuido.sin_port = htons(porta);

    if(bind(socket_distribuido, (struct sockaddr*)&endereco_distribuido, sizeof(endereco_distribuido)) < 0)
        printf("Falha no bind\n");

    if(listen(socket_distribuido, 10) < 0)
        printf("Falha no listen\n");

    printf("Escutando na porta %d\n", porta);

    while(1) {
        int socket_cliente;
        struct sockaddr_in endereco_cliente;
        unsigned tam_cliente = sizeof(endereco_cliente);

        if((socket_cliente = accept(socket_distribuido, (struct sockaddr*) &endereco_cliente, &tam_cliente)) < 0)
            printf("Falha no accept\n");

        printf("Conectado com o cliente %s\n", inet_ntoa(endereco_cliente.sin_addr));
        char* mensagem = receber_mensagem(socket_cliente);
        printf("Mensagem recebida: %s\n", mensagem);
        close(socket_cliente);

        if(!strcmp(mensagem, "noturno"))
            modo_noturno();
        if(!strcmp(mensagem, "emergencia")){
            modo_emergencia();
        }
        
    }
}

int main(int argc, char** argv) {
    int* saidas;
    int* entradas;

    char* config = obter_texto_arquivo();

    // tratar entrada na linha de comando
    if(argc != 2) {
        printf("erro no uso\n");
        printf("Uso correto: ./cruzamento <opcao>\n");
        printf("<opcao> pode ser --c1 ou --c2\n");
        exit(1);
    }


    if(!strcmp(argv[1], "--c1")) {
        cruzamento = 1;
        porta = obter_porta(config, '1');
        buzzer = BUZZER_1;
        saidas = saidas_cruzamento1;
        entradas = entradas_cruzamento_1;
        sensores = sensores_cruzamento_1;
    }

    else if (!strcmp(argv[1], "--c2")){
        cruzamento = 2;
        porta = obter_porta(config, '2');
        buzzer = BUZZER_2;
        saidas = saidas_cruzamento2;
        entradas = entradas_cruzamento_2;
        sensores = sensores_cruzamento_2;
    }

    else {
        printf("erro no uso\n");
        printf("Uso correto: ./cruzamento <opcao>\n");
        printf("<opcao> pode ser --c1 ou --c2\n");
        exit(1);
    }

    inicializar_placa(entradas, saidas, SIZE_ENTRADAS, SIZE_SAIDAS);

    // handler para ctrl + c
    signal(SIGINT, fechar_processo);

    wiringPiISR(entradas[0], INT_EDGE_RISING, verificar_botao_ped_principal);
    wiringPiISR(entradas[1], INT_EDGE_RISING, verificar_botao_ped_aux);


    principal = inicializar_semaforo(saidas[0], saidas[1], ID_SEMAFORO_PRINCIPAL, sensores[2], sensores[3]);
    auxiliar = inicializar_semaforo(saidas[2], saidas[3], ID_SEMAFORO_AUXILIAR, sensores[0], sensores[1]);

    args_principal = malloc(sizeof(params));
    args_aux = malloc(sizeof(params));

    args_principal->id = ID_SEMAFORO_PRINCIPAL;
    args_aux->id = ID_SEMAFORO_AUXILIAR;

    // por padrão, os dois semáforos estão no modo normal.
    pthread_create(&t_principal, NULL, &definir_semaforo_a_controlar, args_principal);
    pthread_create(&t_auxiliar, NULL, &definir_semaforo_a_controlar, args_aux);

    wiringPiISR(sensores[0], INT_EDGE_RISING, verificar_sensor_aux_1);
    wiringPiISR(sensores[1], INT_EDGE_RISING, verificar_sensor_aux_2);

    wiringPiISR(sensores[2], INT_EDGE_RISING, verificar_sensor_principal_1);
    wiringPiISR(sensores[3], INT_EDGE_RISING, verificar_sensor_principal_2);

    escutar_servidor_central();

    exit(0);
}

