#include "FSM.h"


void initFSM() {
  configWire();
  uC_config();
}


uint8_t changeState(uint8_t nextState) {

  //Maquina de estados

  if (nextState == LEITURA) return lerTreino();

  else if (nextState == PARSING) return parseTreino();

  else if (nextState == TREINO) return treino();

  else return sleep();
}


