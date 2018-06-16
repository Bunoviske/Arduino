#ifndef TREINO_H
#define TREINO_H

#include "gymBoard.h"
#include "leitura.h"

uint8_t train();
void sinalizaComeco();
void clearAll();
void delayGym(uint32_t delayTime, bool printDisplay, int segundo);

#endif
