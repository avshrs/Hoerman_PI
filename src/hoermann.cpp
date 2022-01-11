// based on https://github.com/stephan192/hoermann_door

#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>

#define SYNC_BYTE   0x55

Hoermann::Hoermann(void)
{
  actual_state = hoermann_state_unkown;
  actual_state_string = "unkown";
  actual_action = hoermann_action_none;
}
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

void Hoermann::open_serial(char * serial_name, int boudrate)
{
  serial.serial_open2(serial_name, boudrate, false, NULL);
  //serial.serial_open(serial_name, boudrate);
}


bool Hoermann::read_rs232(void)
{
  static uint8_t counter = 0;
  static uint8_t len = 0;
  uint8_t data;

  while (1)
  {
    // read the incoming byte:

    serial.serial_read(buf, 1);
    
    data = buf[0];

    if ((data == SYNC_BYTE) && (counter == 0))
    {
      

      
      
      serial.serial_read(buf, 5);
      for (int i = 0 ; i < 5 ;i++){
        rx_buffer[i] = buf[i];
      }
      if (calc_checksum(rx_buffer, 5) == rx_buffer[4])
        {
          counter = 0;
          return true;
        }
        counter = 0;
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
        actual_state_string = "Partially Open";
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

void Hoermann::send_command()
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
