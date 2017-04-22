#include <ESP8266WiFi.h>



/*****************************
  INSTRUÇÕES

  1 - na hora de colocar em modo de gravacao, passar codigo em branco com o esp desligado do arduino
  2 - para fazer upload, após ter feito o procedimento acima, basta colocar o gpio0 em GND e ligar o arduino como bypass (RX -> RX e TX -> TX)
  3 - para o codigo do esp funcionar, basta desligá-lo e depois ligar com o gpio0 em 3.3V (IMPORTANTE).


****************************/

/*
 * 
 * Nesse codigo, o esp serve apenas para enviar/receber dados ao servidor/gateway. 
 * 
 * 
 */

const char* ssid = "UIOT";
const char* PSWD =  "A12818UIOT";
const int porta = 5510;
const char* serverip = "192.168.0.119";

//const char* ssid = "dlink-9FC8";
//const char* PSWD =  "xpqap49352";
//const int porta = 5500;
//const char* serverip = "192.168.0.178";

String resposta = "";
bool flag = 0;

//IPAddress ip(172, 16, 9, 80);

WiFiClient client;


void setup() {
  Serial.begin(9600);
  delay(5000);
  // We start by connecting to a WiFi network


  Serial.print("Connecting to ");
  Serial.println(ssid);


  //  Serial.println(WiFi.begin(ssid, PSWD));
  // delay(5000);

  while (WiFi.begin(ssid, PSWD) != 6) {
    delay(500);
    //Connection failed, trying again...
  }
  delay(5000);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.println(serverip);

  while (!client.connect(serverip, porta)) {
    delay(500);
    //Connection failed, trying again...
  }

  Serial.println("Conectado no server");

  //Serial.print("ok"); //mensagem de confirmação para sincronizar com arduino
}



void loop() {


  while (Serial.available()) {
    resposta = Serial.readString();
    flag = 1;
  }
  if (flag) {
    if (resposta == "get_ip") {

      IPAddress ip = WiFi.localIP();
      //converte ip para string
      String ip_string;

      for (int i = 0; i < 4; i++) {
        ip_string += i ? "." + String(ip[i]) : String(ip[i]);
      }
      Serial.print(ip_string);

      flag = 0;
    }
    else {

      //conecta no servidor se nao houver conexao
      if (!client.connected()) {
        client.stop();
        while (!client.connect(serverip, porta)) {
          delay(500);
          //Connection failed, trying again...
        }
      }

      client.print(resposta);
      resposta = "";

      unsigned long previous_time = millis();
      while ((millis() - previous_time) <= 3000) {

        if (client.available()) {
          //le mensagem do servidor
          resposta = client.readString();

          //manda de volta para o arduino a mensagem do servidor
          Serial.print(resposta);

          break;
        }


      }
      flag = 0;

      //desconecta do servidor
      //client.stop();
    }
  }

}
