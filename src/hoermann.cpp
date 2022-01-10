// based on https://github.com/stephan192/hoermann_door

#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>

#define SYNC_BYTE   0x55


void Hoermann::loop(void)
{
  
  if (read_rs232() == true)
  {
    parse_input();
    std::cout << "actual state: " << get_state_string() << std:: endl; 
  }

  if (actual_action != hoermann_action_none)
  {
    // send_command(0x55);
    actual_action = hoermann_action_none;
  }
  
}

hoermann_state_t Hoermann::get_state(void)
{
  return actual_state;
}

std::string Hoermann::get_state_string(void)
{
  return actual_state_string;
}

void Hoermann::trigger_action(hoermann_action_t action)
{
  actual_action = action;
}

void Hoermann::open_serial(char * serial_name, int boudrate)
{
  serial.serial_open2(serial_name, boudrate, false, NULL);
  serial.serial_open(serial_name, boudrate);
}


bool Hoermann::read_rs232(void)
{
    while (1)
    {
      serial.serial_read(buf, 4);
      //  for(int i=0; i<5 ; i++){
    
      //   std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(buf[i]);
      //   } std::cout << std::endl;
      if (buf[0]== 0x00 && buf[1]== 0x12 ) 
      {
         for(int i=0; i<5 ; i++){
    
        std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(buf[i]);
        } std::cout << std::endl;
        if (calc_checksum(buf, 5) == buf[4])
        {
          return true;
        }
      }
    }
  return false;

} 

void Hoermann::parse_input(void)
{
  if (buf[0] == 0x00)
  {
    if (buf[1] == 0x12)
    {
      if ((buf[2] & 0x01) == 0x01)
      {
        actual_state = hoermann_state_open;
        actual_state_string = "open";
      }
      if ((buf[2] & 0x02) == 0x02)
      {
        actual_state = hoermann_state_open;
        actual_state_string = "close";
      }
      else if ((buf[2] & 0x80) == 0x80)
      {
        actual_state = hoermann_state_venting;
        actual_state_string = "venting";
      }
      else if ((buf[2] & 0x60) == 0x40)
      {
        actual_state = hoermann_state_opening;
        actual_state_string = "opening";
      }
      else if ((buf[2] & 0x60) == 0x60)
      {
        actual_state = hoermann_state_closing;
        actual_state_string = "closing";
      }
      else if ((buf[2] & 0x10) == 0x10)
      {
        actual_state = hoermann_state_error;
        actual_state_string = "error";
      }
      else
      {
        actual_state = hoermann_state_stopped;
        actual_state_string = "stopped";
      }
    }
  }
}
/*
1 1 1 1  1 1 1 1
0 0 0 0  0 0 0 0 x01  open
0 0 0 1  0 0 0 0 x10 error
0 1 0 1  0 0 0 0 x40 opening
0 1 1 0  0 0 0 0 x60 closing
1 0 0 0  0 0 0 0 x80 venting
*/

void Hoermann::send_command(uint8_t i)
{
  output_buffer[0] = i;
  output_buffer[1] = 0x01;
  output_buffer[2] = 0x01;
  output_buffer[3] = (uint8_t)actual_action;
  output_buffer[4] = output_buffer[0] + output_buffer[1] + output_buffer[2] + output_buffer[3];
  for (int i =0 ; i<16; i++){
      output_buffer_[i] = (char)output_buffer[i];
  }
  
  serial.serial_send(&output_buffer_[0], 16);
}

uint8_t Hoermann::calc_checksum(char *p_data, uint8_t length)
{
  uint8_t i;
  uint8_t crc = 0;

  for (i = 0; i < length; i++)
  {
    crc += *p_data;
    p_data++;
  }

  return crc;
}
