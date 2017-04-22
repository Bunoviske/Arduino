#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


//*****************************************************************************************************
//HEADER's
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

//*****************************************************************************************************
//DEFINE's

#define PUT                     0
#define POST                    1
#define GET                     2

#define ACTION_ID_RFID          "4"
#define rele                     6

//Define Cartoes
#define Samuel       "29785"
#define Tom          "44608"
#define Felipe       "38709"
#define Bruno        "3538745235"

//autenticacao
#define host_name               "Arduino_RFID"
#define description_autent      "Arduino_Modulo_Rfid"
#define id_chipset              123
#define id_processor            123
#define hd_serial               123

//servico 1
#define nome_service_servico1   "rfid" //nome servico 1
#define inf_type                "string" //tipo variável
#define scpdurl                 "172,16,9,123"
#define description_service     "Autenticacao" //descricao
#define control_url             "172,16,9,123"
#define event_sud_url           "172,16,9,123"
#define refresh_rate            30000



//update servico
#define default_value_servico1  "nomePessoa"
#define nome_update_servico1    "rfid"


#define TAM_TOKEN_MAX           45
#define TAM_ACTION_ID_MAX       5 //se fo um dispositivo que tem mais de um action id, dobrar esse valor
#define TAM_ITEM_ID_MAX         5 //mesma coisa com o item id

/*flags para mais de 1 servico
  #define flag_servico1           0
  #define flag_servico2           1
*/

// pin 8 - Serial clock out (SCLK)
// pin 9 - Serial data out (DIN)
// pin 10 - Data/Command select (D/C)
// pin 11 - LCD chip select (CS/CE)
// pin 12 - LCD reset (RST)

Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, 10, 11, 12);

//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux; 

char server[] = "raise.uiot.org"; //mudar para string e otimizar string literal ??
IPAddress ip(172, 16, 9, 123);

EthernetClient autenticacao; //client responsavel pela autenticacao
EthernetClient servico; //client responsavel pelo cadastro de servicos
EthernetClient cadastro; //client responsavel pelo cadastro em arguments
EthernetClient update; //client responsavel pela atualizacao em arguments

String jsonString = "";
String id_token = "";
String action_id = ""; // para mais de uma servico String action_id[n], com n == numero de servicos
String item_id = ""; //   mesma coisa para o item_id
String atualiza = "";
String autent = "";
String servicos = "";
int flag_global = 0;
String metodo;
int pos_final_token = 0;
unsigned long previous_time = 0;
unsigned long current_time = 0;
String nome;


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
String autentURL();
String expiredTokenURL(String token);
String serviceURL();
String updateURL(String return_value, int i);
String check_json(String aux);
String parseJson(String parametro, char tipo, String jsonstring);
String update_parsing(String jsonstring, int flag);
String service_parsing(String jsonstring);
String authentication_parsing(String jsonstring);

void initSerial();
void initGPIO();
String readSensor();

//*****************************************************************************************************
//Setup
void setup () {

  //inicializacao das variaveis globais

  mac_aux = F("0xDE,0xAD,0xBE,0xEF,0xFE,0xED");
  metodo = "POST ";

  initSerial(9600);
  initGPIO();
  display_inicial();


  //INICIALIZA FSM
  FSM_SBRT = StateInit;

}


//*****************************************************************************************************
//LOOP
void loop() {
  (*FSM_SBRT)();
}


//*****************************************************************************************************
//Implementação FSM
void StateInit()
{
  do {
    if (Ethernet.begin(mac) != 0) {
      Serial.println(F("Ethernet conectado"));
      delay(1000);
    }
    else {
      Serial.println(F("Failed to configure Ethernet using DHCP"));
      // somente pra ip static Ethernet.begin(mac, ip);
    }
    delay(5000);
  } while (Ethernet.begin(mac) == 0);

  //TROCA DE ESTADO
  FSM_SBRT = StateReadEprom;
}

void StateReadEprom()
{
  // TOKEN smp vai ser do mesmo tamanho
  char tokenAux[TAM_TOKEN_MAX + 1];
  int addr = 0;
  int i;
  while ((EEPROM.read(addr) != (char)'>') && (addr < TAM_TOKEN_MAX + 1)) {
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
    i = 0;
    while ((EEPROM.read(addr) != (char)'>') && (i < TAM_ACTION_ID_MAX + 1)) { //é somado um para considerar o caracter q define o fim do action-id
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
      while ((EEPROM.read(addr) != (char)'>') && (i < TAM_ITEM_ID_MAX + 1)) {
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
  do {
    if (autenticacao.connect(server, 80)) {
      Serial.println(F("Client Autenticacao connected"));

      if (flag_global == 0) {
        autent = autentURL();
      }
      else {
        autent = expiredTokenURL();
      }
      autenticacao.println(metodo + autent + " HTTP/1.1");
      String aux = (String)server;
      autenticacao.println("Host: " + aux);
      autenticacao.println(F("Connection: close"));
      autenticacao.println();
      delay(1000);
    }
    else {
      Serial.println(F("Failed to conect, trying again in 5 seconds..."));
      delay(5000);
    }
  } while (!autenticacao.connected());


  do {
    jsonString = autenticacao.readString();
    jsonString = check_json(jsonString);
    Serial.println(jsonString);
    id_token = authentication_parsing(jsonString);
    Serial.println(id_token);
    if (id_token.toInt() == 403) {
      Serial.println(F("Authentication error, trying again in 5 seconds"));
      delay(5000);
      FSM_SBRT = StateAuthentication;
      autenticacao.stop();
      return;
    }
    else {
      char aux[id_token.length()];
      id_token.toCharArray(aux, id_token.length() + 1);
      int addr;
      for (addr = 0; addr < id_token.length(); addr++) {
        EEPROM.write(addr, aux[addr]);
      }
      EEPROM.write(addr, (char)'>');
      pos_final_token = addr;
    }
  } while ( (autenticacao.available()) );

  if (flag_global == 0) {
    FSM_SBRT = StateService;
  }
  else {
    FSM_SBRT = FSM_SBRT_anterior;
  }
  autenticacao.stop();

}

void StateService()
{

  do {
    if (servico.connect(server, 80) ) {
      Serial.println(F("Client Servicos connected"));
      servicos = serviceURL();
      servico.println("POST " + servicos + " HTTP/1.1");
      String aux = (String)server;
      servico.println("Host: " + aux); // em caso de erro, mudar para cliente de autenticacao
      servico.println(F("Connection: close"));
      servico.println();
      delay(1000);
    }
    else {
      Serial.println(F("Failed to conect, trying again in 5 seconds..."));
      delay(5000);
    }
  } while (!servico.connected());

  do {
    jsonString = servico.readString();
    jsonString = check_json(jsonString);
    Serial.println(jsonString);
    action_id = service_parsing(jsonString);
    Serial.println(action_id);
    if (action_id.toInt() == 403) {
      Serial.println(F("Erro cadastro de servicos, testando em 5 segundos..."));
      delay(5000);
      FSM_SBRT = StateService;
      servico.stop();
      return;
    }
    else if ( action_id.toInt() == 500 ) {
      Serial.println(F("Token expirado"));
      delay(5000);
      FSM_SBRT_anterior = StateService;
      FSM_SBRT = StateAuthentication;
      metodo = F("PUT ");
      flag_global = 1;
      servico.stop();
      return;
    }
    else {
      //se forem dois servicos, mudar aqui. como o tamanho do token é sempre o mesmo, sabe-se onde vai comecar a gravar na eeprom.
      int tam = action_id.length();
      int k = 0;
      char aux[tam];
      for (k = 0; k < tam; k++) {
        aux[k] = (char)action_id[k];
      }

      int addr;
      int inicio = pos_final_token + 1;
      k = 0;
      for (addr = inicio; addr < tam + inicio; addr++) {
        EEPROM.write(addr, aux[k]);
        k++;
      }
      EEPROM.write(addr, (char)'>');
    }
  } while (servico.available());
  FSM_SBRT = StateArguments;
  servico.stop();

}

void StateArguments()
{
  do {
    if (cadastro.connect(server, 80)) {
      Serial.println(F("Client Cadastro connected"));
      atualiza = updateURL(F("stayAlive"), POST);
      cadastro.println("POST " + atualiza + " HTTP/1.1");
      String aux = (String)server;
      cadastro.println("Host: " + aux);
      cadastro.println(F("Connection: close"));
      cadastro.println();
      delay(2000);
    }
    else {
      Serial.println(F("Failed to conect, trying again in 5 seconds..."));
      delay(5000);
    }
  } while (!cadastro.connected());

  do {

    jsonString = cadastro.readString();
    jsonString = check_json(jsonString);
    Serial.println(jsonString);
    item_id = update_parsing(jsonString, 1);
    Serial.println(item_id);

    //se item id for igual a 403, dara problema!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (item_id.toInt() == 403) {
      Serial.println(F("Erro cadastro de arguments, testando em 5 segundos..."));
      delay(5000);
      FSM_SBRT = StateArguments;
      cadastro.stop();
      return;
    }
    else if ( item_id.toInt() == 500 ) {
      Serial.println(F("Token expirado"));
      delay(5000);
      FSM_SBRT_anterior = StateArguments;
      FSM_SBRT = StateAuthentication;
      metodo = F("PUT ");
      flag_global = 1;
      return;
    }
    else {
      // se forem dois servicos, mudar aqui. nao se sabe o tamanho do action-id, entao é necessario percorrer a eeprom para saber onde escrever o item id
      int inicio = pos_final_token + 1;
      int addr;

      //"for" que percorre a eeprom
      for (addr = inicio; addr < TAM_ACTION_ID_MAX + inicio + 1; addr++) {
        if (EEPROM.read(addr) == '>') {
          Serial.print(F("achou action id"));
          break;
        }
      }

      int tam = item_id.length();
      char aux[tam];
      int k = 0;
      for (k = 0; k < tam; k++) {
        aux[k] = (char) item_id[k];
      }
      inicio = addr + 1;
      k = 0;
      for (addr = addr + 1; addr < tam + inicio; addr++) {
        EEPROM.write(addr, aux[k]);
        k++;
      }
      EEPROM.write(addr, '>');
    }
  } while (cadastro.available());
  cadastro.stop();
  FSM_SBRT = StateUpdate;
}


void StateUpdate()
{
  String value = readSensor();
  current_time = millis();
  if ((value == "") && ((current_time - previous_time) >= refresh_rate)) {
    //se value = -1, quer dizer que nao tem cartao de acesso para fazer o get no servidor.
    //assim, é feito um put para ficar verificando se o token nao expirou.
    do {
      if (update.connect(server, 80)) {
        Serial.println(F("Client Update connected"));
        atualiza = updateURL(value, PUT);
        update.println("PUT " + atualiza + " HTTP/1.1");
        String aux = (String)server;
        update.println("Host: " + aux);
        update.println(F("Connection: close"));
        update.println();
        delay(500);
      }
      else {
        Serial.println(F("Failed to connect, trying again in 5 seconds..."));
        delay(5000);
      }
    } while (!update.connected());

    while (update.available()) {
      jsonString = update.readString();
      jsonString = check_json(jsonString);
      Serial.println(jsonString);
      String response = update_parsing(jsonString, 0);
      Serial.println(response);
      if (response.toInt() == 500) {
        FSM_SBRT = StateAuthentication;
        FSM_SBRT_anterior = StateUpdate;
        metodo = F("PUT ");
        flag_global = 1;
        update.stop();
        return;
      }
      else if (response.toInt() == 403) {
        Serial.println(F("Erro update, testando em 5 segundos..."));
        delay(5000);
        FSM_SBRT = StateUpdate;
        update.stop();
        return;
      }
    }
    previous_time = current_time;
  }

  else if (value != "") {
    //parte que verifica se o cartao de acesso e valido

    do {
      if (update.connect(server, 80)) {
        Serial.println(F("Client Update connected"));
        atualiza = updateURL(value, GET);
        Serial.println(atualiza);
        update.println("GET " + atualiza + " HTTP/1.1");
        String aux = (String)server;
        update.println("Host: " + aux);
        update.println(F("Connection: close"));
        update.println();
        delay(2000);
      }
      else {
        Serial.println(F("Failed to connect, trying again in 5 seconds..."));
        delay(5000);
      }

    } while (!update.connected());

    while (update.available()) {
      jsonString = update.readString();
      jsonString = check_json(jsonString);
      Serial.println(jsonString);
      String response = update_parsing(jsonString, 2);
      Serial.println(response);
      if (response.toInt() == 500) {
        FSM_SBRT = StateAuthentication;
        FSM_SBRT_anterior = StateUpdate;
        metodo = "PUT ";
        flag_global = 1;
        update.stop();
        return;
      }
      else if (response.toInt() == 404) {
        //fecha porta
      }
      else {
        //abre porta
        Serial.println(F("Bem vindo brunao!"));
        digitalWrite(rele, 0);

        display.clearDisplay();   //Apaga o buffer e o display
        display.setTextSize(2);  //Seta o tamanho do texto
        display.setTextColor(BLACK); //Seta a cor do texto
        display.setCursor(0, 0); //Seta a posição do cursor
        display.println(F("Bem vindo"));
        display.println(nome);
        display.display();

        delay(5000);
        display_inicial();
      }
    }
  }
  //  switch (value){
  //
  //    case Samuel:
  //      Serial.println("Bem vindo Samuel!");
  //      digitalWrite(rele,0);
  //      delay(5000);
  //      break;
  //
  //    case Bruno:
  //      Serial.println("Bem vindo Bruno!");
  //      digitalWrite(rele,0);
  //      delay(5000);
  //      break;
  //
  //    case Felipe:
  //      Serial.println("Bem vindo Felipe!");
  //      digitalWrite(rele,0);
  //      delay(5000);
  //      break;
  //
  //    case Tom:
  //      Serial.println("Bem vindo Tonzin1010!");
  //      digitalWrite(rele,0);
  //      delay(5000);
  //      break;
  //
  //      default: digitalWrite(rele,1);
  //  }
  if (value == Bruno) {
    digitalWrite(rele, 0);

    display.clearDisplay();   //Apaga o buffer e o display
    display.setTextSize(2);  //Seta o tamanho do texto
    display.setTextColor(BLACK); //Seta a cor do texto
    display.setCursor(0, 0); //Seta a posição do cursor
    display.println(F("Bem vindo"));
    display.println(nome);
    display.display();

    delay(5000);
    display_inicial();
  }
  digitalWrite(rele, 1);
  FSM_SBRT = StateUpdate;
  update.stop();

}


String readSensor()
{

  if (Serial1.available()) {
    String resposta = Serial1.readString();
    Serial.println(resposta);
    return resposta;

  }
  return "";

}

//*****************************************************************************************************


void botao(){

  digitalWrite(rele,0);
  double i = 400000; //aproximadamente 4,5 segs
  //Serial.println("Entrou interrupt");

  while (i!=0){
    i--;  
  }
  digitalWrite(rele,1);

  
  
}

void display_inicial() {

  display.clearDisplay();   //Apaga o buffer e o display
  display.setTextSize(2);  //Seta o tamanho do texto
  display.setTextColor(BLACK); //Seta a cor do texto
  display.setCursor(10, 20); //Seta a posição do cursor
  display.println(F("UIoT"));
  display.display();
}


//INICIALIZA A COMUNICAÇão SERIAL
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
  Serial1.begin(baudrate);
  display.begin();
  display.setContrast(50); //Ajusta o contraste do display

}

//INICIALIZA pinos GPIO
void initGPIO()
{
  pinMode(6, OUTPUT);
  digitalWrite(6, 1);
  attachInterrupt(digitalPinToInterrupt(3), botao, LOW);
}


String autentURL() {

  String URL = F("/devices?name=");
  URL += host_name;
  URL += F("&description=");
  URL += description_autent;
  URL += F("&chipset_id=");
  URL += id_chipset;
  URL += F("&processor_id=");
  URL += id_processor;
  URL += F("&mac=");
  URL += mac_aux;
  URL += F("&serial=");
  URL += hd_serial;

  return URL;

}

String expiredTokenURL() {
  String URL = F("/devices?name=");
  URL += host_name;
  URL += F("&description=");
  URL += description_autent;
  URL += F("&chipset_id=");
  URL += id_chipset;
  URL += F("&processor_id=");
  URL += id_processor;
  URL += F("&mac=");
  URL += mac_aux;
  URL += F("&serial=");
  URL += hd_serial;
  URL += F("&token=");
  URL += id_token;

  return URL;
}


String serviceURL() {

  String URL = F("/services?token=");
  URL += id_token;
  URL += F("&name=");
  URL += nome_service_servico1;
  URL += F("&type=");
  URL += inf_type;
  URL += F("&scpdurl=");
  URL += scpdurl;
  URL += F("&description=");
  URL += description_service;
  URL += F("&control_url=");
  URL += control_url;
  URL += F("&event_sud_url=");
  URL += event_sud_url;
  URL += F("&refresh_rate=");
  URL += refresh_rate;


  return URL;
}



String updateURL(String return_value, int i) {
  String URL;

  //se a flag = 1, significa que é um post para arguments. se for outro valor é um put para arguments
  if (i == POST) {
    URL = F("/arguments?token=");
    URL += id_token;
    URL += F("&action_id=");
    URL += action_id;
    URL += F("&name=");
    URL += nome_update_servico1;
    URL += F("&default_value=");
    URL += default_value_servico1;
    URL += F("&return_value=");
    URL += F("id_tag");
  }

  else if (i == PUT) {

    URL = F("/arguments?token=");
    URL += id_token;
    URL += F("&default_value=");
    URL += default_value_servico1;
    URL += F("&return_value=");
    URL += return_value;
    URL += F("&id=");
    URL += item_id;
  }

  else {
    URL = F("/arguments?token=");
    URL += id_token;
    URL += F("&action_id=");
    URL += ACTION_ID_RFID;
    URL += F("&name=");
    URL += F("RFID");
    URL += F("&return_value=");
    URL += return_value;
  }

  return URL;

}

String check_json(String aux) {


  String jsonstring;
  //Funcao que verifica se tem conteudo json na resposta do servidor
  int initial_pos, final_pos;
  initial_pos = aux.indexOf('{');
  final_pos = aux.lastIndexOf('}');

  if ((initial_pos != -1) && (final_pos != -1)) {
    //resposta do servidor no formato json
    jsonstring = aux.substring(initial_pos, final_pos + 1);
  }
  else {
    initial_pos = aux.indexOf('[');
    final_pos = aux.lastIndexOf(']');
    if ((initial_pos != -1) && (final_pos != -1)) {
      //resposta do servidor no formato json
      jsonstring = aux.substring(initial_pos, final_pos + 1);
    }
    else {
      jsonstring = "";
    }
  }
  return jsonstring;
}

String parseJson(String parametro, char tipo, String jsonstring)
{
  int i = 0, j = 0, flag = 0, gap;
  String valor = "";
  int fim = jsonstring.length() - 1;
  char comp, comp1;

  if (tipo == 'n') {
    comp = ',';
    comp1 = ' ';
    gap = 1;
  }
  else if (tipo == 's') {
    comp = '\"';
    gap = 2;
  }

  while (i != fim) {
    if (jsonstring[i] == parametro[j]) {
      j++;
      if (j == parametro.length()) {
        flag = 1;
        break;
      }
    }
    else {
      j = 0;
    }
    i++;
  }
  //se flag = 1, achou-se o parametro que se deseja obter a variavel
  if (flag == 1) {
    if (tipo == 's') {
      i += gap;
      while (jsonstring[i] != comp) {
        valor += jsonstring[i];
        i++;
      }
    }
    else {
      i += gap;
      while ((jsonstring[i] != comp) && (jsonstring[i] != comp1)) {
        valor += jsonstring[i];
        i++;
      }
    }
    return valor;
  }
}

String update_parsing(String jsonstring, int flag) {
  String resposta;
  int code;

  //fase de post para arguments. Cada fase tem determinada resposta do servidor para ser parseada

  //se flag = 1, representa o parsing para o post. Se for = 0, representa o put e se for igual a 2 representa o get
  if (flag == 1) {
    if (jsonstring != "") {

      //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
      resposta = parseJson(F("\"code\": "), 'n', jsonstring);
      code = resposta.toInt();
      if (code == 200) {
        resposta = parseJson(F("\"item_id\": "), 's', jsonstring);
        return resposta;
      }
      else if (code == 500) {
        return F("500");
      }
      return F("403");
    }
  }
  else if (flag == 0) {
    if (jsonstring != "") {

      //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
      resposta = parseJson(F("\"code\": "), 'n', jsonstring);
      code = resposta.toInt();
      if (code == 200) {
        resposta = F("ok");
        return resposta;
      }
      else if (code == 500) {
        return F("500");
      }
      else {
        return F("403");
      }
    }
  }
  else {
    //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
    resposta = parseJson(F("\"code\": "), 'n', jsonstring);
    code = resposta.toInt();
    if (parseJson(F("\"action_id\": "), 's', jsonstring) == ACTION_ID_RFID) {
      nome = parseJson(F("\"default_value\": "), 's', jsonstring);
      return F("200");
    }
//           if (code == 200) {
//              resposta = parseJson("\"return_value\": ", 's',jsonstring);
//              return resposta;
//           }

    else if (code == 500) {
      return "500";
    }
    else {
      return "404";
    }
  }
}

String service_parsing(String jsonstring) {
  String resposta;
  int code;

  //fase de cadastro de servico. Cada fase tem determinada resposta do servidor para ser parseada

  if (jsonstring != "") {

    //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
    resposta = parseJson(F("\"code\": "), 'n', jsonstring);
    code = resposta.toInt();
    if (code == 200) {
      resposta = parseJson(F("\"action_id\": "), 's', jsonstring);
      return resposta;
    }
    else if (code == 500) {
      return F("500");
    }
    return F("403");
  }
}


String authentication_parsing(String jsonstring) {
  String resposta;
  int code;

  //fase de autenticacao. Cada fase tem determinada resposta do servidor para ser parseada

  if (jsonstring != "") {

    //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
    resposta = parseJson(F("\"code\": "), 'n', jsonstring);
    code = resposta.toInt();
    if (code == 200) {
      resposta = parseJson(F("\"token\": "), 's', jsonstring);
      return resposta;
    }
    return F("403");
  }
}

