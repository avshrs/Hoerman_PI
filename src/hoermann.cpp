#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <algorithm>    // std::fill

void Hoermann_pi::init(const char* serial_name, int boudrate, uint8_t lead_zero)
{
    serial.serial_open(serial_name, boudrate, lead_zero);
}


void Hoermann_pi::run_loop(void)
{   
    auto start = timer.now();
  
    while (1)
    {   
        RX_Buffer* rx_buf;
        TX_Buffer* tx_buf;

        rx_buf = new RX_Buffer;
        tx_buf = new TX_Buffer;
        
        serial.serial_read(rx_buf);
        start = timer.now();
        if(is_frame_corect(rx_buf))
        {     
            print_buffer(rx_buf->buf.data(),rx_buf->buf.size());

            if(is_broadcast(rx_buf))
            {
                if(is_broadcast_lengh_correct(rx_buf))
                    {
                        update_broadcast_status(rx_buf);
                    }
            }
            else if(is_slave_query(rx_buf))
            {   
                if(is_slave_scan(rx_buf))
                {
                    make_scan_responce_msg(rx_buf, tx_buf);
                }    
                else if(is_slave_status_req(rx_buf))
                {
                    make_status_req_msg(rx_buf, tx_buf);
                    while(1)
                    {
                        auto check = timer.now();
                        auto deltaTime = std::chrono::duration_cast<mi>(check - start).count();
                        if( deltaTime > (tx_buf->timeout) && deltaTime < max_frame_delay)
                        {   
                            std::cout << "--------------\n";
                            print_buffer(rx_buf->buf.data(),rx_buf->buf.size());
                            print_buffer(tx_buf->buf.data(),tx_buf->buf.size());
                            // std::cout << "--------------\n\n";
                            serial.serial_send(tx_buf);
                            auto check2 = timer.now();
                            auto deltaTime2 = std::chrono::duration_cast<mi>(check2 - start).count();
                            
                            std::cout << "-------"<<deltaTime2 <<"-------\n";
                            break;
                        }
                        usleep(10);
                    }
                }
            }
        }
        delete rx_buf;
        delete tx_buf;
    } 
}       


uint8_t Hoermann_pi::get_length(RX_Buffer* buf)
{   
    if(buf->buf.size() > 2)
    {
        return buf->buf.at(1) & 0x0F;
    }
    else
        return 0x00;
}

uint8_t Hoermann_pi::get_counter(RX_Buffer* buf)
{
    if(buf->buf.size() > 2)
    {
        return (buf->buf.at(1) & 0xF0) + 0x10;
    }
    else
        return 0x00;

}

bool Hoermann_pi::is_broadcast(RX_Buffer* buf)
{   
    if(buf->buf.size() == 5)
    {
        if(buf->buf.at(0) == BROADCAST_ADDR && buf->buf.at(0) == 0x12 && calc_crc8(buf->buf.data(), 4) == buf->buf.at(4))
        {   
            return true;
        }
        else
            return false;
    }
    else
    {
        return false;
    }
}

bool Hoermann_pi::is_slave_query(RX_Buffer* buf)
{   
    if(buf->buf.size() > 3 && buf->buf.size() < 6 )
    {
        if(buf->buf.at(0) == UAP1_ADDR)
            return true;
        else
            return false;
    }
    else
    {
       return false; 
    }
}
bool Hoermann_pi::is_frame_corect(RX_Buffer* buf)
{   
    if(buf->buf.size() > 3 && buf->buf.size() < 6)
    {
        if(calc_crc8(buf->buf.data(), buf->buf.size()-1) == buf->buf.at(buf->buf.size()-1) )
            return true; 
        else 
            return false;
    }
    else
    {
       return false; 
    }
}

bool Hoermann_pi::is_slave_scan(RX_Buffer* buf)
{
    if(buf->buf.size() == 5)
    {
        if(is_broadcast_lengh_correct(buf) && (buf->buf.at(2) == CMD_SLAVE_SCAN))
            return true;
        else
            return false;
    }
    else
    {
        return false;    
    }
}

bool Hoermann_pi::is_slave_status_req(RX_Buffer* buf)
{
    if(buf->buf.size() == 4)
    {    
    if(is_req_lengh_correct(buf) && (buf->buf.at(2) == CMD_SLAVE_STATUS_REQUEST))
        return true;
    else
        return false;
    }
    else
    {
        return false;
    }
}

bool Hoermann_pi::is_broadcast_lengh_correct(RX_Buffer *buf)
{
    if(get_length(buf) == broadcast_lengh)
        return true;
    else
        return false;
}

bool Hoermann_pi::is_req_lengh_correct(RX_Buffer *buf)
{
    if(get_length(buf) == reguest_lengh)
        return true;
    else
        return false;
}

void Hoermann_pi::update_broadcast_status(RX_Buffer *buf)
{
  broadcast_status = buf->buf.at(2);
  broadcast_status |= (uint16_t)buf->buf.at(3) << 8;
}

void Hoermann_pi::print_buffer(uint8_t *buf, int len)
{   
    std::cout << "Len: "<< std::dec <<len << "|";
    for(int i = 0; i < len  ; i++)
        {
        std::cout << " 0x" << std::setw(2);
        std::cout << std::setfill('0') << std::hex;
        std::cout << static_cast<int>(buf[i]);
        }
    std::cout<<" | "<<std::endl;
}

uint8_t Hoermann_pi::get_master_address()
{
    return master_address;
}

void Hoermann_pi::make_scan_responce_msg(RX_Buffer* rx_buf, TX_Buffer* tx_buf)
{
    tx_buf->buf.push_back(get_master_address());
    tx_buf->buf.push_back(0x02 | get_counter(rx_buf));
    tx_buf->buf.push_back(UAP1_TYPE);
    tx_buf->buf.push_back(UAP1_ADDR);
    tx_buf->buf.push_back(calc_crc8(tx_buf->buf.data(), 4));
    tx_buf->timeout = 3000;
}

void Hoermann_pi::make_status_req_msg(RX_Buffer* rx_buf, TX_Buffer* tx_buf)
{
    tx_buf->buf.push_back(get_master_address());
    tx_buf->buf.push_back(0x03 | get_counter(rx_buf));
    tx_buf->buf.push_back(CMD_SLAVE_STATUS_RESPONSE);
    tx_buf->buf.push_back(static_cast<uint8_t>(slave_respone_data));
    tx_buf->buf.push_back(0x10);
    slave_respone_data = RESPONSE_DEFAULT;
    tx_buf->buf.push_back(calc_crc8(tx_buf->buf.data(), 5));
    tx_buf->timeout = 1000;
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
