#include "gymBoard.h"

namespace gb {

char direcao = 'i';
bool firstTime = 1;
uint8_t serieAtualCont = 0;


//uint16_t NUMPIXELS = MAX_LEDS; //int NUMPIXELS = tamPiscina * 10;

Serie serieAtual;
uint8_t subState;
uint8_t goPause;
uint8_t startNow;
uint8_t flagSinalizaComeco = 1; 
int atual_pos = 0; //ultima posicao do led aceso.
Adafruit_NeoPixel fita = Adafruit_NeoPixel(MAX_LEDS, PIN_WS2811, NEO_GRB + NEO_KHZ400);

}


