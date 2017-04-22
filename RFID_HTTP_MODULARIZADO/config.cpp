#include "config.h"


/*********
 * 
 * URLs cONFIGURATION
 * 
 ********/

String autentURL() {

  String URL = "/devices?name=";
  URL += host_name;
  URL += "&description=";
  URL += description_autent;
  URL += "&chipset_id=";
  URL += id_chipset;
  URL += "&processor_id=";
  URL += id_processor;
  URL += "&mac=";
  URL += mac_aux;
  URL += "&serial=";
  URL += hd_serial;

  return URL;

}

String expiredTokenURL(){
  String URL = "/devices?name=";
  URL += host_name;
  URL += "&description=";
  URL += description_autent;
  URL += "&chipset_id=";
  URL += id_chipset;
  URL += "&processor_id=";
  URL += id_processor;
  URL += "&mac=";
  URL += mac_aux;
  URL += "&serial=";
  URL += hd_serial;
  URL += "&token=";
  URL += id_token;

  return URL;
}


String serviceURL() {

  String URL = "/services?token=";
    URL += id_token;
    URL += "&name=";
    URL += nome_service_servico1;
    URL += "&type=";
    URL += inf_type;
    URL += "&scpdurl=";
    URL += scpdurl;
    URL += "&description=";
    URL += description_service;
    URL += "&control_url=";
    URL += control_url;
    URL += "&event_sud_url=";
    URL += event_sud_url;
    URL += "&refresh_rate=";
    URL += refresh_rate;


  return URL;
}



String updateURL(unsigned int return_value, int i){
    String URL;

    if(i == PUT){

       URL = "/arguments?token=";
       URL += id_token;
       URL += "&return_value=";
       URL += return_value;
       URL += "&id=";
       URL += item_id;
    }
    //se i nao for PUT, i = GET.
    else {
      URL = "/arguments?token=";
      URL += id_token;
      URL += "&action_id=";
      URL += ACTION_ID_RFID;
      URL += "&name=";
      URL += "RFID_CODE";
      URL += "&return_value=";
      URL += return_value;
    }
    
    return URL;

}
