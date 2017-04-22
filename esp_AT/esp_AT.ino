#include <SoftwareSerial.h>

float temp;
#define DEBUG true
#define SSID "dlink-9FC8"
#define SENHA "xpqap49352"
int connectionId=0; 


SoftwareSerial esp8266(2,3); // make RX Arduino pin 2, make TX Arduino pin 3.
                             
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200); // your esp's baud rate might be different
  delay(10000);
  sendData("AT+RST\r\n", 2000, DEBUG); // rst
  delay(5000);
  sendData("AT+CIOBAUD=9600\r\n", 2000, DEBUG);




  // Conecta a rede wireless
  sendData("AT+CWJAP=\"dlink-9FC8\",\"xpqap49352\"\r\n", 2000, DEBUG);
  delay(3000);

  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra o endereco IP

  delay(3000);
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Configura para multiplas conexoes
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Inicia o web server na porta 80
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);


//  sendData("AT+RST\r\n",2000,DEBUG); // reset module
//  sendData("AT+CWJAP=\"UIoT\",\"161S9PQ1\"\r\n", 2000, DEBUG);
//  delay(5000);
//  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
//  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
//  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  //sendData("AT+CIPSTART=connectionId,\"TCP\",\"192.168.1.112\",8000\r\n",1000,DEBUG);
//  String cipstart = "AT+CIPSTART=";
//     cipstart += connectionId;
//     cipstart += ",";
//     cipstart += "\"TCP\"";
//     cipstart += ",";
//     cipstart += "\"192.168.1.112\"";
//     cipstart += ",";
//     cipstart += 8000;
//     cipstart +="\r\n";
//     sendData(cipstart,1000,DEBUG);
}

void loop(){
  
}



 String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
}

