
//*****************************************************************************************************
//HEADER's

#include <EEPROM.h>
#include "settings.h"
#include "dados_dispositivo.h"

//GLOBAIS

String mac_aux = "0xDE,0xAD,0xBE,0xEF,0xFE,0xED";

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

void initSerial(int baudrate);
void initGPIO();
unsigned int readSensor();

//*****************************************************************************************************
//ESP866 CONNECTION GLOBAL VARIABLES

//declarar se for cliente
Client_Connection * esp_client = new Client_Connection(SSID, PSWD, 1, 1, protocolo, serverip, porta);

//declarar se for servidor
//Server_Connection * esp_client = new Server_Connection(SSID, PSWD, 1, 1, protocolo, porta);

int connectionId;



//*****************************************************************************************************
//Setup
void setup () {
  initSerial(115200);
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
  //principal função da classe de conexão. Responsável por conectar o dispositivo na rede e configurá-lo
  esp_client->get_connection();
  esp_client->change_baud(19200, 3000);


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

 esp_client->create_connection(2000);
 connectionId = esp_client->get_connectionId();
 Serial.println("Client Autenticacao connected");
 id_token = send_data("get_token", 2000, connectionId);
 
 delay(500);



 //escrita na eeprom
  Serial.println(id_token);
 
  char aux[id_token.length()];
  id_token.toCharArray(aux,id_token.length()+1);
  int addr;
  for (addr = 0; addr < id_token.length(); addr++){
    EEPROM.write(addr, aux[addr]);
  }
  EEPROM.write(addr, (char)'>');
  pos_final_token = addr; 
     

  if (flag_global == 0) {
    FSM_SBRT = StateService;
  }
  else{
    FSM_SBRT = FSM_SBRT_anterior;
  }
  
  esp_client->close_connection(2000);
  delay(2000);
}

void StateService()
{

     esp_client->create_connection(2000);
     connectionId = esp_client->get_connectionId();
     Serial.println("Client Servico connected");
     action_id = send_data("get_action_id", 2000, connectionId);
     
     delay(500);

    //escrita na eeprom
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
        
    FSM_SBRT = StateArguments;
    esp_client->close_connection(2000);
    delay(2000);
}

void StateArguments()
{
     esp_client->create_connection(2000);
     connectionId = esp_client->get_connectionId();
     Serial.println("Client Cadastro connected");
     item_id = send_data("get_item_id", 2000, connectionId);
    
     delay(500);

     //escrita na eeprom
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
  
  esp_client->close_connection(2000);
  FSM_SBRT = StateUpdate;
  delay(2000);
}


void StateUpdate()
{
     unsigned int value = readSensor();

     esp_client->create_connection(2000);
     connectionId = esp_client->get_connectionId();
     
     Serial.println("Client Update connected");

     String atualizacao = mac_aux;
     atualizacao += ';';
     atualizacao += esp_client->get_ip();
     atualizacao += ';'; 
     atualizacao += host_name;
     atualizacao += ';';
     atualizacao += nome_service_servico1;
     atualizacao += ';';
     atualizacao += value;
     atualizacao += ';';
     atualizacao += refresh_rate;
     
     Serial.println(atualizacao);


     String resposta = send_data(atualizacao, 2000, connectionId);
     Serial.println(resposta);
 
    
     FSM_SBRT = StateUpdate;
     esp_client->close_connection(2000);
     delay(refresh_rate);
}

//*****************************************************************************************************
//METODOS PRIVADOS


unsigned int readSensor()
{
  return ((float)analogRead(A0)/1024)*100;
} 

//INICIALIZA A COMUNICAÇão SERIAL 
void initSerial(int baudrate)
{
  Serial.begin(115200);
}

//INICIALIZA pinos GPIO
void initGPIO()
{

}




