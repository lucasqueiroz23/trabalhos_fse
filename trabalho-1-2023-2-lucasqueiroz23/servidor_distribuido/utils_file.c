#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils_file.h"

char* obter_texto_arquivo() {
    FILE *fp = fopen("../config_servidores", "r");

    // obter tamanho do arquivo
    fseek(fp, 0, SEEK_END);
    int tamanho_arquivo = ftell(fp);
    rewind(fp);

    char* texto = malloc(sizeof(char) * tamanho_arquivo);

    // copiar conteúdos do arquivo para um vetor
    char caracter;  
    int i = 0;
    while((caracter = fgetc(fp)) != EOF && i < tamanho_arquivo) {
        texto[i] = caracter; 
        i++;
    }

    return texto;
}

int obter_porta(char* texto, char servidor) {
    int i = 0;
    int tamanho_str = strlen(texto);

    while(i < tamanho_str) {
        if (texto[i] == 'C' && texto[i+1] == 'R') {
            if(!(texto[i + 11] == servidor)) {
                i++;
                continue;
            }

            i += 11;

            int line_break = 0;
            while (line_break != 2) {
                if(texto[i] == '\n') line_break++;
                i++;
            }

            while(texto[i] != '=') 
                i++;

            i++;
            int j = i;

            while(texto[i] != '\n') {
                i++;
            }

            int size = i - j;
            char* str = malloc(size*sizeof(char));

            int k = 0;

            while(j < i && k < size) {
                str[k] = texto[j++];
                k++;
            }

            return atoi(str);

        } else i++;
    }



    return 0;
}

char* obter_ip_servidor(char* texto) {
    int i = 0;
    int tamanho_str = strlen(texto);

    while(i < tamanho_str) {
        if (texto[i] == 'S' && texto[i+1] == 'E' && texto[i+2] == 'R') {
            i += 15;

            int line_break = 0;
            while (line_break != 1) {
                if(texto[i] == '\n') line_break++;
                i++;
            }

            while(texto[i] != '=') 
                i++;

            i++;
            int j = i;

            while(texto[i] != '\n') {
                i++;
            }

            int size = i - j;
            char* str = malloc(size*sizeof(char));

            int k = 0;

            while(j < i && k < size) {
                str[k] = texto[j++];
                k++;
            }

            return str;

        } else i++;
    }



    return " ";
}

int obter_porta_servidor(char* texto) {
    int i = 0;
    int tamanho_str = strlen(texto);

    while(i < tamanho_str) {
        if (texto[i] == 'S' && texto[i+1] == 'E' && texto[i+2] == 'R') {
            i += 15;

            int line_break = 0;
            while (line_break != 2) {
                if(texto[i] == '\n') line_break++;
                i++;
            }

            while(texto[i] != '=') 
                i++;

            i++;
            int j = i;

            while(texto[i] != '\n') {
                i++;
            }

            int size = i - j;
            char* str = malloc(size*sizeof(char));

            int k = 0;

            while(j < i && k < size) {
                str[k] = texto[j++];
                k++;
            }

            return atoi(str);

        } else i++;
    }



    return 0;
}
