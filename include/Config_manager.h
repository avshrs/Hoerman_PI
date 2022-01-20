#pragma once
#include "vars.h"
#include <vector>

class Config_manager{
    private:
        Mqtt_config mqtt_config;
        Hoermann_cfg h_config;

    public:
        void read_config();
        std::string date();
        std::string get_mqtt_ClientId();
        std::string get_mqtt_ip();
        int         get_mqtt_port();
        std::string get_mqtt_password();
        std::string get_mqtt_username();
        std::string get_mqtt_keepAliveTopic();
        std::string get_mqtt_keepAliveMsg();
        std::string get_mqtt_Substring();
        std::string get_mqtt_Pubstring();

        std::string get_hoer_serial_file();
        int         get_hoer_boudrate();
        int         get_hoer_lead_zeros();
        
        // hoermann config
        std::string get_open_string();
        std::string get_stopped_string();
        std::string get_closed_string();
        std::string get_opening_string();
        std::string get_closing_string();
        std::string get_venting_string();
        std::string get_error_string();
        std::string get_offline_string();
        std::string set_open_string();
        std::string set_close_string();
        std::string set_stop_string();
        std::string set_venting_string();
        std::string toggle_Light_string();
};
