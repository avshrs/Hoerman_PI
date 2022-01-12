#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>

Hoermann_pi door;


void th1(){
   door.run_loop();    
}


int main(){
   char serial_name[] = "/dev/ttyUSB0";
   int boudrate = 19200;
   door.init(serial_name, boudrate);
   std::thread t3(th1);
   while(1){
   sleep(30);
   door.set_state("venting");
   sleep(10);
   door.set_state("close");
   sleep(10);
   }
return 1;
}