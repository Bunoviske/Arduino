#ifndef TREINO_H
#define TREINO_H

#include "config.h"

void treino();
void velocidade_constante(float tempo[], float trecho[], bool flag);
int comparaCor(float tam_ante, int tempo_ante, float tam_atual, int tempo_atual);
void calculo_pos(float tam_trecho, int *pos_inicial, int *pos_final);
void ledControl(int delayval, int pos_inicial, int pos_final, uint32_t cor);
void sinaliza_comeco();

#endif
