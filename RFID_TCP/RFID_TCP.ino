

//*****************************************************************************************************
//HEADER's
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

//*****************************************************************************************************
//DEFINE's


//autenticacao
#define host_name               "Arduino_RFID"

//servico 1
#define nome_service_servico1   "rfid" //nome servico 1
#define refresh_rate            10000

#define TAM_TOKEN_MAX           5
#define TAM_ACTION_ID_MAX       5 //se fo um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         5 //mesma coisa com o item id



//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux = "0xDE,0xAD,0xBE,0xEF,0xFE,0xED";

char *server = "172.16.9.84";
int porta = 5500;

EthernetClient autenticacao; //client responsavel pela autenticacao
EthernetClient servico; //client responsavel pelo cadastro de servicos
EthernetClient cadastro; //client responsavel pelo cadastro em arguments
EthernetClient update; //client responsavel pela atualizacao em arguments

String id_token = "";
String action_id = ""; // para mais de uma servico String action_id[n], com n == numero de servicos
String item_id = ""; //   mesma coisa para o item_id
String atualiza = "";
String autent = "";
String servicos = "";

int pos_final_token = 0;

int flag_global = 0;
String metodo = "POST ";

//ponteiro de função da máquina de estados. Ele aponta sempre para a função da máquina de estados que deve ser executada
void (*FSM_SBRT)();
void (*FSM_SBRT_anterior)();

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
  do{
      if(Ethernet.begin(mac) != 0){
        Serial.println("Ethernet conectado");
        delay(1000);
      }
      else{
        Serial.println("Failed to configure Ethernet using DHCP");
      // somente pra ip static Ethernet.begin(mac, ip);
      }
      delay(5000);
  }while(Ethernet.begin(mac) == 0);

  //TROCA DE ESTADO
  FSM_SBRT = StateReadEprom;
}

void StateReadEprom()
{
  // TOKEN smp vai ser do mesmo tamanho
  char tokenAux[TAM_TOKEN_MAX + 1];
  int addr=0;
  int i;
  while ((EEPROM.read(addr) != (char)'>')&&(addr < TAM_TOKEN_MAX + 1)) {
    tokenAux[addr] = EEPROM.read(addr);
    addr++;
  }

  if (addr == TAM_TOKEN_MAX + 1) {
    FSM_SBRT = StateAuthentication;
    return;
  }
  else {
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
      FSM_SBRT = StateService;
      return;
    } 
    else {
      //se tiverem dois servicos, mudar essa parte para que ele reconheça o caracter que divide os dois action-id na memoria
      action_id = "";
      for (int k = 0; k < i; k++) {
        action_id += (String)actionIdAux[k];
      }
      Serial.println(action_id);

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
        FSM_SBRT = StateArguments;
        return;
      }
      else {
        item_id = "";
        for (int k = 0; k < i; k++) {
          item_id += (String)itemIdAux[k];
       }
       Serial.println(item_id);

       FSM_SBRT = StateUpdate;
      }
    }
  }
}


void StateAuthentication()
{
//se flag = 0, post de autenticacao. se flag = 1, put de autenticacao(expiredToken)
  do{
    if (autenticacao.connect(server, porta)) {
      Serial.println("Client Autenticacao connected");
      autenticacao.print("get_token");
      delay(1000);
      }
    else{
      Serial.println("Failed to conect, trying again in 5 seconds...");
      delay(5000);
      }
    }while(!autenticacao.connected());


  do {
      id_token = autenticacao.readString();
      Serial.println(id_token);
     
      char aux[id_token.length()];
      id_token.toCharArray(aux,id_token.length()+1);
      int addr;
      for (addr = 0; addr < id_token.length(); addr++){
        EEPROM.write(addr, aux[addr]);
      }
      EEPROM.write(addr, (char)'>');
      pos_final_token = addr; 
      
  } while((autenticacao.available()));

  if (flag_global == 0) {
    FSM_SBRT = StateService;
  }
  else{
    FSM_SBRT = FSM_SBRT_anterior;
  }
  autenticacao.stop();
  delay(refresh_rate);
}

void StateService()
{

  do{
    if (servico.connect(server, porta) ) {
        Serial.println("Client Servicos connected");
        servico.print("get_action_id");
        delay(1000);
    }
    else{
        Serial.println("Failed to conect, trying again in 5 seconds...");
        delay(5000);
        }
  }while(!servico.connected());

     do{
        action_id = servico.readString();
        Serial.println(action_id);
        //implementar token expirado
        
        
        //se forem dois servicos, mudar aqui. como o tamanho do token é sempre o mesmo, sabe-se onde vai comecar a gravar na eeprom.
        int tam = action_id.length();
        int k = 0;
        char aux[tam];
        for (k=0;k< tam; k++){
          aux[k] = (char)action_id[k];
        }
        
        int addr;
        int inicio = pos_final_token + 1;
        k = 0;
        for (addr = inicio; addr < tam + inicio; addr++){
          EEPROM.write(addr, aux[k]);
          k++;
        }
        EEPROM.write(addr, (char)'>');
        
    }while (servico.available());
    FSM_SBRT = StateArguments;
    servico.stop();
    delay(refresh_rate);
}

void StateArguments()
{
    do{
        if (cadastro.connect(server, porta)) {
          Serial.println("Cadastro Connected");
          cadastro.print("get_item_id");
          delay(2000);
        }
        else{
          Serial.println("Failed to conect, trying again in 5 seconds...");
          delay(5000);
        }
      }while(!cadastro.connected());

   do {
    item_id = cadastro.readString();
    Serial.println(item_id);
    //implementar token expirado
    
      // se forem dois servicos, mudar aqui. nao se sabe o tamanho do action-id, entao é necessario percorrer a eeprom para saber onde escrever o item id
      
     int inicio = pos_final_token + 1;
      int addr;
      
      //"for" que percorre a eeprom 
      for(addr = inicio; addr < TAM_ACTION_ID_MAX + inicio + 1; addr++){
        if (EEPROM.read(addr) == '>'){
          Serial.print("achou action id");
          break;
        }
      }
      
      int tam = item_id.length();
      char aux[tam];
      int k=0;
      for (k=0;k<tam;k++){
        aux[k] =(char) item_id[k];
      }
      inicio = addr + 1;
      k = 0;      
      for (addr = addr + 1; addr < tam + inicio; addr++){
        EEPROM.write(addr, aux[k]);
        k++;
      }
      EEPROM.write(addr, '>');
  
  }while (cadastro.available());
  cadastro.stop();
  FSM_SBRT = StateUpdate;
  delay(refresh_rate);
}


void StateUpdate()
{
  unsigned int value = readSensor();

  
      do{
        if (update.connect(server, porta)){
           String atualizacao = mac_aux;
           atualizacao += ';';
           atualizacao += Ethernet.localIP();
           atualizacao += ';'; 
           atualizacao += host_name;
           atualizacao += ';';
           atualizacao += nome_service_servico1;
           atualizacao += ';';
           atualizacao += value;
           atualizacao += ';';
           atualizacao += refresh_rate;
           Serial.println(atualizacao);
           update.print(atualizacao);
           delay(2000);
         }
         else{
           Serial.println("Failed to connect, trying again in 5 seconds...");
           delay(5000);
         }
      }while (!update.connected());
      
      while (update.available()) {
          String resposta = update.readString();
          Serial.println(resposta);
      }
  
  
  FSM_SBRT = StateUpdate;
  update.stop();
  delay(refresh_rate);
}

//*****************************************************************************************************
//METODOS PRIVADOS


unsigned int readSensor()
{
  String receivedTagCode;
  unsigned int tmp;
  
  //  if (Serial1.available()) {
  //      receivedTagCode = Serial1.readString(); //Le String do cartao
  //      String str = receivedTagCode.substring(5,11); //Quebea String, pega apenas dados do carto ignorando CheckSum
  //      tmp = hexToDec(str); //Converte dados
  //      Serial.println(tmp);
  //      return tmp;
  //  }
  return 0;

}



unsigned int hexToDec(String hexString) {
  
  unsigned int decValue = 0;
  int nextInt;
  
  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    
    decValue = (decValue * 16) + nextInt;
  }
  return decValue;
}

//INICIALIZA A COMUNICAÇão SERIAL 
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
  //Serial1.begin(baudrate);
}

//INICIALIZA pinos GPIO
void initGPIO()
{

}




