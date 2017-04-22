#include <SoftwareSerial.h>
    
//Define os pinos para a serial   
SoftwareSerial master(2, 3); // RX, TX 

/*
senha com 10 digitos = 0123456789
BLUETOOTH DEVE SER CONFIGURADO MASTER PARA PAREAR COM IPHONE. O DISPOSITIVO PROCURA PELO CELULAR E PEDE A AUTENTICAÇAO*/
    
void setup()   
{  
  Serial.begin(9600);  
  master.begin(38400); 
  
}  
    
void loop()  
{  

  
  String command = "", resposta = "";

     while(master.available()) {
       
       char z = (char)master.read();
       resposta += z;  
     }
    Serial.print(resposta);
    resposta = "";
  
  
    while(Serial.available()){
        
        char c = Serial.read(); // read the next character.
        command+=c;
      } 
      master.print(command);
      command = "";
}



