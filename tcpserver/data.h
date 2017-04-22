#ifndef DATA_H
#define DATA_H

#include "AT.h" 

class Data
{
  private:
  String data_value;
  String jsonstring;    
  public:

  String get_data();
  void set_data(String data);
  void data_warning(const int timeout, int connectionId);
  int send_data(const int timeout);
  void create_http(String data, String metodo);
  String parseJson(String, char);
  String get_jsonstring();
  void check_json(String);
  
};

#endif
