#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>


void Hoermann_pi::init(const char* serial_name, int boudrate)
{
    serial.serial_open(serial_name, boudrate);
}


void Hoermann_pi::run_loop(void)
{   
    auto check = timer.now();
    auto start = timer.now();
    RX_Buffer* rx_buf;
    TX_Buffer* tx_buf;
    rx_buf = new RX_Buffer;
    tx_buf = new TX_Buffer;
    for(int i = 0 ; i < 18 ; i++)
    {
    rx_buf->buf.push_back(0);
    tx_buf->buf.push_back(0);
    }       
    while (1)
    {   
        for(int i = 0 ; i < 18 ; i++)
        {
            rx_buf->buf[i] = 0x00;
            tx_buf->buf[i] = 0x00;
        }
        
        
        serial.serial_read(rx_buf->buf.data(), 7);
       
        start = timer.now();
     
        if(is_broadcast(rx_buf))
        {
            print_buffer(rx_buf->buf.data(),7);
           if(is_broadcast_lengh_correct(rx_buf))
                {
                update_broadcast_status(rx_buf);
                }
            
        }
        else if(is_slave_query(rx_buf))
        {   
            serial.clear_buffer();
            if(is_slave_scan(rx_buf))
            {
                make_scan_responce_msg(rx_buf, tx_buf);
            }    
            else if(is_slave_status_req(rx_buf))
            {
                make_status_req_msg(rx_buf, tx_buf);
            }    
            while(1)
            {
                check = timer.now();
                auto deltaTime = std::chrono::duration_cast<mi>(check - start).count();

                if( deltaTime > (tx_buf->timeout))
                {   
                    print_buffer(rx_buf->buf.data(),7);
                    print_buffer(tx_buf->buf.data(),7);
                    std::cout << "time delta: " << deltaTime << "timeout: " << std::dec << tx_buf->timeout<< std::endl;
                    uint8_t empty[1] = {0};
                    serial.serial_send(empty, 1);
                    serial.serial_send(tx_buf->buf.data(), tx_buf->len);
                    break;
                }
                usleep(10);
            }
        }
 
    } 
}       


uint8_t Hoermann_pi::get_length(RX_Buffer* buf)
{
    return buf->buf[1] & 0x0F;
}

uint8_t Hoermann_pi::get_counter(RX_Buffer* buf)
{
    return (buf->buf[1] & 0xF0) + 0x10;
}

bool Hoermann_pi::is_broadcast(RX_Buffer* buf)
{
    if(buf->buf[0] == BROADCAST_ADDR && calc_crc8(buf->buf.data(), 4) == buf->buf[4])
    {   
        // if(master_address != buf->buf[3])
        // {std::cout << "broadcast" <<std::endl;
        //     master_address = buf->buf[3];
        // }
        return true;
    }
    else
        return false;
}

bool Hoermann_pi::is_slave_query(RX_Buffer* buf)
{   
    if(buf->buf[0] == UAP1_ADDR)
        return true;
    else
        return false;
}
bool Hoermann_pi::is_slave_scan(RX_Buffer* buf)
{
    if(is_broadcast_lengh_correct(buf) && (buf->buf[2] == CMD_SLAVE_SCAN))
        return true;
    else
        return false;
}


bool Hoermann_pi::is_slave_status_req(RX_Buffer* buf)
{
    if(is_req_lengh_correct(buf) && (buf->buf[2] == CMD_SLAVE_STATUS_REQUEST))
        return true;
    else
        return false;
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
  broadcast_status = buf->buf[2];
  broadcast_status |= (uint16_t)buf->buf[3] << 8;
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


uint8_t Hoermann_pi::get_master_address()
{
    return master_address;
}

void Hoermann_pi::make_scan_responce_msg(RX_Buffer* rx_buf, TX_Buffer* tx_buf)
{
    tx_buf->buf[0] = get_master_address();
    tx_buf->buf[1] = 0x02 | get_counter(rx_buf);
    tx_buf->buf[2] = UAP1_TYPE;
    tx_buf->buf[3] = UAP1_ADDR;
    tx_buf->buf[4] = calc_crc8(tx_buf->buf.data(), 4);
    tx_buf->len = 5;
    // tx_buf.received_time = buf->received_time;
    tx_buf->timeout = 3500;
}

void Hoermann_pi::make_status_req_msg(RX_Buffer* rx_buf, TX_Buffer* tx_buf)
{
    tx_buf->buf[0] = get_master_address();
    tx_buf->buf[1] = 0x03 | get_counter(rx_buf);
    tx_buf->buf[2] = CMD_SLAVE_STATUS_RESPONSE;
    tx_buf->buf[3] = static_cast<uint8_t>(slave_respone_data);
    tx_buf->buf[4] = static_cast<uint8_t>((slave_respone_data>>8));
    slave_respone_data = RESPONSE_DEFAULT;
    tx_buf->buf[5] = calc_crc8(tx_buf->buf.data(), 5);
    tx_buf->len = 6;
    // tx_buf.received_time = buf->received_time; 
    tx_buf->timeout = 3500;
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
