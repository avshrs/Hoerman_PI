#include "hoermann.h"
#include <iostream>
#include <unistd.h>
Hoermann door;
hoermann_state_t current_door_state = hoermann_state_unkown;
hoermann_state_t last_door_state = hoermann_state_unkown;



int main(){
   while(1){
   char serial_name[] = "/dev/ttyUSB0";
   int boudrate = 19200;
   door.open_serial(serial_name, boudrate);
   for(uint8_t i =0 ; i<200 ; i++){
      door.send_command(i);
   }
   
   door.loop();

   current_door_state = door.get_state();
   std::string state = door.get_state_string();
   
std::cout << current_door_state << std::endl;
std::cout << state << std::endl;
   sleep(1);
   }
return 1;
}