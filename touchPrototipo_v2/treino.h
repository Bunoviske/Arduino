#ifndef TREINO_H
#define TREINO_H

#include "gymBoard.h"
#include "leitura.h"

uint8_t treino();
void clearAllLeds();
void initTimer();
void setFinalPos(int16_t pixelsRange);
void setSinalizaComeco();
void setFirstLed();
void setComp();
void logicaIntervalo();

//sub estados
uint8_t pause();
uint8_t logica();
uint8_t treinar();
uint8_t ledConfig();
uint8_t intervalo();
uint8_t sinalizaComeco();

static void setNextStateVariables(uint8_t nextState);
void setNextSubStateVariables(uint8_t nextSubState);



#endif
