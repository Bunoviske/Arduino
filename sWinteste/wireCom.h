#ifndef WIRECOM_H
#define WIRECOM_H

#include <Wire.h>
#include "sWinBoard.h"

/************************************** I2C ADDRESS **************************************/

#define I2C_ADDRESS  1 // MUDAR CONFORME A RAIA

/************************************** I2C ADDRESS **************************************/


void receiveEvent(int howmany);
void requestEvent();
void configWire();

#endif
