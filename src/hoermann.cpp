
#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include "USB_serial.h"


void Hoermann_pi::init(char* serial_name, int boudrate)
{
    serial.serial_open2(serial_name, boudrate, false, NULL);
    // serial.serial_open(serial_name, boudrate);
}


void Hoermann_pi::run_loop(void)
{   
    auto duration = now.time_since_epoch();
    auto duration2 = now.time_since_epoch();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    while (1)
    {   std::cout<< "read: "<<std::endl;
        serial.serial_read(rx_buffer, 7);
        duration = now.time_since_epoch();
        parse_message();
        std::cout<< "tx_message_ready: "<<std::endl;

        if(tx_message_ready)
        
        {
          std::cout<< "tx_message_ready: in "<<std::endl;
             duration2 = now.time_since_epoch();
            // while(1){
               duration2 = now.time_since_epoch();
               auto d = std::chrono::duration_cast<std::chrono::microseconds>(duration2 - duration).count();
               std::cout<< "microseconds: "<<d<<std::endl;
              // if( d > 3000){
                  serial.serial_send(tx_buffer, tx_length);
                  tx_message_ready = false;
              // }
            usleep(100);
            // }
        } 
    }       
}

void Hoermann_pi::parse_message(void)
{
  uint8_t length;
  uint8_t counter;
  
  length = rx_buffer[1] & 0x0F;
  counter = (rx_buffer[1] & 0xF0) + 0x10;
  
  if(rx_buffer[0] == BROADCAST_ADDR)
  {
    if(length == 0x02)
    {
      broadcast_status = rx_buffer[2];
      broadcast_status |= (uint16_t)rx_buffer[3] << 8;
    }
  }
  if(rx_buffer[+0] == UAP1_ADDR)
  {
    for(int i=0; i<tx_length ; i++)
    {
      std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(rx_buffer[i]);
    }
    std::cout<<std::endl;
    /* Bus scan command? */
    if((length == 0x02) && (rx_buffer[2] == CMD_SLAVE_SCAN))
    {
      tx_buffer[0] = MASTER_ADDR;
      tx_buffer[1] = 0x02 | counter;
      tx_buffer[2] = UAP1_TYPE;
      tx_buffer[3] = UAP1_ADDR;
      tx_buffer[4] = calc_crc8(tx_buffer, 4);
      tx_length = 5;
      tx_message_ready = true;
   }
    /* Slave status request command? */
    if((length == 0x01) && (rx_buffer[2] == CMD_SLAVE_STATUS_REQUEST))
    {
      tx_buffer[0] = MASTER_ADDR;
      tx_buffer[1] = 0x03 | counter;
      tx_buffer[2] = CMD_SLAVE_STATUS_RESPONSE;
      tx_buffer[3] = (uint8_t)slave_respone_data;
      tx_buffer[4] = (uint8_t)(slave_respone_data>>8);
      slave_respone_data = RESPONSE_DEFAULT;
      tx_buffer[5] = calc_crc8(tx_buffer, 5);
      tx_length = 6;
      tx_message_ready = true;
    }    
  }
}

std::string Hoermann_pi::get_state()
{
  if ((broadcast_status & 0x01) == 0x01)
  {
    return states[1];
  }
  else if ((broadcast_status & 0x02) == 0x02)
  {
    return states[2];
  }
  else if ((broadcast_status & 0x80) == 0x80)
  {
    return states[3];
  }
  else if ((broadcast_status & 0x60) == 0x40)
  {
    return  states[4];
  }
  else if ((broadcast_status & 0x60) == 0x60)
  {
    return  states[5];
  }
  else if ((broadcast_status & 0x10) == 0x10)
  {
    return states[6];
  }
  else if (broadcast_status == 0x00)
  {
    return states[0];
  }
  else 
    return states[7];
 
}


void Hoermann_pi::set_state(std::string action)
{

    if(action == "stop")
    {
      std::cout<<"cmd stop"<<std::endl;
      slave_respone_data = RESPONSE_STOP;
    }
    else if(action == "open")
    {
      slave_respone_data = RESPONSE_OPEN;
      std::cout<<"cmd open"<<std::endl;
    }
    else if(action == "close")
    {
      std::cout<<"cmd close"<<std::endl;
      slave_respone_data = RESPONSE_CLOSE;
    }
    else if(action == "venting")
    {
      std::cout<<"cmd venting"<<std::endl;
      slave_respone_data = RESPONSE_VENTING;
    }
    else if(action == "toggle_light")
    {
      std::cout<<"cmd toggle_light"<<std::endl;
      slave_respone_data = RESPONSE_TOGGLE_LIGHT;
    }
    
}



uint8_t Hoermann_pi::calc_crc8(uint8_t *p_data, uint8_t len)
{
size_t i;
uint8_t crc = CRC8_INITIAL_VALUE;
    while(len--){
        crc ^= *p_data++;
        for(i = 0; i < 8; i++){
            if(crc & 0x80){
                crc <<= 1;
                crc ^= 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    //std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(crc);
    return(crc);
}
