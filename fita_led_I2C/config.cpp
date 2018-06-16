#include "config.h"

/************************************** VARIAVEIS GLOBAIS **************************************/


uint8_t tamPiscina;
int NUMPIXELS; //int NUMPIXELS = tamPiscina * 10;
CRGB fita[250]; //modo normal ---->usar BGR
uint8_t qtd_serie = 0;
uint8_t qtdIda = 0;
uint8_t qtdVolta = 0;
char direcao = 'i'; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta
uint16_t ultima_pos = 0;
uint8_t estado;
char buffer[250];
uint16_t contGlobal = 0;
Serie series[TAM_MAX];

/*************************************/

uint8_t received = 0;
uint8_t sendState = 0;
bool startNow = 0;
bool startProgram = 0;
bool sendTrainTime = 0;
bool sendSeriesTime = 0;
bool rcvTime = 0;
float tempo = 0;


/************************************** METODOS **************************************/


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
  Serial.println(F("Saiu do sono"));
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

void readEEPROM() {
  char aux[3]; //vai ter no max dois caracteres o tamanho da piscina
  int i = 0;
  while (EEPROM.read(i) != '\0' && i < 3) {
    aux[i] = EEPROM.read(i);
    i++;
  }
  aux[i] = '\0';
  tamPiscina = atoi(aux);
}

void pause() {
  while (digitalRead(INTERRUPT) != 1) {
    FastLED.show();
  }
}

void Stop() {
  wdt_enable(WDTO_15MS);
  for (;;) {}
}
