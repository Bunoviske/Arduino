#ifndef TREINO_H
#define TREINO_H

/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/adafruit/Adafruit_NeoPixel
*/
#include <Adafruit_NeoPixel.h>

#include "config.h"

void treino();
void velocidade_constante(bool flag);
int comparaCor(float tam_ante, int tempo_ante, float tam_atual, int tempo_atual);
void calculo_pos(float tam_trecho, int *pos_inicial, int *pos_final);
void ledControl(uint16_t delayMs, uint16_t delayUs, int pos_inicial, int pos_final, uint32_t cor);
void sinaliza_comeco();
void configNeopixel();


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

//extern Adafruit_NeoPixel fita;
extern int NUMPIXELS; //está sendo considerado uma fita que tem 30 leds/m e que é alimentada por 12V (controle de 3 leds por vez)
extern uint8_t qtdIda; //qtd de trecho na ida e na volta - MAX 5 !!!
extern uint8_t qtdVolta;
extern char buffer[250];
extern Serie serieAtual;
volatile extern uint16_t contGlobal;
extern uint8_t tamPiscina;
extern uint8_t estado;

volatile extern bool startNow;


#endif
