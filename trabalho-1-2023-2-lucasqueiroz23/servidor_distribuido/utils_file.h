#ifndef __UTILS_FILE_H
#define __UTILS_FILE_H

/**
 * Obter, numa string, o conteúdo em texto de um arquivo.
 * Nesse caso, o arquivo sempre é o mesmo: o de configuração.
 * @returns O conteúdo, numa string, de um arquivo.
 */
char* obter_texto_arquivo();

/**
 * Obter a porta de um servidor, com base no texto obtido pela
 * função obter_texto_arquivo.
 * @params char* texto O texto do arquivo em questão
 * @params int servidor Identificador do servidor cuja porta 
 * é de interesse.
 */
int obter_porta(char* texto, char servidor);

/**
 * Obter o IP de um servidor, com base no texto obtido pela
 * função obter_texto_arquivo.
 * @params char* texto O texto do arquivo em questão
 * @params int servidor Identificador do servidor cujo IP 
 * é de interesse.
 */
char* obter_ip_servidor(char* texto);

/**
 * Obter a porta do servidor central.
 */
int obter_porta_servidor(char* texto);

#endif
