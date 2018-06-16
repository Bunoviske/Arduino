#include "time.h"

//globais
char treinos[MaxRaias][2048] = {'\0'};
uint16_t posicaoTreino[MaxRaias]; //variavel que auxilia o recebimento de varios treinos
bool interrupted[MaxRaias];
uint8_t tamPiscina[MaxRaias];

String data2send = "";
String appMsg = "";
uint8_t idRaia = 0;

unsigned long tempoInicial[MaxRaias];
unsigned long tempoTreino[MaxRaias];
unsigned long tempoSerie[MaxRaias];
unsigned long tempoSerieOffset[MaxRaias];
unsigned long tempoPause[MaxRaias];
unsigned long tempo = 0;
bool lastDescansoSerie[MaxRaias];
float timeIda = 0, timeVolta = 0;


bool checktempoSerie(uint8_t i) {
  //  Serial.println(millis());
  //  Serial.println(tempoInicial[i]);

  //Serial.println(millis() - tempoInicial[i] - tempoSerieOffset[i]);

  if (interrupted[i] == 0 && tempoSerie[i] <= millis() - tempoInicial[i] - tempoSerieOffset[i]) { //acabou o tempo do treino. necessario resetar os tempos
    //resetTime();
    return true;
  }
  return false; //treino ainda acontecendo. entao, nao deve-se perguntar nd para a raia
}

void changeInitialTime() {
  tempoInicial[idRaia] += millis() - tempoPause[idRaia];
}

void changePauseTime() {
  tempoPause[idRaia] = millis();
}


void resetTime() {
  tempoTreino[idRaia] = 0;
  tempoSerie[idRaia] = 0;
  tempoInicial[idRaia] = 0;
  tempoPause[idRaia] = 0;
  tempoSerieOffset[idRaia] = 0;
  lastDescansoSerie[idRaia] = 0; //variavel que deve ser zerada todo final de treino!!!
}

void getSerieTime() { //percorre apenas a serie  atual que sera enviada!!!

  uint16_t contador = posicaoTreino[idRaia];
  float tempoTreinoAux = 0;

  int repeticao = procuraChar(&contador);
  int distancia = procuraChar(&contador);
  float descanso_distancia = procuraChar(&contador);
  //PEGA TRECHOS E TEMPOS
  procuraIdaVolta(&contador);
  float descanso_serie = procuraChar(&contador);

  tempoTreinoAux += descanso_serie + ((repeticao - 1) * descanso_distancia);
  if ((distancia / (tamPiscina[idRaia]))*repeticao % 2 == 0) { //se for par, mesmo tanto de idas e voltas
    tempoTreinoAux += (((float)distancia / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta) * repeticao);
  }
  else { //como é impar, sera necessario discretizar os tempos da ultima repeticao
    tempoTreinoAux += (((float)distancia / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta) * (repeticao - 1)); //todas repeticoes menos a ultima
    tempoTreinoAux += (((float)(distancia - (tamPiscina[idRaia])) / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta)); //todas piscinas menos a ultima
    tempoTreinoAux += timeIda; //ultima piscina. A IDA É SMP ONDE COMECA A PRIMEIRA PISCINA. ENTAO O TEMPO QUE SOBRAR DA ULTIMA PISCINA SMP VAI SER A IDA.
  }
  tempoSerie[idRaia] = tempoTreinoAux * 1000.;

  if (posicaoTreino[idRaia] == 0) tempoSerie[idRaia] += 5000.; //primeira serie do treino, eh necessario add o primeiro sinalizaComeco!

  if (lastDescansoSerie[idRaia] == 0 && descanso_serie > 0.) tempoSerie[idRaia] -= 5000.;
  else if (lastDescansoSerie[idRaia] == 1 && descanso_serie == 0.) tempoSerie[idRaia] += 5000.;
  
  //else if(lastDescansoSerie[idRaia] == 0 && descanso_serie == 0. && posicaoTreino[idRaia] != 0) //nao faz nada!!!!
  //else (ultimo caso: lastDescansoSerie == 1 && descanso_serie > 0) nao faz nada!!!

  if (descanso_serie == 0.) lastDescansoSerie[idRaia] = 0;
  else  lastDescansoSerie[idRaia] = 1;

  // se houver intervalo entre series, entao esse valor eh maior que 5, indicando que tera sinalizaComeco na proxima serie enviada, indicando que precisa subtrair 5s!
  Serial.println(tempoSerie[idRaia]);
}

void getTrainTime() {

  uint16_t contador = posicaoTreino[idRaia]; // posicao inicial do contador do treino. (representa o comeco da primeira serie do treino)

  float tempoTreinoAux = 0;

  //percorre todo o treino e identifica o tempo total dele
  while (treinos[idRaia][contador] != '\0' && treinos[idRaia][contador] != '-') { //qq um dos caracteres sinaliza final do treino!!!

    int repeticao = procuraChar(&contador);
    int distancia = procuraChar(&contador);
    float descanso_distancia = procuraChar(&contador);
    //PEGA TRECHOS E TEMPOS
    procuraIdaVolta(&contador);
    float descanso_serie = procuraChar(&contador);

    tempoTreinoAux += descanso_serie + ((repeticao - 1) * descanso_distancia);
    if ((distancia / (tamPiscina[idRaia]))*repeticao % 2 == 0) { //se for par, mesmo tanto de idas e voltas
      tempoTreinoAux += (((float)distancia / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta) * repeticao);
    }
    else { //como é impar, sera necessario discretizar os tempos da ultima repeticao
      tempoTreinoAux += (((float)distancia / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta) * (repeticao - 1)); //todas repeticoes menos a ultima
      tempoTreinoAux += (((float)(distancia - (tamPiscina[idRaia])) / (2 * tamPiscina[idRaia])) * (timeIda + timeVolta)); //todas piscinas menos a ultima
      tempoTreinoAux += timeIda; //ultima piscina. A IDA É SMP ONDE COMECA A PRIMEIRA PISCINA. ENTAO O TEMPO QUE SOBRAR DA ULTIMA PISCINA SMP VAI SER A IDA.

    }
  }
  Serial.println(posicaoTreino[idRaia]);
  //IMPORTANTE: nao precisa acrescentar mais 5s do primeiro sinaliza comeco pois a ultima serie sempre tem 5s de descanso == 0s no arduino (transicao entre treinos)
  tempoTreino[idRaia] = (tempoTreinoAux) * 1000.; //acrescenta os primeiros 5 segs de sinaliza_comeco() e converte para ms
  //Serial.println(tempo);

}
