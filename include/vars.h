#pragma once
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <string>
#include <vector>

struct TX_Buffer{
    
    std::vector<uint8_t> buf;
    int timeout = 0;
    TX_Buffer(){buf.reserve(16);}

};
struct RX_Buffer{
    RX_Buffer(){buf.reserve(16);}
    std::vector<uint8_t> buf;
};

struct Hoermann_cfg{
    std::string serial_file;
    int boudrate;
    int remove_lead_zero;
    
    std::string get_open;
    std::string get_stopped;
    std::string get_closed;
    std::string get_opening;
    std::string get_closing;
    std::string get_venting;
    std::string get_error;
    std::string get_offline;
    std::string set_open;
    std::string set_close;
    std::string set_stop;
    std::string set_venting;
    std::string toggle_Light;
};
struct Mqtt_config{
    std::string ClientId;
    std::string ServerIp;
    std::string username;
    std::string password;
    int serverPort;
    std::string keepAliveTopic;
    std::string keepAliveMsg;
    std::string Substring;
    std::string Pubstring;


};
