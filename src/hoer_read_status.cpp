#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include "Mosquitto.h"
#include "Config_manager.h"

Hoermann_pi door;

void th1(){
   door.run_loop();    
}

int main(){
   
   Config_manager cfg;

   cfg.read_config();
   std::string serial_file = cfg.get_hoer_serial_file();
   int rs_lead_zero = cfg.get_hoer_lead_zeros();
   int boudrate = cfg.get_hoer_boudrate();

   Mqtt_Client mqtt(cfg.get_mqtt_ClientId().c_str(), cfg.get_mqtt_ip().c_str(), cfg.get_mqtt_port(), cfg.get_mqtt_username().c_str(), cfg.get_mqtt_password().c_str());

   mqtt.register_horman(&door);
   door.register_mqtt(&mqtt);
   door.init(serial_file.c_str(), boudrate, rs_lead_zero);
   
   std::thread t3(th1);
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

