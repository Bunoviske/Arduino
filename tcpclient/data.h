#ifndef DATA_H
#define DATA_H

#include "AT.h" 

static String jsonstring;    
static long id_token;
static long id_serv;
static long timing;

  void data_warning(String,const int timeout, int connectionId);
  int send_data(String, const int timeout);
  String create_http(String metodo, String);
  String parseJson(String, char);
  void check_json(String);
  String authentication_url(int id_chipset,int id_processor,String hd_serial,String host_name,int mac,String driver);
  //String service_url(int mac, String name, String inf_type, String unit, String description);
  //String update_url(int mac, String value);
  void authenticate(String metodo, int connectionId, int id_chipset,int id_processor,String hd_serial,String host_name,int mac,String driver);
  //void service(String metodo, int connectionId, int mac, String name, String inf_type, String unit, String description);
  //bool update(String metodo, int connectionId, int mac, String value);


#endif
