#ifndef PINS
#define PINS

extern "C" {
#include "user_interface.h"
}
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>

/**********************
 * 
 * MUDAR AQUI CASO DESEJAR DIMINUIR A QUANTIDADE MAXIMA DE RAIAS */

#define MaxRaias 1

/**********************/

extern uint8_t arduinoPins[MaxRaias]; 
void definePins();

#endif

