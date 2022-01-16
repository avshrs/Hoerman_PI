#include <stdint.h>
#include <stdbool.h>
#include "hoermann.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <algorithm>    // std::fill
#include "Mosquitto.h"
#include <thread>


void Hoermann_pi::init(const char* serial_name, int boudrate, uint8_t lead_zero)
{
    serial.serial_open(serial_name, boudrate, lead_zero);
}


void Hoermann_pi::run_loop(void)
{   
    auto start = timer.now();
    RX_Buffer* rx_buf;
    TX_Buffer* tx_buf;
    while (true)
    {   
        rx_buf = new RX_Buffer;
        tx_buf = new TX_Buffer;
        try
        {
            serial.serial_read(rx_buf);
        }
        catch (...)
        {
            std::cout<< "ERROR while parsing frame"<<std::endl;   
        }
        start = timer.now();
        try
        {
            if(is_frame_corect(rx_buf))
            {     
                // print_buffer(rx_buf->buf.data(),rx_buf->buf.size());

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
                        while(true)
                        {
                            
                            auto deltaTime = std::chrono::duration_cast<mi>(timer.now() - start).count();
                            if( deltaTime > (tx_buf->timeout) )
                            {   
                                if(deltaTime > max_frame_delay)
                                {
                                    std::cout << "SCAN RESPONCE Frame building to long "<<deltaTime <<"\n";
                                    break;
                                }
                                std::cout << "--------------\n";
                                print_buffer(rx_buf->buf.data(),rx_buf->buf.size());
                                print_buffer(tx_buf->buf.data(),tx_buf->buf.size());
                                
                                serial.serial_send(tx_buf);

                                auto deltaTime2 = std::chrono::duration_cast<mi>(timer.now() - start).count();
                                std::cout << "-------"<<deltaTime2 <<"-------\n";
                                break;
                                
                            }

                            usleep(10);
                        }                    
                        
                    }    
                    else if(is_slave_status_req(rx_buf))
                    {
                        make_status_req_msg(rx_buf, tx_buf);
                        while(true)
                        {
                            
                            auto deltaTime = std::chrono::duration_cast<mi>(timer.now() - start).count();
                            if( deltaTime > (tx_buf->timeout))
                            {   
                                if(deltaTime > max_frame_delay)
                                {
                                    std::cout << "STATUS RESPONCE Frame building to long "<<deltaTime <<"\n";
                                    break;
                                }
                                // std::cout << "--------------\n";
                                // print_buffer(rx_buf->buf.data(),rx_buf->buf.size());
                                // print_buffer(tx_buf->buf.data(),tx_buf->buf.size());
                                // std::cout << "--------------\n\n";
                                serial.serial_send(tx_buf);
                                // auto check2 = timer.now();
                                // auto deltaTime2 = std::chrono::duration_cast<mi>(check2 - start).count();
                                
                                // std::cout << "-------"<<deltaTime2 <<"-------\n";
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
        catch (...)
        {
            delete rx_buf;
            delete tx_buf;
            std::cout<< "ERROR while parsing frame"<<std::endl;
        }
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
  uint16_t broadcast_status_ = buf->buf.at(2);
  broadcast_status_ |= (uint16_t)buf->buf.at(3) << 8;
  if (broadcast_status_ != broadcast_status )
  {
    broadcast_status = broadcast_status_;
    
    std::thread t(pub_thread);
    t.detach();
    
  }
}
void Hoermann_pi::pub_thread()
{
    std::string state = get_state();
    mqtt->pub_door_state(msg);
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
    tx_buf->timeout = 2750;
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
    tx_buf->timeout = 2750;
}


std::string Hoermann_pi::get_state()
{
  if ((broadcast_status & 0x01) == 0x01)
  {
    return cfg->get_stopped_string();
  }
  else if ((broadcast_status & 0x02) == 0x02)
  {
    return cfg->get_open_string();
  }
  else if ((broadcast_status & 0x80) == 0x80)
  {
    return cfg->get_closed_string();
  }
  else if ((broadcast_status & 0x60) == 0x40)
  {
    return  cfg->get_venting_string();
  }
  else if ((broadcast_status & 0x60) == 0x60)
  {
    return  cfg->get_opening_string();
  }
  else if ((broadcast_status & 0x10) == 0x10)
  {
    return cfg->get_closing_string();
  }
  else if (broadcast_status == 0x00)
  {
    return cfg->get_error_string();
  }
  else 
    return cfg->get_offline_string();
 
}


void Hoermann_pi::set_state(std::string action)
{
    if(action == cfg->brama_set_stop_string())
    {
      std::cout<<"Executing RESPONSE_STOP"<<std::endl;
      slave_respone_data = RESPONSE_STOP;
    }
    else if(action == cfg->brama_set_open_string())
    {
      slave_respone_data = RESPONSE_OPEN;
      std::cout<<"Executing RESPONSE_OPEN"<<std::endl;
    }
    else if(action == cfg->brama_set_close_string())
    {
      std::cout<<"Executing RESPONSE_CLOSE"<<std::endl;
      slave_respone_data = RESPONSE_CLOSE;
    }
    else if(action == cfg->brama_set_venting_string())
    {
      std::cout<<"Executing RESPONSE_VENTING"<<std::endl;
      slave_respone_data = RESPONSE_VENTING;
    }
    else if(action == cfg->brama_toggle_Light_string())
    {
      std::cout<<"Executing RESPONSE_TOGGLE_LIGHT"<<std::endl;
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

void Hoermann_pi::door_open()
{
    set_state(cfg->brama_set_open_string());
}
void Hoermann_pi::door_close()
{
    set_state(cfg->brama_set_close_string());
}
void Hoermann_pi::door_venting()
{
    set_state(cfg->brama_set_venting_string());
}
void Hoermann_pi::door_toggle_light()
{
    set_state(cfg->brama_toggle_Light_string());
}
void Hoermann_pi::door_stop()
{
    // if (goes up press down )
    // if (goes down press up )
    set_state( cfg->brama_set_stop_string());
}
void Hoermann_pi::door_lock()
{
    set_state(cfg->brama_set_stop_string());
}

void Hoermann_pi::register_mqtt(Mqtt_Client *mqtt_){
    mqtt = mqtt_;
}

void Hoermann_pi::register_cfg(Config_manager *cfg_){
    cfg = cfg_;
}
