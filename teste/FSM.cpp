#include "FSM.h"


void initFSM() {
  uC_config();
  clearAllLeds();

#ifdef DEBUG
  gb::serieAtual.qtdSeries = 2;
  gb::serieAtual.repeticoes = 5;
  gb::serieAtual.intervalo = 2;
  gb::serieAtual.TimeIda = 1;
  gb::serieAtual.TimeVolta = 2;
#endif

}


uint8_t changeState(uint8_t nextState) {

  //Maquina de estados

  if (nextState == LEITURA) {
    if (gb::firstTime) {
      updateEEPROM(LEITURA);
      gb::firstTime = 0;
    }
    return getUserInfoTouch();
  }

  else if (nextState == TREINO) {
    if (gb::firstTime) {
      updateEEPROM(TREINO);
      gb::firstTime = 0;
    }
    return treino();
  }

}


