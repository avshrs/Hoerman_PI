#include "hoermann.h"
#include <iostream>
#include <thread>
#include <unistd.h>

Hoermann_pi door;


void th1(){
   door.run_loop();    
}


int main(){
   const char serial_name[] = "/dev/ttyUSB2";
   int rs_lead_zero = 1;
   door.init(serial_name, 19200, rs_lead_zero);
   std::thread t3(th1);
   sleep(30);
   while(1){
      door.set_state("toggle_light");
      sleep(5);
      std::cout<<door.get_state()<<std::endl;
      sleep(5);
      door.set_state("toggle_light");
      sleep(5);
      std::cout<<door.get_state()<<std::endl;
      sleep(5);
   }
return 1;
}

// #include "USB_serial.h"
// #include <chrono>
// #include "hoermann.h"
// #include <iostream>
// #include <iomanip>
// #include <unistd.h>

// std::chrono::high_resolution_clock timer;
// using mi = std::chrono::duration<float, std::micro>;

// int main(){
//    auto check = timer.now();
//    auto start = timer.now();
//    USB_serial serial;
//    Hoermann_pi door;
//    char serial_name[] = "/dev/ttyUSB2";
//    serial.serial_open(serial_name);
   
//    while(true){
//       uint8_t buf[6];
//       serial.serial_read(buf, 7);
//       if(buf[1] == 0x28){
//          start = timer.now();
//          door.print_buffer(buf, 7);
//       }   
//       else if((buf[0] == 0x80 && buf[3] != 0x80 ) || (buf[1] == 0x80 && buf[4] != 0x80)){
//             check = timer.now();
//             auto deltaTime = std::chrono::duration_cast<mi>(check - start).count();
//             door.print_buffer(buf, 7);
//             std::cout<< "Packet_Delta: " << deltaTime <<std::endl;

//       }
//    }




// }