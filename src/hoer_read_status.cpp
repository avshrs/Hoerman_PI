#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>

Hoermann_pi door;


void th1(){
   door.run_loop();    
}


int main(){
   char serial_name[] = "/dev/ttyUSB1";
   int boudrate = 19200;
   door.init(serial_name, boudrate);
   std::thread t3(th1);
    sleep(60);
   while(1){
   
   door.set_state("toggle_light");
   sleep(1);
   std::cout<<door.get_state()<<std::endl;
   sleep(1);
   door.set_state("toggle_light");
   sleep(1);
   std::cout<<door.get_state()<<std::endl;
   sleep(1);
   }
return 1;
}