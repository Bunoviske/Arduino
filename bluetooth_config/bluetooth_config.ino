#include <SoftwareSerial.h>
    
//Define os pinos para a serial   
SoftwareSerial master(2, 3); // RX, TX 

String command = ""; // Stores response of bluetooth device  
            // which simply allows n between each  
            // response.  98d3:31:308382

    
void setup()   
{  
  //Inicia a serial  
  Serial.begin(115200);  
  Serial.println("Digite os comandos AT :");  
  master.begin(38400); 
  pinMode(8,OUTPUT);
  digitalWrite(8,1);
  
}  
    
void loop()  
{  
  // Read device output if available.  
  if (master.available()) 
  {  
     while(master.available()) 
     { // While there is more to be read, keep reading.  
       command += (char)master.read();  
       delay(10);
     }  
   Serial.println(command);
   if (command == "ola"){
    digitalWrite(8,0);
    Serial.print("aperte o botao");
    delay(5000);
    digitalWrite(8,1);  
   }
   command = ""; // No repeats  
  }  
   
  // Read user input if available.  
  if (Serial.available())
  {  
    delay(10); // The DELAY!  
    master.write(Serial.read());  
  }  
}
