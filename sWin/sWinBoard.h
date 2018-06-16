#ifndef SWINBOARD_H
#define SWINBOARD_H

#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <Arduino.h>

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
#define PARSING        1
#define TREINO         2
#define SLEEP          3

//sub estados
#define SINALIZA_COMECO 6
#define INTERVALO      5
#define TREINAR        4
#define PAUSE          3
#define STOP           2 //esse estado existe mas nao eh uma funcao
#define LOGICA         1
#define INIT_TREINO    0

//pinos
#define PIN_WS2811     6
#define PIN_ESP        2
#define TRANSISTOR     5
#define LED            3

#define MAX_LEDS       250
#define BUFFER_LENGTH  70

#define DEBUG


/*************************
   Definicoes de variaveis de treino */

typedef struct {
  float trecho[4];
  float tempo[4];  //armazenar os tempos em segundos
} Piscina;

typedef struct {

  Piscina ida;
  Piscina volta;
  uint8_t repeticao; //garantir no app que sao no max 255 repeticoes em uma serie
  uint16_t distancia;
  float descanso_serie;  //armazenar os tempos em segundos
  float descanso_distancia;

} Serie;
/*************************/



namespace sb { //abreviacao para sWinBoard

/*************************
   Variaveis de treino/parsing */


//qtd de trecho na ida e na volta - MAX 5 !!!
extern uint8_t qtdIda;
extern uint8_t qtdVolta;
extern Serie serieAtual;

volatile extern uint16_t NUMPIXELS; //está sendo considerado uma fita que tem 30 leds/m e que é alimentada por 12V (controle de 3 leds por vez)

extern char buffer[BUFFER_LENGTH];

volatile extern uint8_t subState;
volatile extern uint8_t goPause;


/*************************
   Variaveis de comunicacao/tempo */

volatile extern uint8_t received;
volatile extern uint8_t startNow;
volatile extern uint8_t goSleep;

extern uint8_t sendState;

//variaveis do estado treino

extern char direcao; //variavel do estado TREINO que auxilia o estado PARSING e deve ser zerada no sleep
extern uint8_t flagSinalizaComeco; //variavel do estado TREINO que deve ser zerada no sleep
extern int atual_pos; //variavel do estado TREINO que deve ser zerada no sleep

}


#endif
