#pragma once

#include <mosquittopp.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <vector>
#include "H_config.h"

// #define DEBUG

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class mqtt_client : public mosqpp::mosquittopp
{
public:
    mqtt_client(const char *id, const char *host, int port, const char *username, const char *password);

    H_config *h_cfg;
    void publish_state(std::string state);
    void register_config(H_config *h_cfg_);
    void client_loop_forever();
    void register_subs();
    void unregister_subs();
    void on_connect(int rc);
    void on_error();
    void on_disconnect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_subscribe(int mid, int qos_count, const int *granted_qos);
private:
    std::vector<std::string> parse_string(std::string str, char delimiter);

};
