#include "gymBoard.h"

namespace gb {

char direcao = 'i';
bool isSleeping = 1;
bool firstTime = 1;
int serieAtualCont = 0;


//uint16_t NUMPIXELS = MAX_LEDS; //int NUMPIXELS = tamPiscina * 10;

Serie serieAtual;
uint8_t subState;
uint8_t goPause;
uint8_t startNow;
uint8_t flagSinalizaComeco = 1; 
int atual_pos = 0; //ultima posicao do led aceso.
CRGB fita[MAX_LEDS]; //modo normal ---->usar BRG. na memoria sao (MAX_LEDS*3) bytes IMPORTANTE: variaveis que acessam o vetor (index) precisam ser INT!!!!!!
 

}


