#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DHT.h>



/*****************************
  INSTRUÇÕES

  1 - na hora de colocar em modo de gravacao, passar codigo em branco com o esp desligado do arduino
  2 - para fazer upload, após ter feito o procedimento acima, basta colocar o gpio0 em GND e ligar o arduino como bypass (RX -> RX e TX -> TX)
  3 - para o codigo do esp funcionar, basta desligá-lo e depois ligar com o gpio0 em 3.3V (IMPORTANTE).


****************************/

#define NUM_SERVICOS 2
#define refresh_rate 20000
#define host_name "Esp-UDP-DHT11"

const char* ssid = "UIOT";
const char* PSWD =  "A12818UIOT";
const int porta = 5005;
const char* serverip = "172.16.9.61";

String mac_aux = "DE:A8:AE:EF:EE:ED";
String nome_servico[NUM_SERVICOS];
String ip_string;


WiFiUDP client;

// DHT
#define DHTPIN   2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);
  delay(5000);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, PSWD);
  delay(3000);

  while (WiFi.begin(ssid, PSWD) != 6) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  IPAddress ip = WiFi.localIP();
  //converte ip para string

  for (int i = 0; i < 4; i++) {
    ip_string += i ? "." + String(ip[i]) : String(ip[i]);
  }

  nome_servico[0] = "temperatura";
  nome_servico[1] = "humidade";

  client.begin(porta);

}



void loop() {

  int data = 0;
  int value[NUM_SERVICOS];
  for (data = 0; data < NUM_SERVICOS; data++) {
    value[data] = readSensor(data);
    String atualizacao = mac_aux;
    atualizacao += ';';
    atualizacao += ip_string;
    atualizacao += ';';
    atualizacao += host_name;
    atualizacao += ';';
    atualizacao += nome_servico[data];
    atualizacao += ';';
    atualizacao += (String)value[data];
    atualizacao += ';';
    atualizacao += refresh_rate / 1000;
    atualizacao += ';';
    atualizacao += 0;
    Serial.println(atualizacao);



    //conecta no servidor
    client.beginPacket(serverip, porta);
    client.write(atualizacao.c_str());
    client.endPacket();

    delay(1000);


    int packetSize = client.parsePacket();
    //se entrar nesse "if", é pq chegou algum pacote de tamanho packetSize
    Serial.println(packetSize);
    if (packetSize) {

      //le mensagem do servidor
      char buffer[255];
      client.read(buffer, 255);


      //manda de volta para o arduino a mensagem do servidor
      Serial.print(buffer);

    }

    //desconecta do servidor
    client.stop();

  }
  delay(refresh_rate);
}

float readSensor(int data)

{
  float value;
  if (data == 0) {
    value = dht.readTemperature();
    Serial.print("Temperatura: ");
    Serial.println(value);
  }
  else {
    value = dht.readHumidity();
    Serial.print("Humidade: ");
    Serial.println(value);

  }
  return value;
}


