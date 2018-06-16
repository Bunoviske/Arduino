#include "parseTreino.h"

#define debug

int qtdSeries;
int repeticoes;
int intervalo;
float TimeSubida;
float TimeDescida;
float TimeEspera;
char direcao;


//global local
String resposta = "";

void getUserInfo() {


  Serial.println("Quantas séries deseja fazer? \n\n");
  wait();
  qtdSeries = resposta.toInt();

  Serial.println("Quantas repetições em cada série? \n\n");
  wait();
  repeticoes = resposta.toInt();

  Serial.println("Qual o intervalo de descanso entre cada série? (Dê a resposta em segundos)\n\n");
  wait();
  intervalo = resposta.toInt();

  Serial.println("O exercício começa com um movimento de descida ou subida? (Responda 'd' para descida e 's' para subida) \n\n");
  wait();
  direcao = (char)resposta[0];

  if (direcao == 'd') {
    Serial.println("Qual o tempo de descida do exercicio? (Dê a resposta em segundos) \n\n");
    wait();
    TimeDescida = resposta.toFloat();

    Serial.println("Qual o tempo de subida do exercicio? (Dê a resposta em segundos)\n\n");
    wait();
    TimeSubida = resposta.toFloat();

    Serial.println("Após completar a descida, deseja esperar quanto tempo para subir novamente? (Dê a resposta em segundos)\n\n");
    wait();
    TimeEspera = resposta.toFloat();

  }
  else {
    Serial.println("Qual o tempo de subida do exercicio? (Dê a resposta em segundos)\n\n");
    wait();
    TimeSubida = resposta.toFloat();

    Serial.println("Qual o tempo de descida do exercicio? (Dê a resposta em segundos)\n\n");
    wait();
    TimeDescida = resposta.toFloat();

    Serial.println("Após completar a subida, deseja esperar quanto tempo para descer novamente? (Dê a resposta em segundos)\n\n");
    wait();
    TimeEspera = resposta.toFloat();
  }

#ifdef debug

  Serial.print("Quantidade de series: "); Serial.println(qtdSeries);
  Serial.print("Quantidade de repeticoes: "); Serial.println(repeticoes);
  Serial.print("Intervalo em segundos: "); Serial.println(intervalo);
  Serial.print("Tempo de subida em segundos: "); Serial.println(TimeSubida);
  Serial.print("Tempo de descida em segundos: "); Serial.println(TimeDescida);
  Serial.print("Tempo de espera em segundos: "); Serial.println(TimeEspera);
  Serial.print("\n\n");
  
#endif

}

void wait() {

  resposta = "";
  while (Serial.available() == 0) {

  }
  while (Serial.available()) {
    resposta = Serial.readString();
  }
}


