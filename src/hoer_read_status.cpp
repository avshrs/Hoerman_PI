#include "Config_manager.h"
#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>

#include "Mosquitto.h"




void th1(Hoermann_pi *door){
   door->run_loop();    
}
void th2(Mqtt_Client *mqtt){
    mqtt->client_loop_forever();
}

int main(){
   Config_manager cfg;
   cfg.read_config();

   Hoermann_pi door;
   std::string serial_file = cfg.get_hoer_serial_file();
   int rs_lead_zero = cfg.get_hoer_lead_zeros();
   int boudrate = cfg.get_hoer_boudrate();

   Mqtt_Client mqtt(cfg.get_mqtt_ClientId().c_str(), cfg.get_mqtt_ip().c_str(), cfg.get_mqtt_port(), cfg.get_mqtt_username().c_str(), cfg.get_mqtt_password().c_str());
   door.register_mqtt(&mqtt);
   door.register_cfg(&cfg);
   mqtt.register_horman(&door);
   mqtt.register_mcp_config(&cfg);
   sleep(1);

   door.init(serial_file.c_str(), boudrate, rs_lead_zero);
   
   std::thread t3(th1, &door);
   std::thread t2(th2, &mqtt);

   std::string kmsg = cfg.get_mqtt_keepAliveMsg();
   std::string ktop = cfg.get_mqtt_keepAliveTopic();
   std::string pub = cfg.get_mqtt_Pubstring();

   while (true)
   {
        std::string door_state = door.get_state();
        mqtt.publish(NULL, ktop.c_str(), kmsg.length(), kmsg.c_str());
        mqtt.publish(NULL, pub.c_str(), door_state.length(), door_state.c_str());
        sleep(60);
   }
        
    

return 1;
}


