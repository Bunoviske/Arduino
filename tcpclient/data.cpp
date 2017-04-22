#include "data.h"

void data_warning(String data_value, const int timeout, int connectionId)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += data_value.length();
  cipSend += "\r\n";
  sendCommand(cipSend, timeout);
}

int send_data(String data_value, const int timeout)
{
  String resposta;
  resposta = sendCommand(data_value, timeout);
  check_json(resposta);
  if (jsonstring != "") {
    return 1;
  }
  else {
    return 0;
  }
}

/*
bool update(String metodo, int connectionId, int mac, String value) {
  bool jsonflag = 0;
  String resposta;
  long code;

  update_url(mac,value);
  //create_http(metodo);
  data_warning(resposta,2000, connectionId);
  jsonflag = send_data(resposta, 3000);
  
    // se houver conteudo json, chamar funcao para interpretar a mensagem (funcao parseJson)
    if (jsonflag == 1) {
      Serial.print("JSON STRING::::::::::: ");
      Serial.println(jsonstring);
  
      //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
      resposta = parseJson("\"code\": ", 'n');
      Serial.println(resposta);
      code = resposta.toInt();
      if (code == 200) {
        return 1;
      }
      else {
        return 0;
      }
    }
}

void service(String metodo, int connectionId, int mac, String name, String inf_type, String unit, String description) {
  bool jsonflag = 0;
  String resposta;
  long code = 403;

  while (code != 200) {
    service_url(mac, name, inf_type, unit, description);
    //create_http(metodo);
    data_warning(resposta,2000, connectionId);
    jsonflag = send_data(resposta,3000);
  
      // se houver conteudo json, chamar funcao para interpretar a mensagem (funcao parseJson)
      if (jsonflag == 1) {
        Serial.print("JSON STRING::::::::::: ");
        Serial.println(jsonstring);
  
        //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
        resposta = parseJson("\"code\": ", 'n');
        Serial.println(resposta);
        code = resposta.toInt();
        if (code == 200) {
          resposta = parseJson("\"id_serv\": ", 'n');
          Serial.println(resposta);
          this->id_serv = resposta.toInt();
  
          resposta = parseJson("\"timing\": ", 'n');
          Serial.println(resposta);
          this->timing = resposta.toInt();
        }
      }
   } 
}*/

void authenticate(String metodo, int connectionId, int id_chipset,int id_processor,String hd_serial,String host_name,int mac,String driver) {
  bool jsonflag = 0;
  String resposta;
  long code = 403;

  while (code != 200) {

    //fase de autenticacao
    resposta = authentication_url(id_chipset, id_processor, hd_serial, host_name, mac, driver);
    Serial.println(resposta);
    resposta = create_http(metodo, resposta);
    data_warning(resposta, 2000, connectionId);
    jsonflag = send_data(resposta,3000);
  
      // se houver conteudo json, chamar funcao para interpretar a mensagem (funcao parseJson)
      if (jsonflag == 1) {
        Serial.print("JSON STRING::::::::::: ");
        Serial.println(jsonstring);
  
        //mandar como parametro o tipo da variavel a ser analisada. s = string e n = numero.
        resposta = parseJson("\"code\": ", 'n');
        Serial.println(resposta);
        code = resposta.toInt();
        if (code == 200) {
          resposta = parseJson("\"token\": ", 'n');
          Serial.println(resposta);
          id_token = resposta.toInt();
        }
      }
  }
}
/*
String service_url(int mac, String name, String inf_type, String unit, String description) {
  String URL = " /services?id_token=";
  URL += this->id_token;
  URL += "&mac=";
  URL += mac;
  URL += "name=";
  URL += name;
  URL += "&inf_type=";
  URL += inf_type;
  URL += "&unit=";
  URL += unit;
  URL += "&description=";
  URL += description;
}

String update_url(int mac, String value) {
  String URL = " /services?id_token=";
  URL += this->id_token;
  URL += "&mac=";
  URL += mac;
  URL += "&id_serv=";
  URL += this->id_serv;
  URL += "&value=";
  URL += value;
}*/

String authentication_url(int id_chipset, int id_processor, String hd_serial, String host_name, int mac, String driver) {
  
  /*String URL = " /devices?name=";
  URL += host_name;
  URL += "&chipset_id=";
  URL += id_chipset;
  URL += "&processor_id=";
  URL += id_processor;
  URL += "&mac=";
  URL += mac;
  URL += "&serial=";
  URL += hd_serial;
  URL += "&communication_channel=";
  URL += driver;
  return URL;*/
  String URL = " /devices?name=";
  URL += host_name;
  URL += "&description=";
  URL += "oi";
  URL += "&chipset_id=";
  URL += id_chipset;
  URL += "&processor_id=";
  URL += id_processor;
  URL += "&mac=";
  URL += mac;
  URL += "&serial=";
  URL += hd_serial;
  return URL;
}

String create_http(String metodo, String URL)
{
  String http = metodo;
  http += URL;
  http += " HTTP/1.1\r\n";
  http += "\r\n";
  return http;
}



String parseJson(String parametro, char tipo)
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
    if (tipo == 's'){
      i += gap;
      while (jsonstring[i] != comp) {
        valor += jsonstring[i];
        i++;
      }
    }
    else{
      i += gap;
      while ((jsonstring[i] != comp) && (jsonstring[i] != comp1)) {
        valor += jsonstring[i];
        i++;
      }
    }
    return valor;
  }
}



void check_json(String aux) {
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

}

