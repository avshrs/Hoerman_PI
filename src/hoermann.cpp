// based on https://github.com/stephan192/hoermann_door

#include "hoermann.h"
#include <iostream>
#define SYNC_BYTE   0x55


void Hoermann::loop(void)
{
  if (read_rs232() == true)
  {
    parse_input();
  }

  if (actual_action != hoermann_action_none)
  {
    send_command();
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

void Hoermann::open_serial(char * serial_name, int boudrate){

  rs232 = serial.serial_open(serial_name, boudrate);
}


bool Hoermann::read_rs232(void)
{
  static uint8_t counter = 0;
  static uint8_t len = 0;
  while(1){
    // read the incoming byte:
    char buf[2]; 
    serial.serial_read_byte(buf, 1);
    // std::cout << std::dec << buf << std::endl;
    

    uint8_t data = (uint8_t)buf[0];
    
    if ((data == SYNC_BYTE) && (counter == 0))
    {
      rx_buffer[counter] = data;
      counter++;
      len = 0;
    }
    else if (counter > 0)
    {
      rx_buffer[counter] = data;
      counter++;
      if (counter == 3)
      {
        if (data < 16)
        {
          len = data + 4; //3 = SYNC + CMD + LEN + CHK, limit to 15 data bytes
        }
        else
        {
          counter = 0;
        }
      }
      else if (counter == len)
      {
        if (calc_checksum(rx_buffer, len - 1) == data)
        {
          counter = 0;
          return true;
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
      else if ((rx_buffer[3] & 0x02) == 0x02)
      {
        actual_state = hoermann_state_closed;
        actual_state_string = "closed";
      }
      else if ((rx_buffer[3] & 0x80) == 0x80)
      {
        actual_state = hoermann_state_venting;
        actual_state_string = "venting";
      }
      else if ((rx_buffer[3] & 0x60) == 0x40)
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
