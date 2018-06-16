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
void setLedColor(float tam_atual, float tempo_atual, float tam_ante, float tempo_ante);
uint8_t checkIntervalEnd(); //verifica se terminou uma repeticao no estado treinar
uint8_t checkIntervalRep(); //verifica se eh o inicio de uma repeticao (que nao seja a primeira) no estado logica

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
