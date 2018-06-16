#include "FSM.h"

/***************

  Comentarios do codigo

 **************/

//quando escreve na tela o numero de repeticoes, influencia no tempo de execucao do led


/*
 * No arquivo adafruit_tft.cpp, ficar atento a linha:ILI932X_GATE_SCAN_CTRL1  , 0x2700,//Driver Output Control (R60h) MUDAR VALOR CONFORME: #ifdef DRIVER_BUG 0x2700 #else 0xA700
 * 
 * 
 * Coisas a se mudar:
 * 
 * 1- descomentar linha "definePins()" na função uC_config(). Descomentar apenas quando for passar o codigo para o mega
 * 2- as novas telas ja foram integradas. falta apenas as telas do teclado com botao de voltar e os botoes de pause/stop
 * 
 * 
*/


void setup() {  
  initFSM();
}

uint8_t state = LEITURA; //estado inicial

void loop() {
  state = changeState(state);
}


