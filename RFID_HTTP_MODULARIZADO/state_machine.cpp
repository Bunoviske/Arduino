#include "state_machine.h"



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
  char tokenAux[TAM_TOKEN + 1];
  int addr;
  int i;
  for ( addr = 0; addr < TAM_TOKEN + 1; addr++) {
    tokenAux[addr] = EEPROM.read(addr);
  }

  if (tokenAux[TAM_TOKEN] != (char)'>') {
    FSM_SBRT = StateAuthentication;
    return;
  }
  else {
    id_token = "";
    for (i = 0; i < TAM_TOKEN; i++) {
      id_token += (String)tokenAux[i];
    }
    Serial.println(id_token);

    // ACTION ID pode variar o tamanho, então será feita uma leitura dinâmica
    char actionIdAux[TAM_ACTION_ID_MAX];
    addr = TAM_TOKEN + 1;
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
      char itemIdAux[TAM_ITEM_ID_MAX];
      addr++;
      i = 0;
      while((EEPROM.read(addr) != (char)'>')&&(i < TAM_ITEM_ID_MAX + 1)) {
        actionIdAux[i] = EEPROM.read(addr);
        addr++;
        i++;
      }

      if (i == TAM_ITEM_ID_MAX + 1 ) {
        FSM_SBRT = StateService;
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
    if (autenticacao.connect(server, 80)) {
      Serial.println("Client Autenticacao connected");

      if (flag_global == 0){
        autent = autentURL();
      }
      else{
        autent = expiredTokenURL();
      }
      delay(5000);
      autenticacao.println(metodo + autent +" HTTP/1.1");
      String aux = (String)server;
      autenticacao.println("Host: " + aux);
      autenticacao.println("Connection: close");
      autenticacao.println();
      delay(1000);
      }
    else{
      Serial.println("Failed to conect, trying again in 5 seconds...");
      delay(5000);
      }
      Serial.println("oi");
    }while(!autenticacao.connected());


  do {
      jsonString = autenticacao.readString();
      jsonString = check_json(jsonString);
      Serial.println(jsonString);
      id_token = authentication_parsing(jsonString);
      Serial.println(id_token);
      if(id_token.toInt() == 403){
        Serial.println("Authentication error, trying again in 5 seconds");
        delay(5000);
        FSM_SBRT = StateAuthentication;
        autenticacao.stop();
        return;
      }
      else {
        char aux[TAM_TOKEN];
        id_token.toCharArray(aux,TAM_TOKEN);
        int addr;
        for (addr = 0; addr < TAM_TOKEN; addr++){
          EEPROM.write(addr, aux[addr]);
          Serial.println(EEPROM.read(addr));

        }
        EEPROM.write(addr, (char)'>');
        Serial.println(EEPROM.read(addr));
      }
  } while( (autenticacao.available()) );

  if (flag_global == 0) {
    FSM_SBRT = StateService;
  }
  else{
    FSM_SBRT = FSM_SBRT_anterior;
  }
  autenticacao.stop();

}

void StateService()
{

  do{
    if (servico.connect(server, 80) ) {
        Serial.println("Client Servicos connected");
        servicos = serviceURL();
        servico.println("POST " + servicos +" HTTP/1.1");
        String aux = (String)server;
        servico.println("Host: " + aux); 
        servico.println("Connection: close");
        servico.println();
        delay(1000);
    }
    else{
        Serial.println("Failed to conect, trying again in 5 seconds...");
        delay(5000);
        }
  }while(!servico.connected());

     do{
        jsonString = servico.readString();
        jsonString = check_json(jsonString);
        Serial.println(jsonString);
        String retorno = service_parsing(jsonString);
        Serial.println(retorno);
        if(retorno.toInt() == 403){
          Serial.println("Erro cadastro de servicos, testando em 5 segundos...");
          delay(5000);
          FSM_SBRT = StateService;
          servico.stop();
          return;
        }
        else if ( retorno.toInt() == 500 ){
          Serial.println("Token expirado");
          delay(5000);
          FSM_SBRT_anterior = StateService;
          FSM_SBRT = StateAuthentication;
          metodo = "PUT ";
          flag_global = 1;
          servico.stop();
          return;
        }
        else {
          //se forem dois servicos, mudar aqui. como o tamanho do token é sempre o mesmo, sabe-se onde vai comecar a gravar na eeprom.

          /**************************************************************
           * Escrita do action id na eeprom
           ************************************************************/
          int tam = action_id.length();
          int k = 0;
          char aux[tam];
          int addr;
          int inicio = TAM_TOKEN + 1;
          
          for (k=0;k < tam; k++){
            aux[k] = (char)action_id[k];
          }
          

          k = 0;
          for (addr = inicio; addr < tam + inicio; addr++){
            EEPROM.write(addr, aux[k]);
            Serial.println(EEPROM.read(addr));
            k++;
          }
          EEPROM.write(addr, (char)'>');

          /**************************************************************
           * Escrita do item id na eeprom
           ************************************************************/

          inicio = addr + 1;
          tam = item_id.length();
          
          k=0;
          for (k=0;k<tam;k++){
            aux[k] =(char) item_id[k];
          }
          
          k = 0;      
          for (addr = inicio; addr < tam + inicio; addr++){
            EEPROM.write(addr, aux[k]);
            Serial.println(EEPROM.read(addr));
            k++;
          }
          EEPROM.write(addr, '>');
          }
    }while (servico.available());
    FSM_SBRT = StateUpdate;
    servico.stop();

}

void StateUpdate()
{
  unsigned int value = readSensor();
  current_time = millis();
  if ((value == 0) && ((current_time - previous_time) >= refresh_rate)){
    //se value = -1, quer dizer que nao tem cartao de acesso para fazer o get no servidor.
    //assim, é feito um put para ficar verificando se o token nao expirou.
      do{
        if (update.connect(server, 80)){
           Serial.println("Client Update connected");
           atualiza = updateURL(value, PUT);
           update.println("PUT " + atualiza + " HTTP/1.1");
           String aux = (String)server;
           autenticacao.println("Host: " + aux);
           update.println("Connection: close");
           update.println();
           delay(500);
         }
         else{
           Serial.println("Failed to connect, trying again in 5 seconds...");
           delay(5000);
         }
      }while (!update.connected());
      
      while (update.available()) {
           jsonString = update.readString();
           jsonString = check_json(jsonString);
           Serial.println(jsonString);
           String response = update_parsing(jsonString, 0);
           Serial.println(response);
           if (response.toInt() == 500){
             FSM_SBRT = StateAuthentication;
             FSM_SBRT_anterior = StateUpdate;
             metodo = "PUT "; 
             flag_global = 1;
             update.stop();
             return;
           }
           else if (response.toInt() == 403){
              Serial.println("Erro update, testando em 5 segundos...");
              delay(5000);
              FSM_SBRT = StateUpdate;
              update.stop();
              return;        
           }
      }
      previous_time = current_time;
  } 
  else if (value != 0){
    //parte que verifica se o cartao de acesso e valido
      do{
        if (update.connect(server, 80)){
           Serial.println("Client Update connected");
           atualiza = updateURL(value, GET);
           update.println("GET " + atualiza + " HTTP/1.1");
           String aux = (String)server;
           autenticacao.println("Host: " + aux);
           update.println("Connection: close");
           update.println();
           delay(500);
         }
         else{
           Serial.println("Failed to connect, trying again in 5 seconds...");
           delay(5000);
         }
      }while (!update.connected());
      
      while (update.available()) {
           jsonString = update.readString();
           jsonString = check_json(jsonString);
           Serial.println(jsonString);
           String response = update_parsing(jsonString, 2);
           Serial.println(response);
           if (response.toInt() == 500){
             FSM_SBRT = StateAuthentication;
             FSM_SBRT_anterior = StateUpdate;
             metodo = "PUT "; 
             flag_global = 1;
             update.stop();
             return;
           }
           else if (response.toInt() == 404){
              //fecha porta
           }
           else{
             //abre porta
           }
      }        
  }
  
  FSM_SBRT = StateUpdate;
  update.stop();

}
