#include <ESP8266WiFi.h>

const char* ssid     = "FitaLed";
const char* password = "RaiaEsp8266";

const char* host = "192.168.4.1";

WiFiClient client;
unsigned long timeout;


void setup() {
  Serial.begin(115200);
  delay(5000);
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  connectWIFI();

  delay(5000);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.println(host);

  connectServer();

  //client.println("-1:213:4:10:6:2:3/3:2|1:4/4:1>12");

  //msg inicial indicando id
  client.println("213");
  delay(500);

  timeout = millis();
  firstMSG();
  timeout = millis();
}


void loop() {

  if (!client.connected()) {
    Serial.println("Conexao perdida");
    client.stop();
    connectServer();

  }
  else if (millis() - timeout > 5000) {

    Serial.println("Enviando keep alive");
    client.println("oi");
    delay(100);
    timeout = millis();

  }

  
}

void firstMSG() {
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  delay(500);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readString();

  }
  Serial.print(line);

}

void connectWIFI() {
//  Serial.println(WL_DISCONNECTED);
//  Serial.println((int)WiFi.begin(ssid, password));
//  Serial.println((int)WiFi.begin(ssid, password));

  while (WiFi.begin(ssid, password) != 6) {
    delay(500);
    //Connection failed, trying again...
  }

}


void connectServer() {

  while (!client.connect(host, 5000)) {
    //Serial.println("connection failed");
    delay(500);
  }
  Serial.println("connection sucess");
  delay(1000);
}

