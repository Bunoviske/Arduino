#include "uC.h"


void definePins() {
  for (uint8_t i = 0; i < MaxLeds; i++) {
    gb::leds[i] = i + 22;
  }

  for (uint8_t i = 0; i < MaxLeds; i++) {
    pinMode(gb::leds[i], OUTPUT);
    digitalWrite(gb::leds[i], LOW);
  }

}

void updateEEPROM(uint8_t estado) {

  //posicao [0-3] da eeprom : update do estado leitura
  //posicao [4-7] da eeprom : update do estado treino
  //posicao [8-11] da eeprom : update do estado turnoff
  int valInteiro;
  String val = "";

  if (estado == LEITURA) {
    for (int i = 0; i < 4; i++) {
      char c = EEPROM.read(i);
      val += c;
    }

  }
  else if (estado == TREINO) {
    for (int i = 4; i < 8; i++) {
      char c = EEPROM.read(i);
      val += c;
    }
  }
  else if (estado == TURNOFF) {
    for (int i = 8; i < 12; i++) {
      char c = EEPROM.read(i);
      val += c;
    }
  }

  //Serial.println(val);
  //Serial.println(val.toInt());
  valInteiro = val.toInt() + 1;
  val = valInteiro; //valor a ser atualizado
  int j = val.length() - 1;

  if (estado == LEITURA) {
    Serial.print("Estado leitura: ");
    Serial.println(val);
    for (int i = 3; i >= 0; i--) {
      if (j < 0 ) EEPROM.write(i, '0');
      else EEPROM.write(i, val[j]);
      j--;
    }

  }
  else if (estado == TREINO) {
    Serial.print("Estado treino: ");
    Serial.println(val);
    for (int i = 7; i > 3; i--) {
      if (j < 0 ) EEPROM.write(i, '0');
      else EEPROM.write(i, val[j]);
      j--;
    }
  }
  else if (estado == TURNOFF) {
    Serial.print("Estado turnoff: ");
    Serial.println(val);
    for (int i = 11; i > 7; i--) {
      if (j < 0 ) EEPROM.write(i, '0');
      else EEPROM.write(i, val[j]);
      j--;
    }
  }

}


void uC_config() {

  Serial.begin(115200);
  //definePins();
  setNextStateVariables(); //proximo estado: leitura
}

static void setNextStateVariables() {
  gb::subState = GET_USER_INFO_MAIN;
  gb::firstTime = 1;
}
