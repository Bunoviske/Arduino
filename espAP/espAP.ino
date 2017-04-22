#include <ESP8266WiFi.h>
#include <WiFiClient.h>

/* Set these to your desired credentials. */
const char *ssid = "esp8266";
const char *password = "1234";


WiFiServer server(5000);

void setup() {

 
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */

  //IPAddress ip(192, 168, 5, 2);
  //WiFi.config(ip);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("ESP server started");
}

void loop() {

  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");

  // Read the string
  while (client.available() == 0) {
    //Serial.println("Sem dados available");
  }
  String resposta = "";
  while (client.available()) {
        char c = (char)client.read();
        resposta += (String)c;
    //resposta = client.readString();
  }
  client.println("ok"); //manda ok para servidor
  delay(100);
  //client.flush(); //espera os dados do client serial serem enviados para prosseguir o codigo
  client.stop();


  Serial.println(resposta); //manda o dado para o microcontrolador
}
