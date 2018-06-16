#include "gymBoard.h"

namespace gb {

int qtdSeries;
int serieAtual;
int repeticoes;
int intervalo;
float TimeVolta;
float TimeIda;
float TimeEspera;
char direcao = 'd';
uint8_t leds[MaxLeds];
bool isSleeping = 1;
bool firstTime = 1;
uint8_t subState;
uint8_t modo;
uint8_t goPause;
uint8_t startNow;
}


