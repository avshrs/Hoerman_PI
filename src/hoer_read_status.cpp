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
   door.loop();

   std::string state = door.get_state_string();
   

std::cout << state << std::endl;
   
   }
return 1;
}