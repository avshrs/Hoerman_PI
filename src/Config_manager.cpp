#include "Config_manager.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <algorithm>

void Config_manager::read_config(){
    YAML::Node config = YAML::LoadFile("config.yaml");
    std::cout << "outputs settings loading" << std::endl;

  
    
    std::cout << "Mqtt settings loading" << std::endl;
    mqtt_config.ClientId = config["mqtt"]["ClientId"].as<std::string>();
    mqtt_config.ServerIp = config["mqtt"]["ServerIp"].as<std::string>();
    mqtt_config.username = config["mqtt"]["username"].as<std::string>();
    mqtt_config.password = config["mqtt"]["password"].as<std::string>();
    mqtt_config.serverPort = config["mqtt"]["serverPort"].as<int>();
    
    mqtt_config.keepAliveTopic = config["mqtt"]["keepAliveTopic"].as<std::string>();
    mqtt_config.keepAliveMsg = config["mqtt"]["keepAliveMsg"].as<std::string>();
    
    mqtt_config.Substring = config["mqtt"]["Substring"].as<std::string>();
    mqtt_config.Pubstring = config["mqtt"]["Pubstring"].as<std::string>();

    mqtt_config.brama_get_open = config["mqtt"]["brama_get_open"].as<std::string>();
    mqtt_config.brama_get_closed = config["mqtt"]["brama_get_closed"].as<std::string>();
    mqtt_config.brama_get_opening = config["mqtt"]["brama_get_opening"].as<std::string>();
    mqtt_config.brama_get_closing = config["mqtt"]["brama_get_closing"].as<std::string>();
    mqtt_config.brama_get_venting = config["mqtt"]["brama_get_venting"].as<std::string>();
    mqtt_config.brama_get_error = config["mqtt"]["brama_get_error"].as<std::string>();
    mqtt_config.brama_get_offline = config["mqtt"]["brama_get_offline"].as<std::string>();
    mqtt_config.brama_set_open = config["mqtt"]["brama_set_open"].as<std::string>();
    mqtt_config.brama_set_close = config["mqtt"]["brama_set_close"].as<std::string>();
    mqtt_config.brama_set_stop = config["mqtt"]["brama_set_stop"].as<std::string>();
    mqtt_config.brama_set_venting = config["mqtt"]["brama_set_venting"].as<std::string>();
    mqtt_config.brama_set_Light = config["mqtt"]["brama_set_Light"].as<std::string>();

    std::cout << "Mqtt settings loaded" << std::endl;

    h_config.serial_file = config["hoermann"]["serial_file"].as<std::string>();
    h_config.boudrate = config["hoermann"]["boudrate"].as<int>();
    h_config.remove_lead_zero = config["hoermann"]["remove_lead_zero"].as<int>();

}
    

std::string Config_manager::get_mqtt_ClientId(){
    return  mqtt_config.ClientId;
}
std::string Config_manager::get_mqtt_ip(){
    return  mqtt_config.ServerIp;
}
int Config_manager::get_mqtt_port(){
    return  mqtt_config.serverPort;
}
std::string Config_manager::get_mqtt_password(){
    return  mqtt_config.password;
}
std::string Config_manager::get_mqtt_username(){
    return  mqtt_config.username;
}
std::string Config_manager::get_mqtt_keepAliveTopic(){
    return  mqtt_config.keepAliveTopic;
}
std::string Config_manager::get_mqtt_keepAliveMsg(){
    return  mqtt_config.keepAliveMsg;
}
std::string Config_manager::get_mqtt_Substring(){
    return  mqtt_config.Substring;
}
std::string Config_manager::get_mqtt_Pubstring(){
    return  mqtt_config.Pubstring;
}

std::string Config_manager::get_hoer_serial_file()
{
    return  h_config.serial_file;
}
int Config_manager::get_hoer_boudrate()
{
    return  h_config.boudrate;
}
int Config_manager::get_hoer_lead_zeros()
{
    return  h_config.remove_lead_zero;
}


std::string Config_manager::brama_get_open(){
    return  mqtt_config.brama_get_open;
}
std::string Config_manager::brama_get_closed(){
    return  mqtt_config.brama_get_closed;
}
std::string Config_manager::brama_get_opening(){
    return  mqtt_config.brama_get_opening;
}
std::string Config_manager::brama_get_closing(){
    return  mqtt_config.brama_get_closing;
}
std::string Config_manager::brama_get_venting(){
    return  mqtt_config.brama_get_venting;
}
std::string Config_manager::brama_get_error(){
    return  mqtt_config.brama_get_error;
}
std::string Config_manager::brama_get_offline(){
    return  mqtt_config.brama_get_offline;
}
std::string Config_manager::brama_get_open(){
    return  mqtt_config.brama_get_open;
}
std::string Config_manager::brama_set_close(){
    return  mqtt_config.brama_set_close;
}
std::string Config_manager::brama_set_stop(){
    return  mqtt_config.brama_set_stop;
}
std::string Config_manager::brama_set_venting(){
    return  mqtt_config.brama_set_venting;
}
std::string Config_manager::brama_toggle_Light(){
    return  mqtt_config.brama_toggle_Light;
}
