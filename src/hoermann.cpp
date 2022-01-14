#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>


void Hoermann_pi::init(char* serial_name, int boudrate)
{
    serial.serial_open2(serial_name, boudrate, false, NULL);
    // serial.serial_open(serial_name, boudrate);
}


void Hoermann_pi::run_loop(void)
{   
    auto check = timer.now();
    RX_Buffer rx_buf;
    TX_Buffer tx_buf;
    int timeout = 0;
    bool is_broadcast_ = false; 
    while (1)
    {   
        is_broadcast_ = false; 
        serial.serial_read(rx_buf.buf, 7);
        rx_buf.received_time = timer.now();
        if(is_broadcast(rx_buf.buf))
        {
            if(is_broadcast_lengh_correct(rx_buf.buf))
                {
                update_broadcast_status(rx_buf.buf);
                is_broadcast_ = true;
                }
        }
        else if(is_slave_query(rx_buf.buf))
        {
            // print_buffer(rx_buf.buf, 6);
            if(is_slave_scan(rx_buf.buf))
            {
                tx_buf = make_scan_responce_msg(rx_buf);
            }    
            else if(is_slave_status_req(rx_buf.buf))
            {
                tx_buf = make_status_req_msg(rx_buf);
            }    
        }
    
        if(!is_broadcast_){
            while(1)
            {
                check = timer.now();
                auto deltaTime = std::chrono::duration_cast<mi>(check- tx_buf.received_time).count();
                std::cout << deltaTime << std::endl;
                if( deltaTime > tx_buf.timeout)
                {   
                    std::cout << deltaTime << std::endl;
                    serial.serial_send(tx_buf.buf, tx_buf.len);
                    break;
                }
                usleep(10);
            }
        }
    } 
}       


uint8_t Hoermann_pi::get_length(uint8_t* buf)
{
    return buf[1] & 0x0F;
}

uint8_t Hoermann_pi::get_counter(uint8_t* buf)
{
    return (buf[1] & 0xF0) + 0x10;
}

bool Hoermann_pi::is_broadcast(uint8_t* buf)
{
    if(buf[0] == BROADCAST_ADDR)
        return true;
    else
        return false;
}

bool Hoermann_pi::is_slave_query(uint8_t* buf)
{   
    if(buf[0] == UAP1_ADDR)
        return true;
    else
        return false;
}
bool Hoermann_pi::is_slave_scan(uint8_t* buf)
{
    if(is_broadcast_lengh_correct(buf) && (buf[2] == CMD_SLAVE_SCAN))
        return true;
    else
        return false;
}


bool Hoermann_pi::is_slave_status_req(uint8_t* buf)
{
    if(is_req_lengh_correct(buf) && (buf[2] == CMD_SLAVE_STATUS_REQUEST))
        return true;
    else
        return false;
}

bool Hoermann_pi::is_broadcast_lengh_correct(uint8_t *buf)
{
    if(get_length(buf) == broadcast_lengh)
        return true;
    else
        return false;
}

bool Hoermann_pi::is_req_lengh_correct(uint8_t *buf)
{
    if(get_length(buf) == reguest_lengh)
        return true;
    else
        return false;
}

void Hoermann_pi::update_broadcast_status(uint8_t *buf)
{
  broadcast_status = buf[2];
  broadcast_status |= (uint16_t)buf[3] << 8;
}

void Hoermann_pi::print_buffer(uint8_t *buf, int len)
{
    for(int i = 0; i < len  ; i++)
        {
        std::cout << " 0x" << std::setw(2);
        std::cout << std::setfill('0') << std::hex;
        std::cout << static_cast<int>(buf[i]);
        }
    std::cout<<std::endl;
}

TX_Buffer Hoermann_pi::make_scan_responce_msg(RX_Buffer buf)
{
    TX_Buffer tx_buf;
    
    tx_buf.buf[0] = MASTER_ADDR;
    tx_buf.buf[1] = 0x02 | get_counter(buf.buf);
    tx_buf.buf[2] = UAP1_TYPE;
    tx_buf.buf[3] = UAP1_ADDR;
    tx_buf.buf[4] = calc_crc8(tx_buf.buf, 4);
    tx_buf.len = 5;
    tx_buf.received_time = buf.received_time;
    tx_buf.timeout = 46100;
    return tx_buf;
}

TX_Buffer Hoermann_pi::make_status_req_msg(RX_Buffer buf)
{
    TX_Buffer tx_buf;
    
    tx_buf.buf[0] = MASTER_ADDR;
    tx_buf.buf[1] = 0x03 | get_counter(buf.buf);
    tx_buf.buf[2] = CMD_SLAVE_STATUS_RESPONSE;
    tx_buf.buf[3] = (uint8_t)slave_respone_data;
    tx_buf.buf[4] = (uint8_t)(slave_respone_data>>8);
    slave_respone_data = RESPONSE_DEFAULT;
    tx_buf.buf[5] = calc_crc8( tx_buf.buf, 5 );
    tx_buf.len = 6;
    tx_buf.received_time = buf.received_time; 
    tx_buf.timeout = 21100;
    return tx_buf;
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
