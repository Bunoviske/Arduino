//*****************************************************************************************************
//HEADER's

#include <EEPROM.h>
#include <SoftwareSerial.h>


//*****************************************************************************************************
//DEFINE's


//autenticacao
#define host_name               "Arduino_Monitoramento_Energia"

#define refresh_rate            10000

#define NUM_SERVICOS            1 //num servicos define tanto o num de action id quanto num de item id
#define TAM_TOKEN_MAX           15
#define TAM_ACTION_ID_MAX       15 //se for um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         15 //mesma coisa com o item id


//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux = "0xDE,0xAD,0xBE,0xEF,0xFE,0xED";

char *server = "172.16.9.84";
int porta = 5500;

String id_token = "";
String atualiza = "";
String autent = "";
String servicos = "";

String action_id[NUM_SERVICOS]; // para mais de uma servico String action_id[n], com n == numero de servicos
String item_id[NUM_SERVICOS]; //   mesma coisa para o item_id
String nome_servico[NUM_SERVICOS];

int pos_final_token = 0;

int flag_global = 0;
String metodo = "POST ";

//ponteiro de função da máquina de estados. Ele aponta sempre para a função da máquina de estados que deve ser executada
void (*FSM_SBRT)();
void (*FSM_SBRT_anterior)();

SoftwareSerial esp(2,3);
String IpAddress = "";

 

//*****************************************************************************************************
//Prototipos

//FSM
void StateInit(void);       //função que representa o estado inicial da máquina de estados
void StateReadEprom(void);
void StateAuthentication(void);
void StateService(void);
void StateArguments(void);
void StateUpdate(void);


//Metodos privados

void initSerial(unsigned int baudrate);
void initGPIO();
unsigned int readSensor();

//*****************************************************************************************************
//Setup
void setup () {
  initSerial(9600);
  //initGPIO();
  delay(15000);
  nome_servico[0] = "regua_energia";


  //INICIALIZA FSM
  FSM_SBRT = StateInit;

}


//*****************************************************************************************************
//LOOP
void loop(){
  (*FSM_SBRT)();
}


//*****************************************************************************************************
//Implementação FSM
void StateInit()
{

  //estado que pede ao esp o ipaddress dele
  while(IpAddress == ""){
    Serial.println("Pegando ip do esp");
    esp.print("get_ip");
    delay(5000);
    while(esp.available()){
      IpAddress = esp.readString();
    }
  }
  
  //TROCA DE ESTADO
  FSM_SBRT = StateReadEprom;
}

void StateReadEprom()
{
  // na eeprom eh gravado da seguinte maneira: token>action_id[0]/action_id[1]>item_id[0]/item_id[1]>

  char tokenAux[TAM_TOKEN_MAX + 1];
  int addr=0;
  int i;
  while ((EEPROM.read(addr) != (char)'>')&&(addr < TAM_TOKEN_MAX + 1)) {
    tokenAux[addr] = EEPROM.read(addr);
    addr++;
  }

  if (addr == TAM_TOKEN_MAX + 1) {
    //nao achou token
    FSM_SBRT = StateAuthentication;
    return;
  }
  else {
    //achou token
    id_token = "";
    for (i = 0; i < addr; i++) {
      id_token += (String)tokenAux[i];
    }
    Serial.println(id_token);

    pos_final_token = addr;

    // ACTION ID pode variar o tamanho, então será feita uma leitura dinâmica
    char actionIdAux[TAM_ACTION_ID_MAX + 1];
    addr = pos_final_token + 1;
    i=0;
    while((EEPROM.read(addr) != (char)'>')&&(i< TAM_ACTION_ID_MAX + 1)) { //é somado um para considerar o caracter q define o fim do action-id
      actionIdAux[i] = EEPROM.read(addr);
      addr++;
      i++;
    }

    if (i == TAM_ACTION_ID_MAX + 1) {
      //nao achou-se action id na eeprom
      FSM_SBRT = StateService;
      return;
    }
    else {
      // achou-se action id na eeprom
      actionIdAux[i] = '>';
      char comparador;
      int k=0;

      for(int j = 0; j < NUM_SERVICOS; j++){
        action_id[j] = "";

        //dependendo de qual action id esta sendo lido muda-se o caracter de comparacao.
        if(j + 1 == NUM_SERVICOS){
          //se for o ultimo action id
          comparador = '>';
        }
        else{
          //se for action id do meio
          comparador = '/';
        }

        while (actionIdAux[k] != comparador) {
          action_id[j] += (String)actionIdAux[k];
          k++;
        }
        k++;
        Serial.println(action_id[j]);
      }


      // ITEM ID pode variar o tamanho, então será feita uma leitura dinâmica
      char itemIdAux[TAM_ITEM_ID_MAX + 1];
      addr++;
      i = 0;
      while((EEPROM.read(addr) != (char)'>')&&(i < TAM_ITEM_ID_MAX + 1)) {
        itemIdAux[i] = EEPROM.read(addr);
        addr++;
        i++;
      }

      if (i == TAM_ITEM_ID_MAX + 1 ) {
       //nao achou-se item id na eeprom

        FSM_SBRT = StateArguments;
        return;
      }
      else {
        //achou-se item id na eeprom

        itemIdAux[i] = '>';
        k=0;
        for(int j = 0; j < NUM_SERVICOS; j++){
          item_id[j] = "";

          if(j + 1 == NUM_SERVICOS){
            comparador = '>';
          }
          else{
            comparador = '/';
          }

          while (itemIdAux[k] != comparador) {
            item_id[j] += (String)itemIdAux[k];
            k++;
          }
          k++;
          Serial.println(item_id[j]);
        }
        FSM_SBRT = StateUpdate;
      }
    }
  }
}

void StateAuthentication()
{
//se flag = 0, post de autenticacao. se flag = 1, put de autenticacao(expiredToken)
  id_token = "";

       //se houver erro de comunicacao e nao for recebido o token_id, deve ficar travado nesse estado até recebê-lo

//  while (id_token == "" ){
    Serial.println("Client Autenticacao connected");
    esp.print("get_token");
    delay(5000);
      
  
  
    while(esp.available()){
        id_token = esp.readString();
        Serial.print("Token: ");
        Serial.println(id_token);
    } 
 // }
  write_token();
  if (flag_global == 0) {
    FSM_SBRT = StateService;
  }
  else{
    FSM_SBRT = FSM_SBRT_anterior;
  }
  delay(2000);
}

void StateService()
{
  for (int i = 0; i< NUM_SERVICOS; i++) {
      action_id[i] = "";

           //se houver erro de comunicacao e nao for recebido o action_id, deve ficar travado nesse estado até recebê-lo

     // while (action_id[i] == ""){
        Serial.println("Client Servicos connected");
        esp.print("get_action_id");
        delay(5000);
  
  
        while (esp.available()){
            action_id[i] = esp.readString();
            Serial.print("action id: ");
            Serial.println(action_id[i]);
        }
        delay(1000);
     // }
  }
   
  write_action();
  FSM_SBRT = StateArguments;
  delay(2000);

}

void StateArguments()
{
  for (int j = 0; j< NUM_SERVICOS; j++) {
     item_id[j] = "";

     //se houver erro de comunicacao e nao for recebido o item_id, deve ficar travado nesse estado até recebê-lo
   //  while (item_id[j] == ""){
       
       Serial.println("Cadastro Connected");
       esp.print("get_item_id");
       delay(5000);
  
       while (esp.available()) {
          item_id[j] = esp.readString();
          Serial.print("item id:");
          Serial.println(item_id[j]);
       }
 //    }
  }
  
  write_item();
  FSM_SBRT = StateUpdate;
  delay(refresh_rate);
}


void StateUpdate()
{
  int data = 0;
  int value[NUM_SERVICOS];
  for (data = 0; data < NUM_SERVICOS; data++) {
       value[data] = readSensor(data);
       String atualizacao = mac_aux;
       atualizacao += ';';
       atualizacao += IpAddress;
       atualizacao += ';';
       atualizacao += host_name;
       atualizacao += ';';
       atualizacao += nome_servico[data];
       atualizacao += ';';
       atualizacao += (String)value[data];
       atualizacao += ';';
       atualizacao += refresh_rate/1000;
       Serial.println(atualizacao);
       esp.print(atualizacao);
       delay(5000);

       while (esp.available()) {
         String resposta = esp.readString();
         Serial.println(resposta);
       }
       
  }

  FSM_SBRT = StateUpdate;
  delay(refresh_rate);
}

//funcoes de escrita

void write_token(){

    char aux[id_token.length()];
    id_token.toCharArray(aux,id_token.length()+1);
    int addr;
    for (addr = 0; addr < id_token.length(); addr++){
      EEPROM.write(addr, aux[addr]);
    }
    EEPROM.write(addr, (char)'>');
    pos_final_token = addr;
}

void write_action(){

  int inicio = pos_final_token + 1;
  for(int j = 0; j < NUM_SERVICOS; j++){

    int tam = action_id[j].length();
    int k = 0;
    char aux[tam];
    for (k=0;k< tam; k++){
      aux[k] = (char)action_id[j][k];
    }

    int addr;
    k = 0;

    for (addr = inicio; addr < tam + inicio; addr++){
      EEPROM.write(addr, aux[k]);
      k++;
    }

    if (j+1 == NUM_SERVICOS){
      EEPROM.write(addr, (char)'>');
    }
    else{
      EEPROM.write(addr, (char)'/');
    }

    inicio = addr + 1;
  }

}


void write_item(){

  int inicio = pos_final_token + 1;
  int addr;

 
  //"for" que percorre a eeprom
  for(addr = inicio; addr < TAM_ACTION_ID_MAX + inicio + 1; addr++){
    if (EEPROM.read(addr) == '>'){
      Serial.print("achou action id");
      break;
    }
  }
  inicio = addr + 1;
  for(int j =0; j < NUM_SERVICOS; j++){

      int tam = item_id[j].length();
      char aux[tam];
      int k=0;
      for (k=0;k<tam;k++){
        aux[k] =(char) item_id[j][k];
      }
      k = 0;
      for (addr = inicio; addr < tam + inicio; addr++){
        EEPROM.write(addr, aux[k]);
        k++;
      }

      if (j+1 == NUM_SERVICOS){
        EEPROM.write(addr, (char)'>');
      }
      else{
        EEPROM.write(addr, (char)'/');
      }

      inicio = addr + 1;
  }

}

//*****************************************************************************************************
//METODOS PRIVADOS

float readSensor(int data)
{
    int pinoSensor = A0;
   
    int sensorValue_aux = 0;
    float valorSensor = 0;
    float valorCorrente = 0;
    float voltsporUnidade = 0.004887586;// (Maximo valor de tensao do arduino / resolucao de bits do conversor AD) 5%1023
    // Para ACS712 de  5 Amperes usar 0.185
    // Para ACS712 de 10 Amperes usar 0.100
    //  Para ACS712 de 30 Amperes usar 0.066
    float sensibilidade = 0.066;
   
  //Tensao da rede AC 110 Volts e na verdade (127 volts)
   int tensao = 220;
   String receivedTagCode;
   unsigned int corrente;

   for(int i=10000; i>0; i--){
    // le o sensor no pino analogico A0 e ajusta o valor lido, ja que a saída do sensor é (1023)vcc/2 para corrente = 0
    sensorValue_aux = (analogRead(pinoSensor) -510);
    // somam os quadrados das leituras.
    valorSensor += pow(sensorValue_aux,2);
    delay(1);                                           
   }
   
    // finaliza o calculo da média quadratica e ajusta o valor lido para volts
    valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;
    // calcula a corrente considerando a sensibilidade do sernsor (66 mV por amper)
    valorCorrente = (valorSensor/sensibilidade);
   
    //tratamento para possivel ruido
    //O ACS712 para 30 Amperes é projetado para fazer leitura
    // de valores alto acima de 0.25 Amperes até 30.
    // por isso é normal ocorrer ruidos de até 0.20A
    //por isso deve ser tratado
    
    if(valorCorrente <= 0.095){
      valorCorrente = 0;
    }
   
    valorSensor = 0;
 
    delay(100);

  
    Serial.println(valorCorrente * tensao);
    return valorCorrente * tensao;

}



//INICIALIZA A COMUNICAÇão SERIAL
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
  esp.begin(baudrate);

}

//INICIALIZA pinos GPIO
void initGPIO()
{
}

