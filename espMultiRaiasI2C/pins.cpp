#include "pins.h"

uint8_t arduinoPins[MaxRaias];

void definePins(){
  for(uint8_t i = 0; i < MaxRaias; i++){
    if(i == 0) arduinoPins[i] = D0;
    else if(i == 1) arduinoPins[i] = D3;
    else if(i == 2) arduinoPins[i] = D4;
    else if(i == 3) arduinoPins[i] = D5;
    else if(i == 4) arduinoPins[i] = D6;
    else if(i == 5) arduinoPins[i] = D7;
    else if(i == 6) arduinoPins[i] = D8;
    else if(i == 7) arduinoPins[i] = D9;
  }
}

