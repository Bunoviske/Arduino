#ifndef CONFIG_H
#define CONFIG_H

/************************************** I2C ADDRESS **************************************/

#define I2C_ADDRESS  1 // MUDAR CONFORME A RAIA

/************************************** I2C ADDRESS **************************************/


#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <EEPROM.h>


#include "FastLED.h"

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
#define ATRIBUICAO     2

//pinos
#define PIN            6
#define INTERRUPT      2
#define TRANSISTOR     5
#define LED            3

//Tamanho dos vetores de series
#define TAM_MAX        1 //otimizar tamanho das variaveis para caber mais series e trocar o vetor dinamico da struct "Piscina" caso precise
//arduino mega aguenta TAM_MAX = 100 ocupando 46% de variavel global

//#define DEBUG

/*************************
 * Variaveis de treino */

typedef struct {

  float trecho[5];
  float tempo[5];  //armazenar os tempos em segundos

} Piscina;

typedef struct {

  Piscina ida;
  Piscina volta;
  int repeticao;
  int distancia;
  float descanso_serie;  //armazenar os tempos em segundos
  float descanso_distancia;

} Serie;

extern int NUMPIXELS; //está sendo considerado uma fita que tem 30 leds/m e que é alimentada por 12V (controle de 3 leds por vez)
extern CRGB fita[250];

//qtd de series
extern uint8_t qtd_serie; //para manter a parametrizacao, foi somado um ao valor de qtd_serie na funcao de treino

//qtd de trecho na ida e na volta - MAX 5 !!!
extern uint8_t qtdIda;
extern uint8_t qtdVolta;

extern char direcao; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta
extern uint16_t ultima_pos; //variavel que auxilia o controle dos trechos de cada piscina
//char buffer[] = "4:100:60:20:30/20:30|10:40/10:40>120"; //4:10:6:2:3/3:2|1:4/4:1>12
extern char buffer[250];
extern Serie series[TAM_MAX];
extern uint8_t tamPiscina;
extern uint16_t contGlobal;
extern uint8_t estado;

/*************************/

/*************************
 * Variaveis de comunicacao/tempo */

extern uint8_t received;
extern uint8_t sendState;
extern bool startNow;
extern bool startProgram;
extern bool sendTrainTime;
extern bool sendSeriesTime;
extern bool rcvTime;
extern float tempo;

/*************************/

/************************************** METODOS **************************************/

void initialConfig();
void sleep();
void pause();
void Stop();
void readEEPROM();

#endif
