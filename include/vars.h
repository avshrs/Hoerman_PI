#pragma once
#include <cstdio>
#include <unistd.h>
#include <chrono>
#include <vector>

struct TX_Buffer{
    std::vector<uint8_t> buf;
    int timeout = 0;
};
struct RX_Buffer{
    std::vector<uint8_t> buf;
};

struct Hoermann_cfg{
    std::string serial_file;
    int boudrate;
    int remove_lead_zero;
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

    std::string brama_get_open;
    std::string brama_get_closed;
    std::string brama_get_opening;
    std::string brama_get_closing;
    std::string brama_get_venting;
    std::string brama_get_error;
    std::string brama_get_offline;
    std::string brama_get_open;
    std::string brama_set_close;
    std::string brama_set_stop;
    std::string brama_set_venting;
    std::string brama_toggle_Light;
};
