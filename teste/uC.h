#ifndef UC_H
#define UC_H

#include "gymBoard.h"
#include "leitura.h"

uint8_t sleep(); //inclui estado de sleep
void uC_config(); //estado inicial de setup
static void setNextStateVariables();
void updateEEPROM(uint8_t estado);

#endif
