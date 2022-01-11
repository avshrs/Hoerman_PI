#include "hoermann.h"
#include <iostream>
#include <thread>

#include <unistd.h>
Hoermann_pi door;


// void th1(){
    
// }


int main(){
   
   char serial_name[] = "/dev/ttyUSB0";
   int boudrate = 19200;
   door.open_serial(serial_name, boudrate);
   
   door.start_frame_listener();

return 1;
}