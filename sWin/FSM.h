#ifndef FSM_H
#define FSM_H

#include "leitura.h"
#include "treino.h"
#include "parse.h"
#include "uC.h"
#include "wireCom.h"


void initFSM();
uint8_t changeState(uint8_t nextState);


#endif
