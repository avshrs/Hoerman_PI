// based on https://github.com/stephan192/hoermann_door

#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>

#define SYNC_BYTE   0x55


void Hoermann::loop(void)
{
  send_command();
  if (read_rs232() == true)
  {
    parse_input();
    std::cout << "actual state: " << get_state_string() << std:: endl; 
  }

  if (actual_action != hoermann_action_none)
  {
    send_command();
    actual_action = hoermann_action_none;
  }
  usleep(1000000);
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
  serial.serial_open(serial_name, boudrate);
}


bool Hoermann::read_rs232(void)
{
  static uint8_t counter = 0;
  static uint8_t len = 0;
  // 0x00 0x12 0x01 0x02 0x56

  while(1){
    // read the incoming byte:
    char buf[16]; 
    serial.serial_read(buf, 16);
    std::cout << std::setw(4) << std::setfill('0')<<std::hex << static_cast<int>(buf[0]) << std::endl;

    uint8_t data = (uint8_t)buf[0];
    
    if ((data == 0x55) && (counter == 0))
    {
      // std::cout<< std::endl<< std::endl << "found 0x55 in buffer and counter = 0" << std::dec <<  (int)len << std::endl;
      rx_buffer[counter] = data;
      counter++;
      len = 0;
    }
    else if (counter > 0)
    {
      // std::cout << std::setw(4) << std::setfill('0')<<std::hex << static_cast<int>(buf[0]) << std::endl;
      // std::cout << "counter > 0 | " << counter << " len: " << std::dec <<  (int)len << std::endl;

      rx_buffer[counter] = data;
      counter++;
      if (counter == 3)
      {
        // std::cout << "counter == 3 | " << counter << " len: " << std::dec <<  (int)len << std::endl;
        if (data < 16)
        {
          // std::cout << "data  < 16 | " << counter << " len: " << std::dec <<  (int)len << std::endl;
          len = data + 4; //3 = SYNC + CMD + LEN + CHK, limit to 15 data bytes
          // std::cout << "counter == 3 | " << counter << " len: " << std::dec <<  (int)len << std::endl;
        }
        else
        {
          counter = 0;
        }
      }
      else if (counter == len)
      {
        // std::cout << "counter == len | counter:" << counter << " len: " << std::dec << (int)len << std::endl;

        if (calc_checksum(rx_buffer, len - 1) == data)
        {
          counter = 0;
          std::cout << "check sum ok " << std::setw(4) << std::setfill('0')<<std::hex << static_cast<int>(data) <<std::endl<<std::endl<<std::endl;
          return true;
          
        }
        else{
          std::cout << "check sum NOT ok " << std::setw(4) << std::setfill('0')<<std::hex << static_cast<int>(data) <<std::endl<<std::endl<<std::endl;
        }

        counter = 0;
      }
    }
  }

  return false;
}

void Hoermann::parse_input(void)
{
  if (rx_buffer[1] == 0x00)
  {
    if (rx_buffer[2] == 0x02)
    {
      if ((rx_buffer[3] & 0x01) == 0x01)
      {
        actual_state = hoermann_state_open;
        actual_state_string = "open";
      }
      else if ((rx_buffer[3] & 0x80) == 0x80)
      {
        actual_state = hoermann_state_venting;
        actual_state_string = "venting";
      }
      else if ((rx_buffer[3] & 0x40) == 0x40)
      {
        actual_state = hoermann_state_opening;
        actual_state_string = "opening";
      }
      else if ((rx_buffer[3] & 0x60) == 0x60)
      {
        actual_state = hoermann_state_closing;
        actual_state_string = "closing";
      }
      else if ((rx_buffer[3] & 0x10) == 0x10)
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
0 0 0 0  0 0 0 0 x01  
0 0 0 1  0 0 0 0 x10
0 1 0 1  0 0 0 0 x40
0 1 1 0  0 0 0 0 x60
1 0 0 0  0 0 0 0 x80
*/

void Hoermann::send_command(void)
{
  output_buffer[0] = 0x55;
  output_buffer[1] = 0x01;
  output_buffer[2] = 0x01;
  output_buffer[3] = (uint8_t)actual_action;
  output_buffer[4] = output_buffer[0] + output_buffer[1] + output_buffer[2] + output_buffer[3];
  for (int i =0 ; i<16; i++){
      output_buffer_[i] = (char)output_buffer[i];
  }
  
  serial.serial_send(&output_buffer_[0], 5);
}

uint8_t Hoermann::calc_checksum(uint8_t *p_data, uint8_t length)
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
