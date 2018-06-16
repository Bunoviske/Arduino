//#include "comApp.h"
//
////Inicializacao das variaveis globais
//
//char *ssid = "FitaLed12345";
//char *password = "RaiaEsp8266";
//
//WiFiServer server(5000);
//WiFiClient client;
//String raias = "";
//
//void handleAppClient() {
//  //Serial.println("new client");
//
//  while (client.available() == 0 && client.connected()) {
//    //Sem dados available
//    delay(100); //???
//  }
//  //delay(500);
//  raias = "";
//
//  while (client.available()) {
//    char c = client.read();
//    if (c == ':') {
//
//      //leitura da msg do app apos identificar as raias
//      handleMSG();
//
//      //esta sendo considerando que o app está mandando o dado correto, senao vai dar seg fault
//      //funcao que pega os IDs das raias e envia as informacoes onde o app deseja atuar
//      getRaiaId();
//
//      client.stop();
//      return;
//    }
//    else {
//      raias += c;
//    }
//  }
//  //app pediu quais raias estao ligadas
//  if (raias.toInt() == 23) {
//    sendRaias2App();
//  }
//  else {
//    client.println("Erro de sintaxe");
//  }
//  client.stop();
//}
//
//void parseAppMsg() {
//
//  //interrupcao
//  if ( appMsg.toInt() == 699  ) {
//    data2send = "3333&";
//    sendData2Raia(0);
//
//    if (interrupted[idRaia] == 0) {
//      changePauseTime();
//
//      client.println("interrupt ok");
//      delay(50);
//      interrupted[idRaia] = 1; //interrompeu
//    }
//    else {
//      changeInitialTime();
//
//      client.println("desinterrupt ok");
//      delay(50);
//      interrupted[idRaia] = 0; //desinterrompeu
//    }
//  }
//
//  //stop
//  else if ( appMsg.toInt() == 700 ) {
//
//    Serial.println("stop");
//
//    data2send = "4444&";
//    sendData2Raia(0);
//    resetTime();
//    interrupted[idRaia] = 0;
//
//    //reseta todos os treinos enviados para determinada raia, ja que foi dado stop
//    treinos[idRaia][0] = '\0';
//    posicaoTreino[idRaia] = 0;
//
//    client.println("stop ok");
//    delay(50);
//
//  }
//
//  //pular delay entre series
//  else if ( appMsg.toInt() == 701 ) {
//
//    data2send = "1111&";
//    sendData2Raia(0);
//    client.println("startnow ok");
//    delay(50);
//
//
//  }
//
//  //sleep
//  else if ( appMsg.toInt() < 100 && appMsg.toInt() > 0 && appMsg.length() <= 3) { //se for um numero de dois ou menos digitos, eh o tamanho da piscina
//    //obs.: string.length == 3 eh um numero de dois digitos
//    
//    tamPiscina[idRaia] = (uint8_t)appMsg.toInt();
//
//    if (ligado[idRaia] == 0) { //deseja ligar a raia
//      data2send = "2222&"; //indica tirar ou colocar sleep mode
//      sendData2Raia(0);
//      delay(100);
//
//      data2send = appMsg + '&'; //enviar o tamanho da piscina tbm
//      sendData2Raia(0);
//      delay(50); //receber acknowlege dps
//
//    }
//    else { //deseja desligar a raia
//      data2send = "2222&"; //indica tirar ou colocar sleep mode
//      sendData2Raia(0);
//    }
//    resetTime();
//
//    //reseta todos os treinos enviados para determinada raia, ja que foi dado sleep (independente se vai ligar ou desligar o treino eh zerado)
//    treinos[idRaia][0] = '\0';
//    posicaoTreino[idRaia] = 0;
//
//    client.println("sleep ok");
//    ligado[idRaia] = !ligado[idRaia];
//    delay(50);
//  }
//
//  //treino
//  else {
//
//    if (state[idRaia]) {
//
//      saveTreino(); //salva os treinos enviados pelo app na respectiva raia
//
//      getSerie(); //pega o primeira serie do primeiro treino na fila e armazena em data2send
//      sendData2Raia(1); //envia a serie para a raia correspondente ao id
//      state[idRaia] = 0; //volta para zero pois o arduino nao esta no estado de leitura mais
//      interrupted[idRaia] = 1; //primeira serie do treino
//
//      //manda resposta para app
//      client.println("conexao com raia ok");
//      //client.println((float)tempoTreino[idRaia]/1000);
//      delay(50);
//
//    }
//    else {
//      //manda resposta para app
//      client.println("Raia fora do estado de leitura");
//      delay(50);
//    }
//  }
//}
//
//void getRaiaId() {
//
//  int i = 0, j = 0;
//  String aux = "";
//
//  while (i < raias.length()) {
//    if (raias[i] != ',') {
//      aux += raias[i];
//    }
//    else {
//      idRaia = aux.toInt();
//      parseAppMsg();
//      aux = "";
//    }
//    i++;
//  }
//  idRaia = aux.toInt();
//  parseAppMsg();
//
//
//}
//
//void handleMSG() {
//
//  //verificar se deixar string em vez de char array prejudica a memoria
//  //verificar casting
//  appMsg = "";
//  while (client.available()) {
//    char c = client.read();
//    appMsg += c;
//  }
//}
//
//void sendRaias2App() {
//  //manda as raias que estao prontas para receber treino e as que estao ligadas e nao estao prontas
//  String ready2receive = "", raiasOn = "", raiasOff = "", resposta = "";
//  for (uint8_t i = 0; i < MaxRaias; i++) {
//    if (ligado[i] == 1) {
//      if (state[i] == 1) {
//        ready2receive += i;
//        ready2receive += ':';
//      }
//      else { //raias com treino
//        raiasOn += i;
//        raiasOn += ',';
//
//        raiasOn.concat(interrupted[i] == 1 || tempoInicial[i] == 0 ? (float)((tempoPause[i] - tempoInicial[i]) / 1000) : (float)(millis() - tempoInicial[i]) / 1000);
//        //        if (tempoInicial[i] == 0) raiasOn.concat(0); //treino ainda nao comecou
//        //        else if (interrupted[i] == 1)  raiasOn.concat(0); //treino ja comecou e esta interrompido
//        //        else raiasOn.concat((float)(millis() - tempoInicial[i]) / 1000); //treino rodando
//
//        raiasOn += ',';
//        raiasOn.concat((float)tempoTreino[i] / 1000);
//        raiasOn += ':';
//      }
//    }
//    else {
//      raiasOff += i;
//      raiasOff += ':';
//    }
//  }
//  if (ready2receive != "") {
//    ready2receive.remove(ready2receive.length() - 1);
//  }
//  if (raiasOn != "") {
//    raiasOn.remove(raiasOn.length() - 1);
//  }
//  if (raiasOff != "") {
//    raiasOff.remove(raiasOff.length() - 1);
//  }
//  resposta = ready2receive + ';' + raiasOn + ';' + raiasOff;
//  Serial.println(resposta);
//
//  client.println(resposta);
//  delay(100);
//}
