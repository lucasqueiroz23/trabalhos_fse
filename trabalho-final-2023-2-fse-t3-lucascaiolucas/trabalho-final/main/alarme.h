#ifndef __ALARME_H__
#define __ALARME_H__

// pinos da GPIO para o alarme
#define ALARME_BLUE 25
#define ALARME_GREEN 32
#define ALARME_RED 33

// pinos da GPIO para a foto
#define FOTO_RED 26
#define FOTO_GREEN 14
#define FOTO_BLUE 27

/**
 * Ativar o alarme (piscar um led RGB em azul e vermelho e outro para tirar fotos).
 */
void ativar_alarme();

/**
 * Desativar um alarme(setar os botões como desligados).
 */
void desativar_alarme();

/**
 * Ativar o sistema de segurança, ou seja, ativar o buzzer e manter o alarme piscando.
 */
void ativar_sistema_de_seguranca(void* params);
#endif
