#include "config.h"

/************************************** VARIAVEIS GLOBAIS **************************************/


/************************************** METODOS **************************************/

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

uint16_t freeRam ()
{
  uint8_t newVariable;
  // heap is empty, use bss as start memory address
  if ((uint16_t)__brkval == 0)
    return (((uint16_t)&newVariable) - ((uint16_t)&__bss_end));
  // use heap end as the start of the memory address
  else
    return (((uint16_t)&newVariable) - ((uint16_t)__brkval));
}


void sleep() {
  //  Serial.println("S");
  //  delay(1000);
  digitalWrite(TRANSISTOR, LOW);
  digitalWrite(LED, LOW);

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
  Serial.begin(115200);
  //Serial.println(F("Saiu do sono"));
  //toda vez que sai do sono o uC é reiniciado, nao sendo necessario religar o led agr
}

void initialConfig() {

  Serial.begin(115200);
  pinMode(INTERRUPT, INPUT);
  pinMode(TRANSISTOR, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(TRANSISTOR, LOW); //garante que a raia ta desligada
  digitalWrite(LED, HIGH); //uC ligado

  power_adc_disable();
  power_spi_disable();
}

uint8_t readEEPROM() {
  char aux[3]; //vai ter no max dois caracteres o tamanho da piscina
  int i = 0;
  while (EEPROM.read(i) != '\0' && i < 3) {
    aux[i] = EEPROM.read(i);
    i++;
  }
  aux[i] = '\0';
  return (uint8_t)atoi(aux); //retorna tam_piscina
}

void pause() {
  while (digitalRead(INTERRUPT) != 1) {
    //fita.show();
    delay(100);
  }
}

void Stop() {
  wdt_enable(WDTO_15MS);
  for (;;) {}
}
