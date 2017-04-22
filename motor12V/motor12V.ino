int PinoVelocidade = 3;   
int Entrada1 = 8;
int Entrada2 = 9; 
int velocidade;

void setup()  
{  
  //Define os pinos como saida  
  pinMode(PinoVelocidade, OUTPUT);  
  pinMode(Entrada1, OUTPUT);  
  pinMode(Entrada2, OUTPUT);  
}  
   
void loop()  
{  

  digitalWrite(Entrada1, LOW);  
  digitalWrite(Entrada2, HIGH);  
  velocidade = 80;
  for (velocidade=80;velocidade<256;velocidade++){
    analogWrite(PinoVelocidade, velocidade);
    delay(20);
  }
  delay(5000);  
  for (velocidade = 255;velocidade>79;velocidade--){
    analogWrite(PinoVelocidade, velocidade);
    delay(20);
  }
  para_motor();  



  //Aciona o motor no sentido inverso  
  digitalWrite(Entrada1, HIGH);  
  digitalWrite(Entrada2, LOW);  
  velocidade = 80;
  for (velocidade=80;velocidade<256;velocidade++){
    analogWrite(PinoVelocidade, velocidade);
    delay(20);
  }
  delay(5000);
  for (velocidade = 255;velocidade>79;velocidade--){
    analogWrite(PinoVelocidade, velocidade);
    delay(20);
  }
  para_motor();
    
}  

    
void para_motor()  
{  
  digitalWrite(Entrada1, LOW);  
  digitalWrite(Entrada2, LOW);  
  delay(2000);  
}
/*

int PinoVelocidade = 3; //Ligado ao pino 1 do L293D  
int Entrada1 = 8; //Ligado ao pino 2 do L293D  
int Entrada2 = 9; //Ligado ao pino 7 do L293D  
   
void setup()  
{  
  //Define os pinos como saida  
  pinMode(PinoVelocidade, OUTPUT);  
  pinMode(Entrada1, OUTPUT);  
  pinMode(Entrada2, OUTPUT);  
}  
   
void loop()  
{  
  //Define a velocidade de rotacao  
  int velocidade = 255;  
  analogWrite(PinoVelocidade, velocidade);   

  //Aciona o motor  
  digitalWrite(Entrada1, LOW);  
  digitalWrite(Entrada2, HIGH);  
  delay(5000);  

  //Chama a rotina de parada do motor  
  para_motor();  

  //Aciona o motor no sentido inverso  
  digitalWrite(Entrada1, HIGH);  
  digitalWrite(Entrada2, LOW);  
  delay(5000);  
  para_motor();  
}  
    
void para_motor()  
{  
  digitalWrite(Entrada1, LOW);  
  digitalWrite(Entrada2, LOW);  
  delay(1500);  
}  */
