#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define MaxRaias 1
#define tx 64
#define rx 93

const char *ssid = "FitaLed";
const char *password = "RaiaEsp8266"; //TEM QUE TER MAIS QUE 8 CARACTERES!!!!

WiFiServer server(5000);

WiFiClient client;

bool power[MaxRaias];//se for 1, quer dizer que a raia está ligada. se for 0, desligada

uint8_t readPins[MaxRaias]; //opcao sem mux, ou seja, um pino digital para cada digitalRead

uint8_t interruptPins[MaxRaias]; //opcao sem mux, ou seja, um pino digital para cada digitalWrite

bool state[MaxRaias]; //o arduino deve enviar 1 quando estiver no estado de leitura, para o server ter ctz que pode mandar dado do app
//o app deve habilitar a opcao de stop so dps que a serie for enviada, para evitar choque com o estado de leitura

uint8_t idRaia; //id da raia que o app pediu para passar os dados

String serie; //contem serie enviada pelo app

const int channelTX[] = {0, 4, 5}, channelRX[] = {16, 15, 2};

unsigned long tempo;

/***************

   Comentarios do codigo

 **************/

//     NAO É POSSIVEL COMPARAR STRINGS! Para pausar a fita (interrupt), manda-se '69' pelo app e para pedir as raias ligadas, usa-se '23'

void setup() {
  delay(1000);
  Serial.begin(115200);
  //Serial.println();
  //Serial.print("Configuring access point...");

  //IPAddress ip(192, 168, 5, 2);
  //WiFi.config(ip);
  //WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  //Serial.print("AP IP address: ");
  //Serial.println(myIP);
  server.begin();
  //Serial.println("ESP server started");

  readPins[0] = 14;
  interruptPins[0] = 12;

  configGPIO();

  tempo = millis();


}

void loop() {

  /**************
    Comunicacao com raias
  **************/


  //para saber quais raias estao conectadas, ligar digitalRead em algum pino do esp no 3.3v do uC. Assim, se ele tiver ligado sabe-se se há conexao.
  //'for' que pinga todos as raias ligadas com o mux/demux
  if (millis() - tempo >= 1000) {
    checkPower();
    readClients();
    tempo = millis();
  }

  /**************
     Comunicacao com app
  **************/

  client = server.available();

  if (!client) {
    return;
  }
  //funcao so é chamada se houver cliente
  handleID();



}

void configGPIO() {
  for (uint8_t i = 0; i < MaxRaias; i++) {
    pinMode(readPins[i], INPUT_PULLUP);
    power[i] = 0;
  }
  for (uint8_t i = 0; i < MaxRaias; i++) {
    pinMode(interruptPins[i], OUTPUT);
    digitalWrite(interruptPins[i], HIGH);
  }

  for (int pin = 0; pin < 3; pin++) { // setup select pins
    pinMode(channelRX[pin], OUTPUT);
    pinMode(channelTX[pin], OUTPUT);
  }
}
void checkPower() {
  for (uint8_t i = 0; i < MaxRaias; i++) {
    power[i] = !digitalRead(readPins[i]);
    //Serial.println(power[i]);
  }
}

void checkData(uint8_t i) {
  //selecao mux/demux das raias ligadas
  String aux = "";
  while (Serial.available()) {
    aux = Serial.readString();
  }
  if (aux != "") {

    //debug
    //Serial.println(aux);

    state[i] = aux.toInt(); // aux == 1
    Serial.print("ok"); // msg de volta para arduino
    delay(100);
  }
}

void readClients() {
  //so entra no 'for' se a raia estiver ligada
  for (uint8_t i = 0; i < MaxRaias && power[i]; i++) {
    selectMux(i+1,rx);
    checkData(i);
  }
}

void sendData() {
  //selecionar mux/demux e enviar dado para idRaia
  selectMux(idRaia+1,tx);
  Serial.print(serie);
  delay(100);

  //fazer retorno do arduino avisando que recebeu dados??
}

void enviaRaias() {
  String resposta = "";
  for (uint8_t i = 0; i < MaxRaias && power[i]; i++) {
    resposta += i;
    resposta += ':';
  }
  client.println(resposta); //envia as raias separadas por : para o app
  delay(100);
}

void handleInterrupt() {
  digitalWrite(interruptPins[idRaia] , digitalRead(interruptPins[idRaia]) ^ 1);
}

void selectMux(int raia, int flag)
{
  uint8_t bit1, bit2, bit3;
  switch (raia) {
    case 1:
      bit1 = 0;
      bit2 = 0;
      bit3 = 0;
      break;

    case 2:
      bit1 = 1;
      bit2 = 0;
      bit3 = 0;
      break;

    case 3:
      bit1 = 0;
      bit2 = 1;
      bit3 = 0;
      break;

    case 4:
      bit1 = 1;
      bit2 = 1;
      bit3 = 0;
      break;

    case 5:
      bit1 = 0;
      bit2 = 0;
      bit3 = 1;
      break;

    case 6:
      bit1 = 1;
      bit2 = 0;
      bit3 = 1;
      break;

    case 7:
      bit1 = 0;
      bit2 = 1;
      bit3 = 1;
      break;

    case 8:
      bit1 = 1;
      bit2 = 1;
      bit3 = 1;
      break;

  }
  if (flag == tx) {
    digitalWrite(channelTX[2], bit1);
    digitalWrite(channelTX[1], bit2); // Ativa a saida desejada para cada dado
    digitalWrite(channelTX[0], bit3);
  }
  else if (flag == rx) {
    digitalWrite(channelRX[2], bit1);
    digitalWrite(channelRX[1], bit2); // Ativa a saida desejada para cada dado
    digitalWrite(channelRX[0], bit3);
  }
}

void verifyID() {
  if (power[idRaia]) {
    if ( serie.toInt() == 69  ) {
      handleInterrupt();
      client.println("interrupt ok");
      delay(100);
    }
    else if (state[idRaia]) {
      //manda resposta para app
      client.println("conexao com raia ok");
      delay(100);

      //envia a serie para a raia correspondente ao id
      sendData(); //fazer funcao que manda o dado para a raia 'idRaia'
      state[idRaia] = 0; //volta para zero pois o arduino nao esta no estado de leitura mais
      digitalWrite(interruptPins[idRaia], HIGH); //garante que o estado de interrupt esta desligado

    }
    else {
      //manda resposta para app
      client.println("Raia fora do estado de leitura");
      delay(100);
    }
  }
  else {
    //manda resposta para app
    client.println("Raia desligada");
    delay(100);
  }

}

void handleMSG() {

  serie = "";
  while (client.available()) {
    char c = (char)client.read();
    serie += (String)c;
  }
  //Serial.println(serie);

}


void handleID() {
  //Serial.println("new client");

  // Read the string
  while (client.available() == 0 && client.connected()) {
    //Sem dados available
  }
  delay(500);
  String resposta = "";

  while (client.available()) {
    char c = (char)client.read();
    if (c == ':') {

      //esta sendo considerado smp o envio de uma raia por vez!!!!!!!!!!!

      idRaia = resposta.toInt(); //esta sendo considerado que o app está mandado o dado correto, senao vai dar seg fault
      //Serial.println(idRaia);

      //achou a id da raia. Agr é feita a leitura da serie
      handleMSG();

      //verifica se o ID eh de algum dos clientes conectados
      verifyID();

      client.stop();
      return;
    }
    else {
      resposta += (String)c;
    }
  }
  //app pediu quais raias estao ligadas
  if (resposta.toInt() == 23) {
    enviaRaias();
  }
  else {
    client.println("Erro de sintaxe");
  }
  client.stop();
}


