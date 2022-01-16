#include "Mosquitto.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

mqtt_client::mqtt_client(const char *id, const char *host, int port, const char *username, const char *password) : mosquittopp(id)
{
    int keepalive = 60;
    
    username_pw_set(username, password);
    connect(host, port, keepalive);
}

void mqtt_client::client_loop_forever(){
    
        reconnect_delay_set(5, 1000000, true);
        loop_forever();
}

void mqtt_client::register_subs()
{
    std::string sub = cfg->get_mqtt_Substring();

    subscribe(NULL, sub.c_str());
}

void mqtt_client::unregister_subs()
{

    std::string sub = cfg->get_mqtt_Substring();
    unsubscribe(NULL, sub.c_str());
}

void mqtt_client::on_error() {
    std::cout<<"onerror"<<std::endl;
    return;
    }


void mqtt_client::on_connect(int rc)
{
    if (!rc)
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);      
        std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ");
        std::cout << "Connected with code " << rc << std::endl;
        unregister_subs();
        register_subs();
    }
    
}

void mqtt_client::on_disconnect(int rc){
    if (!rc){
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);      
        std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ");
        std::cout << "disconnected - code " << rc << std::endl;
    
    }
}
void mqtt_client::on_subscribe(int mid, int qos_count, const int *granted_qos){
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);      
        std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ");
        std::cout << "Subscription succeeded. " << " mid: " << mid << " qos_count: "<< qos_count << " qos_granted: "<< granted_qos << std::endl;
}


void mqtt_client::register_mcp_config(Config_manager *cfg_){
    cfg = cfg_;
}

void mqtt_client::pub_door_state(std::string msg){
    std::string pub = cfg->get_mqtt_Pubstring();
    publish(NULL, pub.c_str(), msg.length(), msg.c_str());
}


void mqtt_client::on_message(const struct mosquitto_message *message){
    try{
        std::string message_topic(message->topic);
        std::string message_payload(static_cast<char*>(message->payload));
        const char * substring = cfg->get_mqtt_Substring().c_str();

        if(!message_payload.empty() && message_topic.find(substring) != std::string::npos){
            if(message_payload == cfg->brama_set_open_string())
            {
                hoerpi->door_open();
            }
            if(message_payload == cfg->brama_set_close_string())
            {
                hoerpi->door_close();
            }
            if(message_payload == cfg->brama_set_stop_string())
            {
                hoerpi->door_stop();
            }
            if(message_payload == cfg->brama_set_venting_string())
            {
                hoerpi->door_venting();
            }
            if(message_payload == cfg->brama_toggle_Light_string())
            {
                hoerpi->door_toggle_light();
            }
            if(message_payload == "LOCK")
            {
                hoerpi->door_lock();
            }
        }
    }
    catch(...){std::cout << "Received Empty Payload" << std:: endl;}
}

