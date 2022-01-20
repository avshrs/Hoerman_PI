#include "Mosquitto.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


std::string Mqtt_Client::date(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);      
    std::stringstream ss; 
    ss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S | ") ;
    return ss.str();
}


Mqtt_Client::Mqtt_Client(const char *id, const char *host, int port, const char *username, const char *password) : mosquittopp(id)
{
    int keepalive = 60;
    
    username_pw_set(username, password);
    connect(host, port, keepalive);
}

void Mqtt_Client::client_loop_forever(){
    
        reconnect_delay_set(5, 1000000, true);
        loop_forever();
}

void Mqtt_Client::register_subs()
{
    std::cout<< date() <<"register_subs1"<<std::endl;
    std::string sub = cfg->get_mqtt_Substring();
    std::cout<< date() <<"register_subs2"<<std::endl;
    subscribe(NULL, sub.c_str());
}

void Mqtt_Client::unregister_subs()
{
    std::cout<< date() <<"unregister_subs1"<<std::endl;
    std::string sub = cfg->get_mqtt_Substring();
    std::cout<< date() <<"unregister_subs2"<<std::endl;
    unsubscribe(NULL, sub.c_str());
}

void Mqtt_Client::on_error() {
    std::cout<< date() <<"onerror"<<std::endl;
    return;
    }


void Mqtt_Client::on_connect(int rc)
{
    if (!rc)
    {
        std::cout << date() << "Connected with code " << rc << std::endl;
        // unregister_subs();
        register_subs();
    }
    
}


void Mqtt_Client::on_disconnect(int rc){
    if (!rc){
        std::cout << date() << "disconnected - code " << rc << std::endl;
    }
}

void Mqtt_Client::on_unsubscribe(int mid){
        std::cout<< date()  << "Subscription succeeded. " << " mid: " << mid <<  std::endl;
}


void Mqtt_Client::on_subscribe(int mid, int qos_count, const int *granted_qos){

        std::cout<< date()  << "Subscription succeeded. " << " mid: " << mid << " qos_count: "<< qos_count << " qos_granted: "<< granted_qos << std::endl;
}


void Mqtt_Client::register_config(Config_manager *cfg_){
    cfg = cfg_;
}
 
void Mqtt_Client::register_horman(Hoermann_pi *hoerpi_){
    hoerpi = hoerpi_;
}

void Mqtt_Client::pub_door_state(std::string msg){
    std::string pub = cfg->get_mqtt_Pubstring();
    publish(NULL, pub.c_str(), msg.length(), msg.c_str());
}


void Mqtt_Client::on_message(const struct mosquitto_message *message){

    try{
        std::string message_topic(message->topic);
        std::string message_payload(static_cast<char*>(message->payload));
        std::string substring = cfg->get_mqtt_Substring();
        std::cout << date() <<"get instruction: " << message_payload << std::endl;
        if(!message_payload.empty() && message_topic == substring){
            if(message_payload == cfg->set_open_string())
            {
                std::cout << date() <<"door_open: " << message_payload << std::endl;
                hoerpi->door_open();
            }
            else if(message_payload == cfg->set_close_string())
            {
                std::cout << date() <<"door_close: " << message_payload << std::endl;
                hoerpi->door_close();
            }
            else if(message_payload == cfg->set_stop_string())
            {   
                std::cout << date() <<"door_stop: " << message_payload << std::endl;
                hoerpi->door_stop();
            }
            else if(message_payload == cfg->set_venting_string())
            {   
                std::cout << date() <<"door_venting: " << message_payload << std::endl;
                hoerpi->door_venting();
            }
            else if(message_payload == cfg->toggle_Light_string())
            {
                std::cout << date() <<"door_toggle_light: " << message_payload << std::endl;
                hoerpi->door_toggle_light();
            }
            else    
                std::cout<< date() << "Bad payload message" << std::endl;
          }
    }
    catch(...){std::cout << date() << "Received Empty Payload" << std:: endl;}
}

