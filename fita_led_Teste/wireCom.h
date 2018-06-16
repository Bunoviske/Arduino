#ifndef WIRECOM_H
#define WIRECOM_H

#include "parse.h"
#include <Wire.h>

void pega_treino();
void receiveEvent(int howmany);
void requestEvent();
void configWire();

#endif
