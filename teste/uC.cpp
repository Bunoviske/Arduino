#include "uC.h"

uint8_t sleep() {

  //Serial.println(F("Sleep!"));

  gb::isSleeping = 1;

  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_usart0_disable();
  //PRR = (1 << PRTIM2) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRADC);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli(); //desabilita interrupcao
  sleep_enable();
  //sleep_bod_disable();
  sei(); //habilita interrupcao
  sleep_cpu();  //SLEEPING...................................................

  sleep_disable();
  sei(); //habilita interrupcao
  power_adc_enable();
  power_spi_enable();
  power_twi_enable();
  power_timer0_enable();
  power_timer1_enable();
  power_timer2_enable();
  power_usart0_enable();

  //Serial.println(gb::isSleeping);

  gb::isSleeping = 0;
  setNextStateVariables(); //proximo estado: leitura
  gb::firstTime = 1;
  return LEITURA; //proximo estado depois de dormir sempre é ler novo treino

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


  //Serial.println(val);
  //Serial.println(val.toInt());
  valInteiro = val.toInt() + 1;
  val = valInteiro; //valor a ser atualizado
  int j = val.length() - 1;

  if (estado == LEITURA) {
    //Serial.print("Estado leitura: ");
    //Serial.println(val);
    for (int i = 3; i >= 0; i--) {
      if (j < 0 ) EEPROM.write(i, '0');
      else EEPROM.write(i, val[j]);
      j--;
    }

  }
  else if (estado == TREINO) {
    //Serial.print("Estado treino: ");
    //Serial.println(val);
    for (int i = 7; i > 3; i--) {
      if (j < 0 ) EEPROM.write(i, '0');
      else EEPROM.write(i, val[j]);
      j--;
    }
  }


}


void uC_config() {
  
#ifdef DEBUG
  Serial.begin(115200);
#endif

  configTouch();
  setNextStateVariables(); //proximo estado: leitura
}

static void setNextStateVariables() {

}
