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

    std::string get_string_open();
    std::string get_string_closed();
    std::string get_string_opening();
    std::string get_string_closing();
    std::string get_string_venting();
    std::string get_string_error();
    std::string get_string_offline();
    std::string brama_set_open();
    std::string brama_set_close();
    std::string brama_set_stop();
    std::string brama_set_venting();
    std::string brama_toggle_Light();
};
