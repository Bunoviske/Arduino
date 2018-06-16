#include "comApp.h"

/***************

  Comentarios do codigo

 **************/

//  NAO É POSSIVEL COMPARAR STRINGS! Para pausar a fita (interrupt), manda-se '699' pelo app e para pedir as raias prontas para receber serie, usa-se '23'.
//  para dar stop, manda-se 700 pelo app. para dar toogle no modo sleep, manda-se o tamanho da piscina (numero de dois digitos). para dar start now, ou seja,
//  pular o intervalo entre series, manda-se 701

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

  configGPIO();

  tempo = millis();

  //Wire.setClock(10000);
  Wire.begin();

  Serial.println("\nComeçando o codigo");

  delay(1000);
  sleepAll();
}

void loop() {

  /**************
    Comunicacao com raias
  **************/

  if (millis() - tempo >= 100) {
    readRaias();
    tempo = millis();
    
    //uint32_t free = system_get_free_heap_size();
    //Serial.println(free);
  }

  /**************
     Comunicacao com app
  **************/

  client = server.available();

  if (!client) {
    return;
  }
  //funcao so é chamada se houver cliente
  handleAppClient();

}



