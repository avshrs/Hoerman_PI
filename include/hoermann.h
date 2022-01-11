// based on https://github.com/stephan192/hoermann_door

#pragma once
#include <string>
#include "USB_serial.h"

class Mqtt{

};
class Hoermann_pi
{
  public:
    Hoermann_pi();
  private:
    Mqtt mqtt;
    USB_serial serial;
    const char masterID = 0x80;
    const char broadcastID = 0x00;
    const char broadcaststatusID = 0x01;
    const char deviceID = 0x28;
    const char syncID = 0x55;
    

    const std::string states[8] = {"stoped/partially open", 
                                   "open", 
                                   "closed", 
                                   "venting", 
                                   "opening", 
                                   "closing", 
                                   "error", 
                                   "unknown" };
    const std::string actions[7] = {"stop", 
                                    "open", 
                                    "close", 
                                    "venting", 
                                    "toggle_light", 
                                    "none" };

    char in_bufer[16] = {0}; 

  public:
    void start_frame_listener();
    void register_mqtt_client(Mqtt* mqtt);
    void send_action(std::string action);
    std::string get_state();
  private:
    void open_serial(char * serial_name, int boudrate);
    bool read_frames_loop();
    void responce_to_master();
    void publish_state_via_mqtt(std::string state);
    std::string parse_state(uint8_t* rx_buffer);
    void send_command(uint8_t* frame, uint8_t len);
    uint8_t calc_checksum(uint8_t *p_data, uint8_t length);
};


