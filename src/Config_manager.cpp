#include "Config_manager.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>

std::string Config_manager::date(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);      
    std::stringstream ss; 
    ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ") ;
    return ss.str();
}

void Config_manager::read_config(){
    YAML::Node config = YAML::LoadFile("config.yaml");
    

  
    
    std::cout << date() << "Loading Settings" << std::endl;
    mqtt_config.ClientId = config["mqtt"]["ClientId"].as<std::string>();
    mqtt_config.ServerIp = config["mqtt"]["ServerIp"].as<std::string>();
    mqtt_config.username = config["mqtt"]["username"].as<std::string>();
    mqtt_config.password = config["mqtt"]["password"].as<std::string>();
    mqtt_config.serverPort = config["mqtt"]["serverPort"].as<int>();
    
    mqtt_config.keepAliveTopic = config["mqtt"]["keepAliveTopic"].as<std::string>();
    mqtt_config.keepAliveMsg = config["mqtt"]["keepAliveMsg"].as<std::string>();
    
    mqtt_config.Substring = config["mqtt"]["Substring"].as<std::string>();
    mqtt_config.Pubstring = config["mqtt"]["Pubstring"].as<std::string>();

    h_config.get_open = config["mqtt"]["brama_get_open"].as<std::string>();
    h_config.get_stopped = config["mqtt"]["brama_get_stopped"].as<std::string>();
    h_config.get_closed = config["mqtt"]["brama_get_closed"].as<std::string>();
    h_config.get_opening = config["mqtt"]["brama_get_opening"].as<std::string>();
    h_config.get_closing = config["mqtt"]["brama_get_closing"].as<std::string>();
    h_config.get_venting = config["mqtt"]["brama_get_venting"].as<std::string>();
    h_config.get_error = config["mqtt"]["brama_get_error"].as<std::string>();
    h_config.get_offline = config["mqtt"]["brama_get_offline"].as<std::string>();
    h_config.set_open = config["mqtt"]["brama_set_open"].as<std::string>();
    h_config.set_close = config["mqtt"]["brama_set_close"].as<std::string>();
    h_config.set_stop = config["mqtt"]["brama_set_stop"].as<std::string>();
    h_config.set_venting = config["mqtt"]["brama_set_venting"].as<std::string>();
    h_config.toggle_Light = config["mqtt"]["brama_toggle_Light"].as<std::string>();
    h_config.serial_file = config["hoermann"]["serial_file"].as<std::string>();
    h_config.boudrate = config["hoermann"]["boudrate"].as<int>();
    h_config.remove_lead_zero = config["hoermann"]["remove_lead_zero"].as<int>();
    std::cout << date() << "Settings Loaded" << std::endl;
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


std::string Config_manager::get_open_string(){
    return  h_config.get_open;
}
std::string Config_manager::get_stopped_string(){
    return  h_config.get_stopped;
}
std::string Config_manager::get_closed_string(){
    return  h_config.get_closed;
}
std::string Config_manager::get_opening_string(){
    return  h_config.get_opening;
}
std::string Config_manager::get_closing_string(){
    return  h_config.get_closing;
}
std::string Config_manager::get_venting_string(){
    return  h_config.get_venting;
}
std::string Config_manager::get_error_string(){
    return  h_config.get_error;
}
std::string Config_manager::get_offline_string(){
    return  h_config.get_offline;
}
std::string Config_manager::set_open_string(){
    return  h_config.set_open;
}
std::string Config_manager::set_close_string(){
    return  h_config.set_close;
}
std::string Config_manager::set_stop_string(){
    return  h_config.set_stop;
}
std::string Config_manager::set_venting_string(){
    return  h_config.set_venting;
}
std::string Config_manager::toggle_Light_string(){
    return  h_config.toggle_Light;
}
