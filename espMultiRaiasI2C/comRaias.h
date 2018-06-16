#ifndef RAIA_COM
#define RAIA_COM

#include "parseTreino.h"


//VARIAVEIS

extern bool state[MaxRaias]; //o arduino deve enviar 1 quando estiver no estado de leitura, para o server ter ctz que pode mandar dado do app
//o app deve habilitar a opcao de stop so dps que a serie for enviada, para evitar choque com o estado de leitura

extern bool ligado[MaxRaias]; //indica se a raia esta ligada ou nao

//METODOS

void sendData2Raia(bool serie);
void readRaias();
void configGPIO();
void sleepAll();
void waitForSignal2Desinterrupt();

#endif
