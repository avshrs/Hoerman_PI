#pragma once

#include <mosquittopp.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include "Config_manager.h"
#include "hoermann.h"

// #define DEBUG

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class Mqtt_Client : public mosqpp::mosquittopp
{
public:
    Mqtt_Client(const char *id, const char *host, int port, const char *username, const char *password);

    Hoermann_pi *hoerpi;
    Config_manager *cfg;
    std::string date();
    void pub_door_state(std::string msg);
    void pub_in_state(int out, bool sate);
    void register_horman(Hoermann_pi *hoerpi_);
    void register_config(Config_manager* cfg_);
    void client_loop_forever();
    void register_subs();
    void unregister_subs();
    void on_connect(int rc);
    void on_error();
    void on_unsubscribe(int mid);
    void on_disconnect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);
private:
    std::vector<std::string> parse_string(std::string str, char delimiter);

};
