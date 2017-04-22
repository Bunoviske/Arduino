#include "settings.h"
#include "dados_dispositivo.h"

int connectionId;


void setup()
{
     Serial.begin(9600);

  
     
     //principal função da classe de conexão. Responsável por conectar o dispositivo na rede e configurá-lo
     esp_client->get_connection();
     esp_client->create_connection(2000);
     connectionId = esp_client->get_connectionId();

     authenticate("POST", connectionId, id_chipset, id_processor, hd_serial, host_name, mac, driver);
     //service("POST", connectionId, mac, name, inf_type, unit, description);
     
     

     //esp_client->close_connection(2000);

}


void loop()
{
  /*toda vez que for enviar dados, criar conexão e depois fechá-la. Se quiser, pode abrir apenas 
  uma vez no SETUP e não fechar
  */
   String value = "23.9";

/*   if (!dado->update("PUT", connectionId, mac, value)){
    //se der erro na hora de enviar dado, pedir para se autenticar de novo com PUT
      authenticate("PUT", connectionId, id_chipset, id_processor, hd_serial, host_name, mac, driver);
   }*/
   delay(timing);


}


