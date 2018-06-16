#include "leitura.h"

/********************

   COMUNICACAO COM ESP VIA I2C

 ********************/

uint8_t lerTreino() {

  //Serial.println(F("Leitura!"));

  if (sb::goSleep == 1) return SLEEP;
  
  if (sb::received == 1) {
    setNextStateVariables();
    return PARSING; //proximo estado
  }
  return LEITURA; //se mantem nesse estado
}

static void setNextStateVariables() {
  sb::sendState = 0;
  sb::received = 0;
}


