// #include "hoermann.h"
// #include <iostream>
// #include <thread>
// #include <unistd.h>

// Hoermann_pi door;


// void th1(){
//    door.run_loop();    
// }

// int main(){
//    const char serial_name[] = "/dev/ttyUSB2";
//    int rs_lead_zero = 1;
//    door.init(serial_name, 19200, rs_lead_zero);
//    std::thread t3(th1);
//    sleep(30);
//    while(1){
//       door.set_state("toggle_light");
//       sleep(5);
//       std::cout<<door.get_state()<<std::endl;
//       sleep(5);
//       door.set_state("toggle_light");
//       sleep(5);
//       std::cout<<door.get_state()<<std::endl;
//       sleep(5);
//    }
// return 1;
// }

#include "USB_serial.h"
#include <chrono>
#include "hoermann.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include "vars.h"
std::chrono::high_resolution_clock timer;
using mi = std::chrono::duration<float, std::micro>;

int main(){
   auto check = timer.now();
   auto start = timer.now();
   USB_serial serial;
   Hoermann_pi door;
   char serial_name[] = "/dev/ttyUSB2";
   serial.serial_open(serial_name, 19200, 1);
   RX_Buffer *rx_buf;
   rx_buf = new RX_Buffer;
   

   while(true){
      
      serial.serial_read(rx_buf);
      if(rx_buf->buf[0] == 0x28){
         start = timer.now();
         door.print_buffer(rx_buf->buf.data(), rx_buf->buf.size());
      }   
      else if((rx_buf->buf[0] == 0x80 && rx_buf->buf[3] != 0x80 )){
            check = timer.now();
            auto deltaTime = std::chrono::duration_cast<mi>(check - start).count();
            door.print_buffer(rx_buf->buf.data(), rx_buf->buf.size());
            std::cout<< "Packet_Delta: " << deltaTime <<std::endl;

      }
   }




}