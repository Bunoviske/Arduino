#include <ESP8266WiFi.h>
#include <WiFiClient.h>

extern "C" {
#include "user_interface.h"
}

/* Set these to your desired credentials. */
const char *ssid = "FitaLed";
const char *password = "RaiaEsp8266"; //TEM QUE TER MAIS QUE 8 CARACTERES!!!!

WiFiServer server(5000);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

WiFiClient clients[10];

int id[10];

int idRaia; //id da raia que o app pediu para passar os dados


String serie; //contem serie enviada pelo app

int contglobal = 0 ;

unsigned long tempo;


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */

  //IPAddress ip(192, 168, 5, 2);
  //WiFi.config(ip);

  //WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("ESP server started");

  tempo = millis();

}

void loop() {

  if (millis() - tempo > 10000) {
    Serial.println("10s");
    //verifica se os clientes no array estao conectados e se algum nao estive, tira do vetor
    handleConnection( );
    tempo = millis();
  }

  clients[contglobal] = server.available();

  if (!clients[contglobal]) {
    return;
  }
  handleID();

}

void disconnectClient(int i, int *flag) {
  
  clients[i].stop();
  for (int j = i + 1; j < contglobal; j++) {
    clients[j - 1] = clients[j];
  }
  contglobal--;
  (*flag) = (*flag) + 1;
}



//funcao que verifica se os clientes estao conectados e se nao tiverem, tira ele do array de clientes
void handleConnection() {
  int flag = 0;
  for (int i = 0; i < contglobal; i++) {

    if (!clients[i].connected()) {

      Serial.print("Raia ");
      Serial.print(i + flag);
      Serial.println(" Nao Conectada, retirando cliente...");
      disconnectClient(i, &flag);
      i--;


    }
    else {
      //se tiver algum dado para ler, quer dizer que a conexao entre servidor e cliente ainda está ativa
      if (clients[i].available()) {
        String aux = "";
        while (clients[i].available()) {
          aux += (String)clients[i].read();
        }
        //debug
        Serial.println(aux);

      }
      else {
        //raia nao enviou dados corretamente, entao considera-se que a conexao caiu
        Serial.print("Raia ");
        Serial.print(i + flag);
        Serial.println(" Nao Enviando Dados, retirando cliente...");
        disconnectClient(i, &flag);
        i--;

      }
    }
  }

  //  unsigned char number_client;
  //  struct station_info *stat_info;
  //  struct ip_addr *IPaddress;
  //  IPAddress address;
  //
  //  number_client = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  //  stat_info = wifi_softap_get_station_info();
  //  Serial.print(" Total connected_client are = ");
  //  Serial.println(number_client);
  //  while (stat_info != NULL) {
  //
  //    IPaddress = &stat_info->ip;
  //    address = IPaddress->addr;
  //
  //    Serial.print(" ip adress is = ");
  //    Serial.print((address));
  //
  //    stat_info = STAILQ_NEXT(stat_info, next);
  //
  //  }
  //  wifi_softap_free_station_info();


}

void verifyID() {
  if (contglobal == 0) {
    //manda resposta para app
    clients[contglobal].println("Erro de conexao com raia - sem raias conectadas");
    delay(100);
  }
  else {
    for (int i = 0; i < contglobal; i++) {
      if (id[i] == idRaia) {
        //manda resposta para app
        clients[contglobal].println("conexao com raia ok");
        delay(100);
        //envia a serie para a raia correspondente ao ip
        clients[i].println(serie);
        delay(100);
        return;
      }
    }
    //manda resposta para app
    clients[contglobal].println("Erro de conexao com raia");
    delay(100);
  }
}

void handleMSG() {

  serie = "";
  while (clients[contglobal].available()) {
    char c = (char)clients[contglobal].read();
    serie += (String)c;
  }
  Serial.println(serie);


}


void handleID() {
  // Wait until the client sends some data
  Serial.println("new client");

  // Read the string
  while (clients[contglobal].available() == 0) {
    //Serial.println("Sem dados available");
  }
  delay(500);
  String resposta = "";
  while (clients[contglobal].available()) {
    char c = (char)clients[contglobal].read();
    if (c == ':') {

      idRaia = resposta.toInt();
      Serial.println(idRaia);

      //achou a id da raia. Agr é feita a leitura da serie
      handleMSG();

      //verifica se o ID eh de algum dos clientes conectados
      verifyID();

      clients[contglobal].stop();
      return;
    }
    else {
      resposta += (String)c;
    }
  }

  //se chegar nessa parte do codigo, quer dizer que a comunicacao é com uma das raias
  id[contglobal] = resposta.toInt();
  Serial.println(id[contglobal]);
  clients[contglobal].println("ok"); //manda ok para o cliente
  delay(100);
  contglobal++;

}

