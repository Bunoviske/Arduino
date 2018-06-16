#ifndef LEITURA_H
#define LEITURA_H

#include "gymBoard.h"

#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>


//Definicao de cores
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define espaco          12

#define DRIVER_BUG
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin


#define TS_MINX 170 // Use 150 para o 9341
#define TS_MINY 165 // Use 120 para o 9341
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 1
#define MAXPRESSURE 1000

#define MODO_HIPERTROFIA_INICIANTE 1
#define MODO_HIPERTROFIA_AVANCADO 2
#define MODO_RESISTENCIA 3
#define MODO_PERSONALIZADO 4


//metodos
uint8_t leitura();
uint8_t getUserInfoTouch();
uint8_t getUserInfoTouchPersonalizado();
uint8_t getInfoStartTouch(); 
void configTouch();
void initTouch();
void initInicializaComeco();
void updateDescanso(int numero);
void updateRepeticao(uint8_t numero);
void initRepeticao();
void initDescanso();
void initTouchPersonalizado();
void init_info();
static void setNextStateVariables(uint8_t nextState);

#endif
