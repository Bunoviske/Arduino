#ifndef UC_H
#define UC_H

#include "gymBoard.h"
#include "leitura.h"

void uC_config(); //estado inicial de setup
static void setNextStateVariables();
void initSD();
void writeSD();
int checkFileNumber();
void errorHalt(char *);

#endif
