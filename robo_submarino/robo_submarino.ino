/*Pinagem do arduino*/

void direction_input(int);
 
//motor Direito
int EN1 = 2 ;
int EN2 = 4 ;
int velocidadeDireita = 3;
 
//motor Esquerdo
int EN3 = 6 ;
int EN4 = 7 ;
int velocidadeEsquerda = 5;

//motor Central
int EN5 = 9;
int EN6 = 10;
int velocidadeCentral = 8;
 
/*Para movimentar no sentido Horário, exemplo:
   digitalWrite(EN1,HIGH);
   digitalWrite(EN2,LOW);

   Sentido anti-horário seria com HIGH e LOW invertidos
 
 */

//Escolha uma das velocidades:

//Alta
//analogWrite(velocidadeA,230);
 
//Intermediaria
//analogWrite(velocidadeA,150);
 
//Baixa
//analogWrite(velocidadeA,80);
 
 
//Inicializa Pinos
void setup(){
   Serial.begin(9600);

  pinMode(EN1,OUTPUT);
  pinMode(EN2,OUTPUT);
  pinMode(EN3,OUTPUT);
  pinMode(EN4,OUTPUT);
  pinMode(EN5, OUTPUT);
  pinMode(EN6, OUTPUT);
  pinMode(velocidadeDireita,OUTPUT);
  pinMode(velocidadeEsquerda,OUTPUT);
  pinMode(velocidadeCentral,OUTPUT);
   
  digitalWrite(velocidadeDireita,HIGH);
  digitalWrite(velocidadeEsquerda,HIGH);
  digitalWrite(velocidadeCentral,HIGH);
}
 
void loop(){
  char direcao;
 if (Serial.available()){
  direcao = (char)Serial.read();
 }
 direction_input(direcao);
 
}

void direction_input(char direct){
  Serial.println(direct);
  switch(direct){
    case 'w':
      //Movimentar para frente
      digitalWrite(EN1,LOW);
      digitalWrite(EN2,HIGH);

      digitalWrite(EN3,HIGH);
      digitalWrite(EN4,LOW);

      digitalWrite(EN5,LOW);
      digitalWrite(EN6,LOW);

      break;

    case 's':
      //Movimentar para trás
      digitalWrite(EN1,HIGH);
      digitalWrite(EN2,LOW);

      digitalWrite(EN3,LOW);
      digitalWrite(EN4,HIGH);
     
      digitalWrite(EN5,LOW);
      digitalWrite(EN6,LOW);
     
      break;

    case 'a':
       //Girar para esquerda
       digitalWrite(EN1,HIGH);
       digitalWrite(EN2,LOW);

       digitalWrite(EN3,LOW);
       digitalWrite(EN4,LOW);

       digitalWrite(EN5,LOW);
       digitalWrite(EN6,LOW);

       break;

    case 'd':
      //Girar para a direita
      digitalWrite(EN1,LOW);
      digitalWrite(EN2,LOW);

      digitalWrite(EN3,HIGH);
      digitalWrite(EN4,LOW);

      digitalWrite(EN5,LOW);
      digitalWrite(EN6,LOW);

      break;

   case '1':
      //Movimentar para cima
      digitalWrite(EN1,LOW);
      digitalWrite(EN2,LOW);

      digitalWrite(EN3,LOW);
      digitalWrite(EN4,LOW);

      digitalWrite(EN5,HIGH);
      digitalWrite(EN6,LOW);

      break;

   case '2':
      //Movimentar para baixo
      digitalWrite(EN1,LOW);
      digitalWrite(EN2,LOW);

      digitalWrite(EN3,LOW);
      digitalWrite(EN4,LOW);

      digitalWrite(EN5,LOW);
      digitalWrite(EN6,HIGH);

      break;

   case '3':
    //Parar todos os motores
    digitalWrite(EN1,LOW);
    digitalWrite(EN2,LOW);

    digitalWrite(EN3,LOW);
    digitalWrite(EN4,LOW);

    digitalWrite(EN5,LOW);
    digitalWrite(EN6,LOW);

    break;

  }
}
