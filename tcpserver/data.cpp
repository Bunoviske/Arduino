#include "data.h"

void Data::data_warning(const int timeout, int connectionId)
{
     String cipSend = "AT+CIPSEND=";
     cipSend += connectionId;
     cipSend += ",";
     cipSend += data_value.length();
     cipSend +="\r\n";
     sendCommand(cipSend,timeout);
}

int Data::send_data(const int timeout)
{
   String resposta;
   resposta = sendCommand(data_value,timeout);
   this->check_json(resposta);
   if (this->jsonstring != ""){
    return 1;
   }
   else{
    return 0;
   }
}

String Data::get_data(){
  return data_value;
}

void Data::set_data(String data)
{
  data_value = data;
}

void Data::create_http(String data, String metodo)
{
    String http = metodo;
    http += data;
    http += " HTTP/1.1\r\n";
    //http += "Connection: close\r\n";
    //http += "Content-Lenght: ";
    //http += data.length();
    //http += "\r\n";
    http += "\r\n";
    this->set_data(http);
            
}



String Data::parseJson(String parametro, char tipo)
{
    int i=0, j=0, flag=0, gap;
    String valor = "";
    int fim = jsonstring.length() - 1;
    char comp;
    
    if (tipo == 'n'){
      comp = ' ';
      gap = 1;
    }
    else if(tipo == 's'){
      comp = '\"';
      gap = 2;
    }
    
    while (i != fim){
      if (jsonstring[i] == parametro[j]){
        j++;
        if(j == parametro.length()){
          flag=1;
          break;
        }
      }
      else{
        j=0;
      }
      i++;
    }
    //se flag = 1, achou-se o parametro que se deseja obter a variavel
    if (flag == 1){
      i += gap;
      while (jsonstring[i] != comp){
        valor += jsonstring[i];
        i++;
      }
      return valor;
    }
}    


String Data::get_jsonstring(){
  return jsonstring;
}

void Data::check_json(String aux){
   //Funcao que verifica se tem conteudo json na resposta do servidor
   int initial_pos, final_pos;
   
   initial_pos = aux.indexOf('{');
   final_pos = aux.lastIndexOf('}');
   if ((initial_pos != -1) && (final_pos != -1)){
    //resposta do servidor no formato json
    jsonstring = aux.substring(initial_pos, final_pos+1);
   }
   else{ 
    initial_pos = aux.indexOf('[');
    final_pos = aux.lastIndexOf(']');
    if ((initial_pos != -1) && (final_pos != -1)){
      //resposta do servidor no formato json
      jsonstring = aux.substring(initial_pos, final_pos+1);
    }
    else{
      jsonstring = "";
    }
  }
  
}

