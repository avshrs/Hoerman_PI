// based on https://github.com/stephan192/hoermann_door

#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>



void Hoermann_pi::open_serial(char * serial_name, int boudrate)
{
    serial.serial_open2(serial_name, boudrate, false, NULL);
}


void Hoermann_pi::start_frame_listener(){
    while (1)
    {
        serial.serial_read(in_bufer, 6);
        if(in_bufer[0] == deviceID && in_bufer[3]== masterID)
        {
            //0x28 0x02 0x01 0x80 0x0D - master requet frame
            //0x28 0x82 0x01 0x80 0x0D - master requet frame ? also?
            //------------------------------------------------------
            //0x80 0x12 0x14 0x28 0xA7 - sleve responce frame
            //0x14 - device type?

            uint8_t frame[4] = {masterID, 0x12, 0x14, deviceID};
            send_command(frame, 4);
        }
        else if (in_bufer[0] == broadcastID &&  in_bufer[2]== broadcaststatusID)
        {
            // Broadcast 
        }

    }

}

std::string Hoermann_pi::parse_state(uint8_t* rx_buffer)
{
  if (rx_buffer[0] == 0x00)
  {
    if (rx_buffer[1] == 0x12)
    {
      if ((rx_buffer[2] & 0x01) == 0x01)
      {
        return states[1];
      }
      else if ((rx_buffer[2] & 0x02) == 0x02)
      {
        return states[2];
      }
      else if ((rx_buffer[2] & 0x80) == 0x80)
      {
        return states[3];
      }
      else if ((rx_buffer[2] & 0x60) == 0x40)
      {
        return  states[4];
      }
      else if ((rx_buffer[2] & 0x60) == 0x60)
      {
        return  states[5];
      }
      else if ((rx_buffer[2] & 0x10) == 0x10)
      {
        return states[6];
      }
      else if (rx_buffer[2] == 0x00)
      {
        return states[0];
      }
      else 
        return states[7];
    }
  }
}

void Hoermann_pi::send_command()
{
  output_buffer[0] = 0x55;
  output_buffer[1] = 0x01;
  output_buffer[2] = 0x01;
  output_buffer[3] = (uint8_t)actual_action;
  output_buffer[4] = calc_checksum(output_buffer, 4);
  for (int i =0 ; i<16; i++){
      output_buffer_[i] = (char)output_buffer[i];
  }
  serial.serial_send(&output_buffer_[0], 5);
}


uint8_t Hoermann_pi::calc_checksum(uint8_t *p_data, uint8_t len)
{
size_t i;
uint8_t crc = 0xF3;
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
    return(crc);
}

