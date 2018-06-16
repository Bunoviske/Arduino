#include "parseTreino.h"

void saveTreino() {

  int i;
  for (i = 0; i < appMsg.length() ; i++) {
    treinos[idRaia][i] = appMsg[i];
  }
  treinos[idRaia][i] = '\0';
  posicaoTreino[idRaia] = 0;
  Serial.println(treinos[idRaia]);
}

uint8_t getSerie() { //retorna true se tiver serie a ser enviada e false caso nao tenha. A serie a ser enviada estara armazenada em data2send

  uint8_t retorno = 1; //retorna 1 caso nao entra nos dois testes. representa o caso de uma serie que nao seja a primeira do treino

  if (treinos[idRaia][posicaoTreino[idRaia]] == '\0') {
    resetTime();
    return false; //acabaram todos os treinos dessa raia
  }
  if (posicaoTreino[idRaia] == 0 || treinos[idRaia][posicaoTreino[idRaia]] == '-' ) { //pega o proximo treino!
    if (treinos[idRaia][posicaoTreino[idRaia]] == '-') posicaoTreino[idRaia]++; //avanca o contador para o proximo treino caso este nao seja o primeiro!
    resetTime();
    getTrainTime(); //pega o tempo do novo treino
    retorno = 2; //indica que eh o comeco de um novo treino, e deve-se esperar a acao do usuario para despausar.
  }

  //pega a proxima serie!
  
  getSerieTime(); //tempo da serie
  
  bool flag = 0;
  char c;
  data2send = "";

  while (flag == 0) {
    c = treinos[idRaia][posicaoTreino[idRaia]];
    if (c == '-' || c == ';' || c == '\0') { //caracteres que sinalizam final de serie
      flag = 1;
      if (c == ';') {
        posicaoTreino[idRaia]++;
      }
    }
    else {
      data2send += c;
      posicaoTreino[idRaia]++;
    }
  }
  data2send += '&'; //marcador que identifica que a string acabou no protocolo I2C
  Serial.println(data2send);
  return retorno; //data2send tem um novo treino para enviar
}

float procuraChar(uint16_t* cont) {

  uint16_t i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros/floats temporariamente

  while (treinos[idRaia][*cont] != ':' && treinos[idRaia][*cont] != '\0' && treinos[idRaia][*cont] != ';' && treinos[idRaia][*cont] != '-') {
    bufferAux[i] = treinos[idRaia][*cont];
    (*cont)++;
    i++;
  }
  bufferAux[i] = '\0';

  if (treinos[idRaia][*cont] != '\0' && treinos[idRaia][*cont] != '-') {
    (*cont)++; //incrementa para sair da condicao de stop
  }
  return atof(bufferAux);

}
void procuraIdaVolta(uint16_t* cont) {

  uint16_t i = 0;
  char bufferAux[10]; //nao precisa ter mts caracteres, ja que ira armazenar inteiros temporariamente
  uint8_t sem_volta = 1; //flag para saber se o app mandou a string com as informacoes de ida e volta ou so de ida

  //IMPORTATE: variaveis globais!!!!
  timeIda = 0; timeVolta = 0; //zerar variaveis globais de retorno da funcao procuraIdaVolta


  /*******
    ATRIBICAO DA IDA
  *******/


  while (treinos[idRaia][*cont] != '/') {
    if (treinos[idRaia][*cont] == ':') { //atribuicao dos trechos de ida, entao nao faz nada!
      bufferAux[i] = '\0';
      (*cont)++;
      i = 0;
    } else {
      bufferAux[i] = treinos[idRaia][*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0'; //atribuicao dos trechos de ida, entao nao faz nada!
  (*cont)++;
  i = 0;


  while (treinos[idRaia][*cont] != '|' && treinos[idRaia][*cont] != '>') {
    if (treinos[idRaia][*cont] == ':') {
      bufferAux[i] = '\0';
      timeIda += atof(bufferAux);
      (*cont)++;
      i = 0;
    } else {
      bufferAux[i] = treinos[idRaia][*cont];
      (*cont)++;
      i++;
    }
  }

  bufferAux[i] = '\0';
  timeIda += atof(bufferAux);
  (*cont)++;
  i = 0;

  if (treinos[idRaia][*cont - 1] == '|') {
    sem_volta = 0;
  }


  /*******
    ATRIBICAO DA VOLTA
  *******/


  //app mandou a volta na string
  if (sem_volta == 0) {
    while (treinos[idRaia][*cont] != '/') {
      if (treinos[idRaia][*cont] == ':') { //atribuicao dos trechos de volta. Nao faz nada!
        bufferAux[i] = '\0';
        (*cont)++;
        i = 0;
      } else {
        bufferAux[i] = treinos[idRaia][*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    (*cont)++; //atribuicao dos trechos de volta. Nao faz nada!
    i = 0;


    while (treinos[idRaia][*cont] != '>') {
      if (treinos[idRaia][*cont] == ':') {
        bufferAux[i] = '\0';
        timeVolta += atof(bufferAux);
        (*cont)++;
        i = 0;
      } else {
        bufferAux[i] = treinos[idRaia][*cont];
        (*cont)++;
        i++;
      }
    }

    bufferAux[i] = '\0';
    timeVolta += atof(bufferAux);
    (*cont)++;
    i = 0;

  }

  // app so mandou as idas. Isso significa que o tempo de ida eh o igual o tempo de volta
  else {
    timeVolta = timeIda;
  }
}

