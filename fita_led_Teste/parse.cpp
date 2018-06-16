#include "parse.h"

/********************

   Manipulacao de dados

 ********************/

volatile bool rcvTime = 0;
volatile bool sendTrainTime = 0;
volatile float tempo = 0;

void pega_serie() {

  serieAtual.repeticao = procuraChar();
  serieAtual.distancia = procuraChar();
  serieAtual.descanso_distancia = procuraChar();

  //PEGA TRECHOS E TEMPOS
  procuraIdaVolta();

  serieAtual.descanso_serie = procuraChar();


#ifdef DEBUG
  Serial.print(F("repeticao:"));
  Serial.println((serieAtual.repeticao));
  Serial.print(F("distancia:"));
  Serial.println((serieAtual.distancia));
  Serial.print(F("descanso_distancia:"));
  Serial.println((serieAtual.descanso_distancia));
  Serial.print(F("descanso serie:"));
  Serial.println((serieAtual.descanso_serie));

  Serial.println(F("Idas trechos"));

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(serieAtual.ida.trecho[i]);
  }

  Serial.println(F("Idas tempos"));

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(serieAtual.ida.tempo[i]);
  }

  Serial.println(F("Voltas trechos"));
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(serieAtual.volta.trecho[i]);
  }

  Serial.println(F("Voltas tempos"));
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(serieAtual.volta.tempo[i]);
  }
#endif

  estado = TREINO;

}

void getTrainTime() {

  tempo = 0;
  float timeAuxIda = 0, timeAuxVolta = 0;


  //percorre todo o treino e identifica o tempo total dele
  while (buffer[contGlobal] != '\0') {

    serieAtual.repeticao = procuraChar();
    serieAtual.distancia = procuraChar();
    serieAtual.descanso_distancia = procuraChar();
    procuraIdaVolta();
    serieAtual.descanso_serie = procuraChar();

    tempo += serieAtual.descanso_serie + ((serieAtual.repeticao - 1) * serieAtual.descanso_distancia);

    timeAuxIda = 0, timeAuxVolta = 0;
    for (int i = 0; i < qtdIda; i++) {
      timeAuxIda += serieAtual.ida.tempo[i];
    }
    //Serial.println(timeAuxIda);

    for (int i = 0; i < qtdVolta; i++) {
      timeAuxVolta += serieAtual.volta.tempo[i];
    }

    //Serial.println(timeAuxVolta);

    if ((serieAtual.distancia / tamPiscina)*serieAtual.repeticao % 2 == 0) { //se for par, mesmo tanto de idas e voltas
      tempo += (((float)serieAtual.distancia / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta) * serieAtual.repeticao);
    }
    else { //como é impar, sera necessario discretizar os tempos da ultima repeticao

      tempo += (((float)serieAtual.distancia / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta) * (serieAtual.repeticao - 1)); //todas repeticoes menos a ultima
      tempo += (((float)(serieAtual.distancia - tamPiscina) / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta)); //todas piscinas menos a ultima
      tempo += timeAuxIda; //ultima piscina. A IDA É SMP ONDE COMECA A PRIMEIRA PISCINA. ENTAO O TEMPO QUE SOBRAR DA ULTIMA PISCINA SMP VAI SER A IDA.
      //      if (direcao == 'i') {
      //        tempo += timeAuxIda; //ultima piscina
      //      }
      //      else {
      //        tempo += timeAuxVolta; //ultima piscina.
      //      }
    }
  }
  Serial.println(tempo);
  //envia o treino para o esp e espera uma resposta de confirmacao, que ira alterar a variavel rcvTime
  sendTrainTime = 1;
  while (rcvTime == 0 ) {
    delay(100);
  }
  rcvTime = 0;
  sendTrainTime = 0;

//  Serial.print(F("Memoria time = "));
//  Serial.println(freeRam());
}

float procuraChar() {

  int i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente

  while (buffer[contGlobal] != ':' && buffer[contGlobal] != '\0' && buffer[contGlobal] != ';') {
    bufferAux[i] = buffer[contGlobal];
    contGlobal++;
    i++;
  }
  bufferAux[i] = '\0';

  if (buffer[contGlobal] != '\0') {
    contGlobal++; //incrementa para sair da condicao de stop
  }

//  Serial.print(F("Memoria procuraChar = "));
//  Serial.println(freeRam());

  return atof(bufferAux);
}

void procuraIdaVolta() {

  int i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  bool sem_volta = 1; //flag para saber se o app mandou a string com as informacoes de ida e volta ou so de ida
  uint8_t contAux = 0;


  /*******
    ATRIBICAO DA IDA
  *******/


  while (buffer[contGlobal] != '/') {
    if (buffer[contGlobal] == ':') {
      bufferAux[i] = '\0';
      serieAtual.ida.trecho[contAux] = atof(bufferAux);
      contGlobal++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = buffer[contGlobal];
      contGlobal++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  serieAtual.ida.trecho[contAux] = atof(bufferAux);
  contGlobal++;
  i = 0;

  qtdIda = contAux + 1;
  contAux = 0;

  while (buffer[contGlobal] != '|' && buffer[contGlobal] != '>') {
    if (buffer[contGlobal] == ':') {
      bufferAux[i] = '\0';
      serieAtual.ida.tempo[contAux] = atof(bufferAux);
      contGlobal++;
      contAux++;
      i = 0;
    } else {
      bufferAux[i] = buffer[contGlobal];
      contGlobal++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  serieAtual.ida.tempo[contAux] = atof(bufferAux);
  contGlobal++;
  i = 0;

  if (buffer[contGlobal - 1] == '|') {
    sem_volta = 0;
  }


  /*******
    ATRIBICAO DA VOLTA
  *******/

  contAux = 0;

  //app mandou a volta na string
  if (sem_volta == 0) {
    while (buffer[contGlobal] != '/') {
      if (buffer[contGlobal] == ':') {
        bufferAux[i] = '\0';
        serieAtual.volta.trecho[contAux] = atof(bufferAux);
        contGlobal++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = buffer[contGlobal];
        contGlobal++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    serieAtual.volta.trecho[contAux] = atof(bufferAux);
    contGlobal++;
    i = 0;

    qtdVolta = contAux + 1;
    contAux = 0;

    while (buffer[contGlobal] != '>') {
      if (buffer[contGlobal] == ':') {
        bufferAux[i] = '\0';
        serieAtual.volta.tempo[contAux] = atof(bufferAux);
        contGlobal++;
        contAux++;
        i = 0;
      } else {
        bufferAux[i] = buffer[contGlobal];
        contGlobal++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    serieAtual.volta.tempo[contAux] = atof(bufferAux);
    contGlobal++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que as idas devem ser atribuidas iguais as voltas
  else {
    for (int i = 0; i < qtdIda; i++) {
      serieAtual.volta.trecho[i] = serieAtual.ida.trecho[i];
      serieAtual.volta.tempo[i] = serieAtual.ida.tempo[i];
    }
    qtdVolta = qtdIda;
  }

//  Serial.print(F("Memoria procuraIdaVolta = "));
//  Serial.println(freeRam());

}


