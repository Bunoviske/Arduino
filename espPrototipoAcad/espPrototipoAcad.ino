#include "comApp.h"
#include "time.h"

/***************

  Comentarios do codigo

 **************/

//do pino D0 para o pino D9 representa a descida do movimento. Esp fica encima.


void setup() {

  delay(1000);
  Serial.begin(115200);
  //  //Serial.println();
  //  //Serial.print("Configuring access point...");
  //
  //  //IPAddress ip(192, 168, 5, 2);
  //  //WiFi.config(ip);
  //  //WiFi.mode(WIFI_AP);
  //
  //  WiFi.softAP(ssid, password);
  //
  //  IPAddress myIP = WiFi.softAPIP();
  //  //Serial.print("AP IP address: ");
  //  //Serial.println(myIP);
  //  server.begin();
  //  //Serial.println("ESP server started");

  definePins();

  Serial.println("\nComeçando o codigo");

  delay(1000);
}

void loop() {

 /**************
    Comunicacao com usuario manualmente e treino
  **************/

  getUserInfo();
  train();


  /**************
     Comunicacao com app
  **************/

  //  client = server.available();
  //
  //  if (!client) {
  //    return;
  //  }
  //  //funcao so é chamada se houver cliente
  //  handleAppClient();

}



