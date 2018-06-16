#include "FSM.h"


void initFSM() {
  uC_config();
  clearAllLeds();

#ifdef DEBUG
  gb::qtdSeries = 2;
  gb::repeticoes = 5;
  gb::intervalo = 2;
  gb::TimeIda = 1;
  gb::TimeVolta = 2;
#endif

}


uint8_t changeState(uint8_t nextState) {

  //Maquina de estados

  if (nextState == LEITURA) {
    if (gb::firstTime) {
      gb::firstTime = 0;
    }
    return getUserInfoTouch();
  }

  else if (nextState == TREINO) {
    if (gb::firstTime) {
      gb::firstTime = 0;
    }
    return treino();
  }

}


