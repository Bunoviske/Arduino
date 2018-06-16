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

#define MaxLeds 9

/**********************/

extern uint8_t leds[MaxLeds]; 
void definePins();

#endif

