#include "parse.h"

/********************

   Manipulacao de dados

 ********************/


void pega_serie() {

  series[qtd_serie].repeticao = procuraChar();
  series[qtd_serie].distancia = procuraChar();
  series[qtd_serie].descanso_distancia = procuraChar();

  //PEGA TRECHOS E TEMPOS
  procuraIdaVolta();

  series[qtd_serie].descanso_serie = procuraChar();


#ifdef DEBUG
  Serial.print(F("repeticao:"));
  Serial.println((series[qtd_serie].repeticao));
  Serial.print(F("distancia:"));
  Serial.println((series[qtd_serie].distancia));
  Serial.print(F("descanso_distancia:"));
  Serial.println((series[qtd_serie].descanso_distancia));
  Serial.print(F("descanso serie:"));
  Serial.println((series[qtd_serie].descanso_serie));

  Serial.println(F("Idas trechos"));

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(series[qtd_serie].ida.trecho[i]);
  }

  Serial.println(F("Idas tempos"));

  for (int i = 0; i < qtdIda; i++) {
    Serial.println(series[qtd_serie].ida.tempo[i]);
  }

  Serial.println(F("Voltas trechos"));
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(series[qtd_serie].volta.trecho[i]);
  }

  Serial.println(F("Voltas tempos"));
  for (int i = 0; i < qtdVolta; i++) {
    Serial.println(series[qtd_serie].volta.tempo[i]);
  }
#endif

  estado = TREINO;

}

void getTrainTime() {

  tempo = 0;
  float timeAuxIda = 0, timeAuxVolta = 0;


  //percorre todo o treino e identifica o tempo total dele
  while (buffer[contGlobal] != '\0') {

    series[qtd_serie].repeticao = procuraChar();
    series[qtd_serie].distancia = procuraChar();
    series[qtd_serie].descanso_distancia = procuraChar();
    procuraIdaVolta();
    series[qtd_serie].descanso_serie = procuraChar();

    tempo += series[qtd_serie].descanso_serie + ((series[qtd_serie].repeticao - 1) * series[qtd_serie].descanso_distancia);

    timeAuxIda = 0, timeAuxVolta = 0;
    for (int i = 0; i < qtdIda; i++) {
      timeAuxIda += series[qtd_serie].ida.tempo[i];
    }
    //Serial.println(timeAuxIda);

    for (int i = 0; i < qtdVolta; i++) {
      timeAuxVolta += series[qtd_serie].volta.tempo[i];
    }

    //Serial.println(timeAuxVolta);

    if ((series[qtd_serie].distancia / tamPiscina)*series[qtd_serie].repeticao % 2 == 0) { //se for par, mesmo tanto de idas e voltas
      tempo += (((float)series[qtd_serie].distancia / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta) * series[qtd_serie].repeticao);
    }
    else { //como é impar, sera necessario verificar se tem uma ida ou volta a mais

      tempo += (((float)series[qtd_serie].distancia / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta) * (series[qtd_serie].repeticao - 1));
      tempo += (((float)(series[qtd_serie].distancia - tamPiscina) / (2 * tamPiscina)) * (timeAuxIda + timeAuxVolta)); //todas piscinas menos a ultima
      tempo += timeAuxIda; //ultima piscina. A IDA É SMP ONDE COMECA A PRIMEIRA PISCINA. ENTAO O TEMPO QUE SOBRAR DA ULTIMA PISCINA SMP VAI SER A IDA.
      //      if (direcao == 'i') {
      //        tempo += timeAuxIda;
      //      }
      //      else {
      //        tempo += timeAuxVolta;
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

  Serial.print(F("Memoria time = "));
  Serial.println(freeMemory());
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

  Serial.print(F("Memoria procuraChar = "));
  Serial.println(freeMemory());

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
      series[qtd_serie].ida.trecho[contAux] = atof(bufferAux);
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
  series[qtd_serie].ida.trecho[contAux] = atof(bufferAux);
  contGlobal++;
  i = 0;

  qtdIda = contAux + 1;
  contAux = 0;

  while (buffer[contGlobal] != '|' && buffer[contGlobal] != '>') {
    if (buffer[contGlobal] == ':') {
      bufferAux[i] = '\0';
      series[qtd_serie].ida.tempo[contAux] = atof(bufferAux);
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
  series[qtd_serie].ida.tempo[contAux] = atof(bufferAux);
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
        series[qtd_serie].volta.trecho[contAux] = atof(bufferAux);
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
    series[qtd_serie].volta.trecho[contAux] = atof(bufferAux);
    contGlobal++;
    i = 0;

    qtdVolta = contAux + 1;
    contAux = 0;

    while (buffer[contGlobal] != '>') {
      if (buffer[contGlobal] == ':') {
        bufferAux[i] = '\0';
        series[qtd_serie].volta.tempo[contAux] = atof(bufferAux);
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
    series[qtd_serie].volta.tempo[contAux] = atof(bufferAux);
    contGlobal++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que as idas devem ser atribuidas iguais as voltas
  else {
    for (int i = 0; i < qtdIda; i++) {
      series[qtd_serie].volta.trecho[i] = series[qtd_serie].ida.trecho[i];
      series[qtd_serie].volta.tempo[i] = series[qtd_serie].ida.tempo[i];
    }

    qtdVolta = qtdIda;

  }

  Serial.print(F("Memoria procuraIdaVolta = "));
  Serial.println(freeMemory());

}


