#ifndef TREINO_H
#define TREINO_H

#include "gymBoard.h"
#include "leitura.h"

uint8_t treino();
void sinalizaComeco();
void clearAllLeds();
void delayGym(uint32_t delayTime, bool printDisplay, int segundo);

#endif
