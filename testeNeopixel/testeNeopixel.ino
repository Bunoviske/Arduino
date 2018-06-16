#include "FSM.h"

/***************

  Comentarios do codigo

 **************/

//quando escreve na tela o numero de repeticoes, influencia no tempo de execucao do led


void setup() {
  initFSM();
}

#ifdef DEBUG
uint8_t state = TREINO; //estado inicial do debug
#else
uint8_t state = LEITURA; //estado inicial
#endif

void loop() {
  state = changeState(state);
}


