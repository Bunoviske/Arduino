#include "sWinBoard.h"

namespace sb { //abreviacao para sWinBoard

/*************************
   Variaveis de treino/parsing */

volatile uint16_t NUMPIXELS; //int NUMPIXELS = tamPiscina * 10;
char buffer[BUFFER_LENGTH];

uint8_t qtdIda = 0;
uint8_t qtdVolta = 0;
Serie serieAtual;

volatile uint8_t subState = 0;
volatile uint8_t goPause = 0;

/*************************
   Variaveis de comunicacao/tempo */

volatile uint8_t received = 0;
volatile uint8_t goSleep = 0;
volatile uint8_t startNow = 0;

uint8_t sendState = 0;

char direcao = 'i'; //variavel que determina se é ida ou volta ------> 'i' = ida e 'v' = volta
uint8_t flagSinalizaComeco = 1; 
int atual_pos = 0; //ultima posicao do led aceso. 

/*************************************/

}


