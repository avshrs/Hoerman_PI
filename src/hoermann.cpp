
#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include "USB_serial.h"


/* CRC table for polynomial 0x07 */
static const uint8_t crctable[256] = {
  0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
  0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
  0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
  0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
  0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
  0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
  0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
  0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
  0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
  0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
  0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
  0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
  0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
  0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
  0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
  0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

void Hoermann_pi::init(char* serial_name, int boudrate)
{
    serial.serial_open(serial_name, boudrate);
}


void Hoermann_pi::run_loop(void)
{   int count = 3000;
    while (1)
    {
        serial.serial_read(rx_buffer, 5);

        parse_message();

        if(tx_message_ready)
        {
            usleep(count);
            uint8_t buf2[2] = {0x00};
            serial.serial_send(buf2, 1);
            serial.serial_send(tx_buffer, tx_length);
            tx_message_ready = false;
            
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
  if(rx_buffer[0] == UAP1_ADDR)
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



// uint8_t Hoermann_pi::calc_crc8(uint8_t *p_data, uint8_t len)
// {
// size_t i;
// uint8_t crc = CRC8_INITIAL_VALUE;
//     while(len--){
//         crc ^= *p_data++;
//         for(i = 0; i < 8; i++){
//             if(crc & 0x80){
//                 crc <<= 1;
//                 crc ^= 0x07;
//             } else {
//                 crc <<= 1;
//             }
//         }
//     }
//     //std::cout << " 0x"<<std::setw(2) << std::setfill('0')<<std::hex << static_cast<int>(crc);
//     return(crc);
// }

uint8_t Hoermann_pi::calc_crc8(uint8_t *p_data, uint8_t len)
{
  uint8_t i;
  uint8_t data;
  uint8_t crc = CRC8_INITIAL_VALUE;
  
  for(i = 0; i < len; i++)
  {
    /* XOR-in next input byte */
    data = *p_data ^ crc;
    p_data++;
    /* get current CRC value = remainder */
    crc = crctable[data];
  }
  
  return crc;
}