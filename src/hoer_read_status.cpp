#include "hoermann.h"
#include <iostream>

Hoermann door;
hoermann_state_t current_door_state = hoermann_state_unkown;
hoermann_state_t last_door_state = hoermann_state_unkown;



int main(){
   door.loop();
   current_door_state = door.get_state();
   std::string state = door.get_state_string();
   
std::cout << current_door_state << std::endl;
std::cout << state << std::endl;

return 1;
}