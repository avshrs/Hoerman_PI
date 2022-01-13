
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
    auto start = timer.now();
    auto check = timer.now();
    using ms = std::chrono::duration<float, std::micro>;

    while (1)
    {   
        serial.serial_read(rx_buffer, 7);
        
        start = timer.now();
        
        parse_message();

        if(tx_message_ready)
        {
          while(1)
          {
            check = timer.now();
            auto deltaTime = std::chrono::duration_cast<ms>(check - start).count();
           
           if( deltaTime > 3110)
           {
                  serial.serial_send(tx_buffer, tx_length);
                  tx_message_ready = false;
                  break;
            }
            usleep(10);
          }
        } 
    }       
}


void Hoermann_pi::parse_message(void)
{
  uint8_t length;
  uint8_t counter;
  
  length = rx_buffer[1+lz] & 0x0F;
  counter = (rx_buffer[1+lz] & 0xF0) + 0x10;
  

  if(rx_buffer[0+lz] == BROADCAST_ADDR)
  {
    if(length == 0x02)
    {
      broadcast_status = rx_buffer[2+lz];

      broadcast_status |= (uint16_t)rx_buffer[3+lz] << 8;
    }
  }
  if(rx_buffer[0+lz] == UAP1_ADDR)
  {
            for(int i=0; i<5+lz ; i++)
            {
                std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(rx_buffer[i]);
            }
            std::cout<<std::endl;
    /* Bus scan command? */
    if((length == 0x02) && (rx_buffer[2+lz] == CMD_SLAVE_SCAN))
    {
      if (lz)
      {
        tx_buffer[0] = 0x00;
      }
      
      tx_buffer[0+lz] = MASTER_ADDR;
      tx_buffer[1+lz] = 0x02 | counter;
      tx_buffer[2+lz] = UAP1_TYPE;
      tx_buffer[3+lz] = UAP1_ADDR;
      tx_buffer[4+lz] = calc_crc8(tx_buffer, 4+lz);
      tx_length = 5+lz;
      tx_message_ready = true;
   }
    /* Slave status request command? */
    if((length == 0x01) && (rx_buffer[2+lz] == CMD_SLAVE_STATUS_REQUEST))
    {
      if (lz)
      {
        tx_buffer[0] = 0x00;
      }
      tx_buffer[0+lz] = MASTER_ADDR;
      tx_buffer[1+lz] = 0x03 | counter;
      tx_buffer[2+lz] = CMD_SLAVE_STATUS_RESPONSE;
      tx_buffer[3+lz] = (uint8_t)slave_respone_data;
      tx_buffer[4+lz] = (uint8_t)(slave_respone_data>>8);
      slave_respone_data = RESPONSE_DEFAULT;
      tx_buffer[5+lz] = calc_crc8(tx_buffer, 5+lz);
      tx_length = 6+lz;
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
