#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <aJSON.h> //Carrega a biblioteca JSON
//#include <DHT.h> //Carrega a biblioteca DHT


String requestPOST();
String requestGET();
void parseJson(char *jsonString);

/*
 * * -----------------------------------------------------------------------------------------
 * ETHERNET
 * * -----------------------------------------------------------------------------------------
*/

//MAC do Arduino
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(172,16,6,159); // aviseja.com.br
//char server[] = "http://192.168.25.7"; // Aqui se for DNS

//Porta do servidor
const int port = 80;

// Instancia objeto Client
EthernetClient client;


/*
 * * -----------------------------------------------------------------------------------------
 * Globais
 * * -----------------------------------------------------------------------------------------
*/

//Controle de comunicação
boolean startRead = false;

//JSON
String jsonString = "";
String jsonStringCurr = "";
int adesaoDiaCurr = 0;

/*
 * * -----------------------------------------------------------------------------------------
 * GPIO's
 * * -----------------------------------------------------------------------------------------
*/
int led = 13;

//Define a ligação ao pino de dados do sensor
#define DHTPIN A5

//Define o tipo de sensor DHT utilizado
#define DHTTYPE DHT11

//DHT dht(DHTPIN, DHTTYPE);

/*
 * * -----------------------------------------------------------------------------------------
 *  Setup - Configuração
 * * -----------------------------------------------------------------------------------------
*/
void setup() {

  Serial.begin(9600);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  Serial.println("Iniciando DHCP");

  // Iniciando a conexao ethernet e recebendo o IP dinamicamente
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Erro na rede");
    Serial.println("Reiniciar 5 seg.");
    // Problemas na conexao ethernet, verifique a sua rede
    delay(1000);
    setup();
  }

  Serial.println(Ethernet.localIP());

}

/*
 * * -----------------------------------------------------------------------------------------
 *  Loop Infinito
 * * -----------------------------------------------------------------------------------------
*/

void loop()
{

  //Le valor da umidade
  //float h = dht.readHumidity();
  //Le o valor da temperatura
  //float t = dht.readTemperature(); //Le o valor da temperatura


//  jsonString = requestGET();

   jsonString = requestPOST();

  if(!jsonString.equals("")){
    if(!jsonString.equals(jsonStringCurr)){

      //Atualiza a variavel que armazena o JSON atual
      jsonStringCurr = jsonString;
      char jsonChar[jsonString.length()];
      jsonString.toCharArray(jsonChar, jsonString.length() + 1);
      parseJson(jsonChar);
      jsonString = "";

    }
  }
}


/*
 * * -----------------------------------------------------------------------------------------
 * Metodos Privados
 * * -----------------------------------------------------------------------------------------
*/
String requestPOST(){

  Serial.println("conectando ao server...");

  // Iniciando conexao com o servidor
  if (client.connect(server, port)) {
    Serial.println("conectado");

    //post de cadastro device
    //String POST = "/raise/devices?name=testeArduino&description=arduino&udn=1234567890&type=deviceRFID&manufacturer=fabricanteARduino&manufacturer_url=www.arduino.com&serial=serial&upc=1232137214&version_major=10&version_minor=1&xml_uri=xml";

   // post de cadastro de action
   // String POST = "/raise/actions?name=TESTEDENOVO";

    //put
    String POST = "/devices?name=Bruno&description=oi&chipset_id=123&processor_id=123&mac=123&serial=123";

    client.print("POST ");
    client.print(POST);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(POST.length());
    client.println();

    //Aguardando conexao
    while(!client.available()){
      delay(1);
    }

    //Percorre os caracteres do envelope HTTP do servidor e armazena na String apenas o conteudo JSON
    while (client.available()) {
    char c = client.read();
    if( c == '{' || c== '[' ) {
      startRead = true;
    }
    if ( startRead ) {
      jsonString += c;
    }
  }

  //Reseta a flag de leitura de conteudo JSON
  startRead = false;

  }
 else {
    // Caso nao ocorra conexao
    Serial.println("conexao falhou");
  }

  //Aguarda a desconexao com o servidor
  while(client.connected()){
    delay(1);
  }

  // Finaliza o socket de conexao
  if (!client.connected()) {
    Serial.println();
    Serial.println("desconectando...");
    client.stop();

  }

  Serial.println(jsonString);
  return jsonString;
}



String requestGET(){

 Serial.println("conectando ao server...");

  // Iniciando conexao com o servidor
  if (client.connect(server, port)) {
    Serial.println("connectado");
    // GET no PATH / do servidor

    String GET = "/raise/";

    client.print("GET ");
    client.print(GET);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    //Aguardando conexao
    while(!client.available()){
      delay(1);
    }

    //Percorre os caracteres do envelope HTTP do servidor e armazena na String apenas o conteudo JSON
    while (client.available()) {
    char c = client.read();
    if( c == '{' || c== '[' ) {
      startRead = true;
    }
    if ( startRead ) {
      jsonString += c;
    }
  }

  //Reseta a flag de leitura de conteudo JSON
  startRead = false;

  }
 else {
    // Caso nao ocorra conexao
    Serial.println("conexao falhou");
  }

  //Aguarda a desconexao com o servidor
  while(client.connected()){
    delay(1);
  }

  // Finaliza o socket de conexao
  if (!client.connected()) {
    Serial.println();
    Serial.println("desconectando...");
    client.stop();

  }

  Serial.println(jsonString);
  return jsonString;
}

void parseJson(char *jsonString)
{
    //Inicializa o objeto Pai
    aJsonObject* root = aJson.parse(jsonString);

    if (root != NULL) {

        aJsonObject* token = aJson.getObjectItem(token, "token");

       /* aJsonObject* code = aJson.getObjectItem(code, "code");

        //Caputura o objeto name
        aJsonObject* message = aJson.getObjectItem(message, "message");

        Serial.print("Id: ");
        Serial.print(id->valuestring);
        Serial.println();

        Serial.print("Code: ");
        Serial.print(code->valuestring);
        Serial.println();

        Serial.print("Message: ");
        Serial.print(message->valuestring);
        Serial.println();

        delay(5000);
*/
    }

    //Deleta o objeto apos a utilizacao para liberacao de memoria
    aJson.deleteItem(root);
}
