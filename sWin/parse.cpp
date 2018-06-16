#include "parse.h"

uint8_t parseTreino() {

  Serial.println(F("Parse!"));

  if (sb::goSleep == 1) return SLEEP;

  uint16_t contador = 0;

  sb::serieAtual.repeticao = procuraChar(&contador);
  sb::serieAtual.distancia = procuraChar(&contador);
  sb::serieAtual.descanso_distancia = procuraChar(&contador);

  //PEGA TRECHOS E TEMPOS
  if (sb::direcao == 'i')
    procuraIdaVolta(&contador);
  else
    procuraVoltaIda(&contador); //atribui a serie ao contrario, para que a ida do usuario seja a volta do arduino!!!!

  sb::serieAtual.descanso_serie = procuraChar(&contador);


#ifdef DEBUG
  Serial.print(F("repeticao:"));
  Serial.println((sb::serieAtual.repeticao));
  Serial.print(F("distancia:"));
  Serial.println((sb::serieAtual.distancia));
  Serial.print(F("descanso_distancia:"));
  Serial.println((sb::serieAtual.descanso_distancia));
  Serial.print(F("descanso serie:"));
  Serial.println((sb::serieAtual.descanso_serie));

  Serial.println(F("Idas trechos"));

  for (int i = 0; i < sb::qtdIda; i++) {
    Serial.println(sb::serieAtual.ida.trecho[i]);
  }

  Serial.println(F("Idas tempos"));

  for (int i = 0; i < sb::qtdIda; i++) {
    Serial.println(sb::serieAtual.ida.tempo[i]);
  }

  Serial.println(F("Voltas trechos"));
  for (int i = 0; i < sb::qtdVolta; i++) {
    Serial.println(sb::serieAtual.volta.trecho[i]);
  }

  Serial.println(F("Voltas tempos"));
  for (int i = 0; i < sb::qtdVolta; i++) {
    Serial.println(sb::serieAtual.volta.tempo[i]);
  }
#endif

  Serial.println(freeMemory());

  setNextStateVariables();
  return TREINO; //proximo estado

}

static void setNextStateVariables() {
  sb::subState = INIT_TREINO;
}


float procuraChar(uint16_t* cont) {

  uint16_t i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente

  while (sb::buffer[*cont] != ':' && sb::buffer[*cont] != '\0' && sb::buffer[*cont] != ';') {
    bufferAux[i] = sb::buffer[*cont];
    (*cont)++;
    i++;
  }
  bufferAux[i] = '\0';

  if (sb::buffer[*cont] != '\0') {
    (*cont)++; //incrementa para sair da condicao de stop
  }

  Serial.println(freeMemory());

  return atof(bufferAux);
}

void procuraIdaVolta(uint16_t* cont) {

  uint16_t i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  uint8_t sem_volta = 1; //flag para saber se o app mandou a string com as informacoes de ida e volta ou so de ida
  uint8_t contAux = 0;


  /*******
    ATRIBICAO DA IDA
  *******/


  while (sb::buffer[*cont] != '/') {
    if (sb::buffer[*cont] == ':') {
      bufferAux[i] = '\0';
      sb::serieAtual.ida.trecho[contAux] = atof(bufferAux);
      (*cont)++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = sb::buffer[*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  sb::serieAtual.ida.trecho[contAux] = atof(bufferAux);
  (*cont)++;
  i = 0;

  sb::qtdIda = contAux + 1;
  contAux = 0;

  while (sb::buffer[*cont] != '|' && sb::buffer[*cont] != '>') {
    if (sb::buffer[*cont] == ':') {
      bufferAux[i] = '\0';
      sb::serieAtual.ida.tempo[contAux] = atof(bufferAux);
      (*cont)++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = sb::buffer[*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  sb::serieAtual.ida.tempo[contAux] = atof(bufferAux);
  (*cont)++;
  i = 0;

  if (sb::buffer[*cont - 1] == '|') {
    sem_volta = 0;
  }


  /*******
    ATRIBICAO DA VOLTA
  *******/

  contAux = 0;

  //app mandou a volta na string
  if (sem_volta == 0) {
    while (sb::buffer[*cont] != '/') {
      if (sb::buffer[*cont] == ':') {
        bufferAux[i] = '\0';
        sb::serieAtual.volta.trecho[contAux] = atof(bufferAux);
        (*cont)++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = sb::buffer[*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    sb::serieAtual.volta.trecho[contAux] = atof(bufferAux);
    (*cont)++;
    i = 0;

    sb::qtdVolta = contAux + 1;
    contAux = 0;

    while (sb::buffer[*cont] != '>') {
      if (sb::buffer[*cont] == ':') {
        bufferAux[i] = '\0';
        sb::serieAtual.volta.tempo[contAux] = atof(bufferAux);
        (*cont)++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = sb::buffer[*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    sb::serieAtual.volta.tempo[contAux] = atof(bufferAux);
    (*cont)++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que as idas devem ser atribuidas iguais as voltas
  else {
    for (uint8_t i = 0; i < sb::qtdIda; i++) {
      sb::serieAtual.volta.trecho[i] = sb::serieAtual.ida.trecho[i];
      sb::serieAtual.volta.tempo[i] = sb::serieAtual.ida.tempo[i];
    }
    sb::qtdVolta = sb::qtdIda;
  }
  Serial.println(freeMemory());
  
}

void procuraVoltaIda(uint16_t* cont) { //serie invertida!!!!!!!!!!!!!!!

  uint16_t i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  uint8_t sem_volta = 1; //flag para saber se o app mandou a string com as informacoes de ida e volta ou so de ida
  uint8_t contAux = 0;


  /*******
    ATRIBICAO DA VOLTA
  *******/


  while (sb::buffer[*cont] != '/') {
    if (sb::buffer[*cont] == ':') {
      bufferAux[i] = '\0';
      sb::serieAtual.volta.trecho[contAux] = atof(bufferAux);
      (*cont)++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = sb::buffer[*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  sb::serieAtual.volta.trecho[contAux] = atof(bufferAux);
  (*cont)++;
  i = 0;

  sb::qtdVolta = contAux + 1;
  contAux = 0;

  while (sb::buffer[*cont] != '|' && sb::buffer[*cont] != '>') {
    if (sb::buffer[*cont] == ':') {
      bufferAux[i] = '\0';
      sb::serieAtual.volta.tempo[contAux] = atof(bufferAux);
      (*cont)++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = sb::buffer[*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  sb::serieAtual.volta.tempo[contAux] = atof(bufferAux);
  (*cont)++;
  i = 0;

  if (sb::buffer[*cont - 1] == '|') {
    sem_volta = 0;
  }


  /*******
    ATRIBICAO DA IDA
  *******/

  contAux = 0;

  //app mandou a volta na string
  if (sem_volta == 0) {
    while (sb::buffer[*cont] != '/') {
      if (sb::buffer[*cont] == ':') {
        bufferAux[i] = '\0';
        sb::serieAtual.ida.trecho[contAux] = atof(bufferAux);
        (*cont)++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = sb::buffer[*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    sb::serieAtual.ida.trecho[contAux] = atof(bufferAux);
    (*cont)++;
    i = 0;

    sb::qtdIda = contAux + 1;
    contAux = 0;

    while (sb::buffer[*cont] != '>') {
      if (sb::buffer[*cont] == ':') {
        bufferAux[i] = '\0';
        sb::serieAtual.ida.tempo[contAux] = atof(bufferAux);
        (*cont)++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = sb::buffer[*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    sb::serieAtual.ida.tempo[contAux] = atof(bufferAux);
    (*cont)++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que as idas devem ser atribuidas iguais as voltas
  else {
    for (uint8_t i = 0; i < sb::qtdVolta; i++) {
      sb::serieAtual.ida.trecho[i] = sb::serieAtual.volta.trecho[i];
      sb::serieAtual.ida.tempo[i] = sb::serieAtual.volta.tempo[i];
    }
    sb::qtdIda = sb::qtdVolta;
  }

}
