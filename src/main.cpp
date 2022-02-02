#include "Config_manager.h"
#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <iomanip>
#include <ctime>
#include "Mosquitto.h"



std::string date(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);      
    std::stringstream ss; 
    ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ") ;
    return ss.str();
}

void th1(Hoermann_pi *door){
   door->run_loop();    
}
void th2(Mqtt_Client *mqtt){
    mqtt->client_loop_forever();
}

int main()
{
   Config_manager cfg;
   Hoermann_pi door;

    //  read config from config.yaml
   cfg.read_config();

    // run mqtt client
   Mqtt_Client mqtt(cfg.get_mqtt_ClientId().c_str(), cfg.get_mqtt_ip().c_str(), cfg.get_mqtt_port(), cfg.get_mqtt_username().c_str(), cfg.get_mqtt_password().c_str());

   door.register_mqtt(&mqtt);
   door.register_cfg(&cfg);

   std::string serial_file_name = cfg.get_hoer_serial_file();
   int rs_lead_zero = cfg.get_hoer_lead_zeros();
   int boudrate = cfg.get_hoer_boudrate();
   door.init(serial_file_name.c_str(), boudrate, rs_lead_zero);

   mqtt.register_horman(&door);
   mqtt.register_config(&cfg);

   sleep(1);
   
   std::thread Hoermann_door_service(th1, &door);
   std::thread Mqtt_service(th2, &mqtt);

   std::string kmsg = cfg.get_mqtt_keepAliveMsg();
   std::string ktop = cfg.get_mqtt_keepAliveTopic();
   std::string pub = cfg.get_mqtt_Pubstring();

   while (true)
   {
        std::string door_state = door.get_state();
        std::cout<< date() << "door status: "  << door_state << std::endl;
        mqtt.publish(NULL, ktop.c_str(), kmsg.length(), kmsg.c_str());
        mqtt.publish(NULL, pub.c_str(), door_state.length(), door_state.c_str());
        sleep(60);
   }

return 0;
}


