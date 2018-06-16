#include "pins.h"

uint8_t leds[MaxLeds];

void definePins() {
  for (uint8_t i = 0; i < MaxLeds; i++) {
    if (i == 0) leds[i] = D0;
    else if (i == 1) leds[i] = D1;
    else if (i == 2) leds[i] = D2;
    else if (i == 3) leds[i] = D3;
    else if (i == 4) leds[i] = D4;
    else if (i == 5) leds[i] = D5;
    else if (i == 6) leds[i] = D6;
    else if (i == 7) leds[i] = D7;
    else if (i == 8) leds[i] = D8;
  }

  for (uint8_t i = 0; i < MaxLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

}



