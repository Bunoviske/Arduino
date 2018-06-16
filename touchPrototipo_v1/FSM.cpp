#include "FSM.h"


void initFSM() {
  uC_config();
  configTouch();
}


uint8_t changeState(uint8_t nextState) {

  //Maquina de estados

  if (nextState == LEITURA) {
    if (gb::firstTime) {
      updateEEPROM(LEITURA);
      gb::firstTime = 0;
    }
    //Serial.println("oi");
    return leitura();
  }

  else if (nextState == TREINO) {
    if (gb::firstTime) {
      updateEEPROM(TREINO);
      gb::firstTime = 0;
    }
    uint8_t ret = train();
    if (ret == LEITURA) updateEEPROM(2); //se esse valor for 2, sinaliza q a pessoa terminou o treino (gravar na eeprom)
    return ret;
  }
}


