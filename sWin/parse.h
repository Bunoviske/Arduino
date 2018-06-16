#ifndef PARSE_H
#define PARSE_H
#include "sWinBoard.h"
uint8_t parseTreino();
float procuraChar(uint16_t* cont);
void procuraIdaVolta(uint16_t* cont);
void procuraVoltaIda(uint16_t* cont);
static void setNextStateVariables();
#endif
