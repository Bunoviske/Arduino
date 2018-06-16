#include "uC.h"

uint8_t sleep() {

  //Serial.println(F("Sleep!"));

  sb::goSleep = 0; //zera a variavel de controle do sono
  digitalWrite(TRANSISTOR, HIGH);
  digitalWrite(LED, LOW);
  
  //variaveis de treino que devem ser zeradas quando desliga o arduino
  sb::direcao = 'i';
  sb::atual_pos = 0;
  sb::flagSinalizaComeco = 1;

  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_usart0_disable();
  //PRR = (1 << PRTIM2) | (1 << PRTIM2) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRSPI) | (1 << PRADC);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli(); //desabilita interrupcao
  sleep_enable();
  sleep_bod_disable();
  sei(); //habilita interrupcao
  sleep_cpu();  //SLEEPING...................................................

  sleep_disable();
  sei(); //habilita interrupcao
  power_timer0_enable();
  power_timer1_enable();
  power_timer2_enable();
  power_usart0_enable();

  setNextStateVariables(); //proximo estado: leitura
  return LEITURA; //proximo estado depois de dormir sempre é ler novo treino

}

void uC_config() {

  Serial.begin(115200);
  pinMode(PIN_ESP, OUTPUT);
  pinMode(TRANSISTOR, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(TRANSISTOR, HIGH); //garante que a raia ta desligada
  digitalWrite(PIN_ESP, LOW);

  //power_adc_disable();
  //power_spi_disable();

  setNextStateVariables(); //proximo estado: leitura
}

static void setNextStateVariables() {
  sb::sendState = 1;
  //sb::received = 0; //inicia zerada
  digitalWrite(LED, HIGH); //uC ligado
}
