/* Falta tratar o caso em que há desconexão com o servidor e há acumulação de dados no canal Serial do esp
 *  
 *  Discutir dois métodos:
 *  
 *  1-fazer esse tratamento no código do esp, fazendo ele limpar o canal e perder dados enviados pelo arduino
 *  
 *  2-fazer esse tratamento no código do arduino, fazendo com que ele mande mais dados apenas se houver resposta ok do servidor na última requisição feita.
 * 
 *  
 */



//*****************************************************************************************************
//HEADER's

#include <SoftwareSerial.h>
#include <DHT.h>


//*****************************************************************************************************
//DEFINE's


//autenticacao
#define host_name               "Arduino_Monitoramento_Clima"

#define refresh_rate            10000

#define NUM_SERVICOS            3 //num servicos define tanto o num de action id quanto num de item id

// DHT
#define DHTPIN                  7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


//*****************************************************************************************************
//GLOBAIS

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
String mac_aux = "0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED";

String nome_servico[NUM_SERVICOS];

//ponteiro de função da máquina de estados. Ele aponta sempre para a função da máquina de estados que deve ser executada
void (*FSM_SBRT)();
void (*FSM_SBRT_anterior)();

SoftwareSerial esp(2, 3);

String IpAddress = "";



//*****************************************************************************************************
//Prototipos

//FSM
void StateInit(void);       //função que representa o estado inicial da máquina de estados
void StateUpdate(void);


//Metodos privados

void initSerial(unsigned int baudrate);
void initGPIO();
unsigned int readSensor();

//*****************************************************************************************************
//Setup
void setup () {
  initSerial(9600);
  initGPIO();

  delay(20000); // DELAY INICIAL DO CODIGO, PARA GARANTIR QUE O ESP JÁ ESTÁ INICIALIZADO E CONECTADO NA REDE E NO SERVIDOR

  Serial.println("Iniciando codigo");

  nome_servico[0] = "temperatura";
  nome_servico[1] = "humidade";
  nome_servico[2] = "luminosidade";

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

  //  //aguardando confirmação de inicialização do esp
  //  while (true) {
  //    if (esp.available()) {
  //      String resposta = esp.readString();
  //      Serial.println(resposta);
  //      if (resposta == "ok") {
  //        Serial.println("Sincronizado com esp");
  //        break;
  //      }
  //      else {
  //        Serial.println("Erro de sincronização com o esp, resetar ambos dispositivos");
  //      }
  //    }
  //
  //  }

  //Estado que pede ao esp o ipaddress dele. 
  //O codigo só deve avançar se tiver o ip. Caso não tenha, quer dizer que o esp não está conectado no servidor ou na rede.
  
  Serial.println("Pegando ip do esp");

  esp.print("get_ip");

  while (IpAddress == "") {
    if (esp.available()) {
      IpAddress = esp.readString();
      Serial.println(IpAddress);
    }
  }

  //TROCA DE ESTADO
  FSM_SBRT = StateUpdate;

  delay(2000);
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
    atualizacao += refresh_rate / 1000;
    Serial.println(atualizacao);
    esp.print(atualizacao);

    unsigned long previous_time = millis();
    while ((millis() - previous_time) <= 5000) {
      if (esp.available()) {
        String resposta = esp.readString();
        Serial.println(resposta);
        break;
      }
    }

  }

  FSM_SBRT = StateUpdate;
  delay(refresh_rate);
}



//*****************************************************************************************************
//METODOS PRIVADOS

float readSensor(int data)
{
  {
    float value;
    if (data == 0) {
      value = dht.readTemperature();
      Serial.print("Temperatura: ");
      Serial.println(value);
    }
    else if (data == 1) {
      value = dht.readHumidity();
      Serial.print("Humidade: ");
      Serial.println(value);

    }
    else {
      value = analogRead(A0);
      Serial.print("raw value: ");
      Serial.print(value);
      //transformacao para que 0 seja baixa luminosidade e 1024 seja alta luminosidade
      value = 1024 - value;
      //transformacao de porcentagem
      value = (100 * value) / 1024;
      Serial.print("Luminosidade: ");
      Serial.print(value);
      Serial.println("%");
    }
    return value;

  }


}

//  //essa formatação não coloca 0x antes de cada parte do MAC Address
//  String mac2String(byte mac_array[]) {
//    String s;
//    for (byte i = 0; i < 6; ++i)
//    {
//      char buf[3];
//      sprintf(buf, "0x%2x", mac_array[i]);
//      s += buf;
//      if (i < 5) s += ',';
//    }
//    return s;
//  }



//INICIALIZA A COMUNICAÇão SERIAL
void initSerial(unsigned int baudrate)
{
  Serial.begin(baudrate);
  esp.begin(baudrate);

}

//INICIALIZA pinos GPIO
void initGPIO()
{
  dht.begin();
}

