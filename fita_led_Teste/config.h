#ifndef CONFIG_H
#define CONFIG_H

/************************************** I2C ADDRESS **************************************/

#define I2C_ADDRESS  1 // MUDAR CONFORME A RAIA

/************************************** I2C ADDRESS **************************************/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <EEPROM.h>


/*
   LINK PARA INSTALACAO DA BIBLIOTECA
   https://github.com/McNeight/MemoryFree
*/
#include <MemoryFree.h>

/**************************************

   DEFINES

**************************************/

//estados
#define LEITURA        0
#define TREINO         1
#define ATRIBUICAO     2

//pinos
#define PIN            6
#define INTERRUPT      2
#define TRANSISTOR     5
#define LED            3


//#define DEBUG


/************************************** METODOS **************************************/

void initialConfig();
void sleep();
void pause();
void Stop();
uint8_t readEEPROM();
uint16_t freeRam();

#endif
