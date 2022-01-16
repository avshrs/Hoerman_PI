#pragma once
#include "vars.h"
#include <vector>

class Config_manager{
    public:

    Mqtt_config mqtt_config;
    Hoermann_cfg h_config;

    void read_config();
    
    std::string get_mqtt_ClientId();
    std::string get_mqtt_ip();
    int get_mqtt_port();
    std::string get_mqtt_password();
    std::string get_mqtt_username();
    std::string get_mqtt_keepAliveTopic();
    std::string get_mqtt_keepAliveMsg();
    std::string get_mqtt_Substring();
    std::string get_mqtt_Pubstring();

    std::string get_hoer_serial_file();
    int get_hoer_boudrate();
    int get_hoer_lead_zeros();

    std::string brama_get_open();
    std::string brama_get_closed();
    std::string brama_get_opening();
    std::string brama_get_closing();
    std::string brama_get_venting();
    std::string brama_get_error();
    std::string brama_get_offline();
    std::string brama_get_open();
    std::string brama_set_close();
    std::string brama_set_stop();
    std::string brama_set_venting();
    std::string brama_toggle_Light();
};
