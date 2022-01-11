#include "hoermann.h"
#include <iostream>
// #include <thread>

#include <unistd.h>
Hoermann door;
hoermann_state_t current_door_state = hoermann_state_unkown;
hoermann_state_t last_door_state = hoermann_state_unkown;


// void th1(){
//     door.loop();
// }


int main(){
   
   char serial_name[] = "/dev/ttyUSB0";
   int boudrate = 19200;
   door.open_serial(serial_name, boudrate);
   
   // while(1){
   door.loop();
   current_door_state = door.get_state();
   
   std::string state = door.get_state_string();
   
// std::cout << current_door_state << std::endl;
   std::cout << state << std::endl;
   sleep(1);
   door.trigger_action(hoermann_action_venting);
   sleep(10);
   door.trigger_action(hoermann_action_close);
   sleep(10);
   // }
return 1;
}