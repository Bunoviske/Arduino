#ifndef GYMBOARD_H
#define GYMBOARD_H

#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <Arduino.h>
#include <EEPROM.h>

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/McNeight/MemoryFree
*/
#include <MemoryFree.h>

/**************************************

   DEFINES

**************************************/

//estados
#define LEITURA        0
#define TREINO         1
#define TURNOFF        2

#define GET_USER_INFO_MAIN           7
#define GET_USER_INFO_START          8
#define GET_USER_INFO_PERSONALIZADO  9

#define MaxLeds 15
#define ledsSinalizaComeco 10

//#define debug


namespace gb { //abreviacao para gymBoard

//variaveis

extern int qtdSeries;
extern int serieAtual;
extern int repeticoes;
extern int intervalo;
extern float TimeIda;
extern float TimeVolta;
extern float TimeEspera;
extern char direcao;
extern uint8_t leds[MaxLeds]; 
extern bool isSleeping;
extern bool firstTime;
extern uint8_t subState;

extern uint8_t modo;
extern uint8_t goPause;
extern uint8_t startNow;


}


#endif
