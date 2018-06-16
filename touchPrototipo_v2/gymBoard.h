#ifndef GYMBOARD_H
#define GYMBOARD_H

//#include <avr/pgmspace.h>
//#include <avr/wdt.h>
//#include <avr/sleep.h>
//#include <avr/interrupt.h>
//#include <avr/power.h>
//#include <Arduino.h>
//#include <EEPROM.h>

//#include <SD.h>
#include "PetitFS.h"

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/McNeight/MemoryFree
*/
//#include <MemoryFree.h>

/**************************************

   DEFINES

**************************************/

#define DEBUG

//estados
#define LEITURA        0
#define TREINO         1
#define SLEEP          2

//sub estados
#define SINALIZA_COMECO 6
#define INTERVALO       5
#define TREINAR         4
#define PAUSE           3
#define STOP            2 //esse estado existe mas nao eh uma funcao
#define LOGICA          1
#define INIT_TREINO     0

#define GET_USER_INFO_MAIN           7
#define GET_USER_INFO_START          8
#define GET_USER_INFO_PERSONALIZADO  9

#define MAX_LEDS            15
#define ledsSinalizaComeco  15

#ifdef DEBUG
#define PIN_WS2811          6 //ocorre um bug com a fita de led (leds azuis acendem) quando esse pino é usado com a biblioteca do touch
#else 
#define PIN_WS2811          0
#endif

//#define SD_CS             10 //pino de selecao do cartao SD


/*************************
   Definicoes de variaveis de treino */

typedef struct {

 uint8_t qtdSeries;
 uint8_t repeticoes;
 int intervalo;
 uint8_t TimeIda;
 uint8_t TimeVolta;
 uint8_t TimeEspera;

} Serie;
/*************************/


namespace gb { //abreviacao para gymBoard

//variaveis

extern char direcao;
extern bool firstTime;
extern uint8_t serieAtualCont;


extern Serie serieAtual;
//extern uint16_t NUMPIXELS; //int NUMPIXELS = tamPiscina * 10;

extern uint8_t subState;
extern uint8_t modo;
extern uint8_t goPause;
extern uint8_t startNow;

extern uint8_t flagSinalizaComeco; //variavel do estado TREINO que deve ser zerada no sleep
extern int atual_pos; //variavel do estado TREINO que deve ser zerada no sleep

extern CRGB fita[MAX_LEDS]; //modo normal ---->usar BRG. na memoria sao (MAX_LEDS*3) bytes IMPORTANTE: variaveis que acessam o vetor (index) precisam ser INT!!!!!!

}


#endif
