#ifndef PARSE_H
#define PARSE_H

#include "treino.h"

void pega_serie();
float procuraChar();
void procuraIdaVolta();
void getTrainTime();

volatile extern bool rcvTime;
volatile extern bool sendTrainTime;
volatile extern float tempo;

#endif
