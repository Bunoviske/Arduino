#ifndef TIME_H
#define TIME_H

#include "parseTreino.h"

extern char treinos[MaxRaias][2048];
extern uint16_t posicaoTreino[MaxRaias];
extern bool interrupted[MaxRaias]; //indica se a raia esta interrompida ou nao
extern uint8_t tamPiscina[MaxRaias];

extern String data2send; //contem treino enviada pelo app
extern String appMsg;
extern uint8_t idRaia; //id da raia que o app pediu para passar os dados

extern unsigned long tempoInicial[MaxRaias];
extern unsigned long tempoTreino[MaxRaias];
extern unsigned long tempoSerie[MaxRaias];
extern unsigned long tempoSerieOffset[MaxRaias];
extern unsigned long tempoPause[MaxRaias];
extern unsigned long tempo;
extern bool lastDescansoSerie[MaxRaias]; //eh zero se descanso serie da ultima serie for zero e 1 se for >= 5
extern float timeIda, timeVolta;


bool checktempoSerie(uint8_t i);
void changeInitialTime();
void resetTime();
void changePauseTime();
void getTrainTime();
void getSerieTime();


#endif
