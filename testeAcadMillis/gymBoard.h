
#ifndef GYMBOARD_H
#define GYMBOARD_H

//#include <avr/pgmspace.h>
//#include <avr/wdt.h>
//#include <avr/sleep.h>
//#include <avr/interrupt.h>
//#include <avr/power.h>
//#include <Arduino.h>
//#include <EEPROM.h>

#include <SD.h>
#include <Adafruit_NeoPixel.h>


/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/McNeight/MemoryFree
*/
//#include <MemoryFree.h>

/**************************************

   DEFINES

**************************************/

//#define DEBUG

//estados
#define LEITURA        0
#define TREINO         1
#define SLEEP          2


#define MAX_LEDS            15
#define ledsSinalizaComeco  15

#ifdef DEBUG
#define PIN_WS2811          6 //ocorre um bug com a fita de led (leds azuis acendem) quando esse pino é usado com a biblioteca do touch
#else 
#define PIN_WS2811          0
#endif




/*************************
   Definicoes de variaveis de treino */

//typedef struct {
//
// int qtdSeries;
// int repeticoes;
// int intervalo;
// float TimeIda;
// float TimeVolta;
// float TimeEspera;
//
//} Serie;
/*************************/


namespace gb { //abreviacao para gymBoard

//variaveis

extern int qtdSeries;
extern int repeticoes;
extern int intervalo;
extern int TimeVolta;
extern int TimeIda;
extern int TimeEspera;

extern int serieAtual;
extern char direcao;
extern bool firstTime;

extern Adafruit_NeoPixel fita;

}


#endif
