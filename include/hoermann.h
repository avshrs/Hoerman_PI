#pragma once
#include <string>
#include "USB_serial.h"
// #include "Mosquitto.h"
class Mqtt{

};
class Hoermann_pi
{
  private:
    Mqtt mqtt;
    USB_serial serial;
    const uint8_t master_id = 0x80;
    const uint8_t device_id = 0x28;
    const uint8_t sync_id = 0x55;
    //broadcast message
    const uint8_t broadcast_id = 0x00;
    const uint8_t seq_mask = 0xf0;
    const uint8_t seq_sign_mask = 0x0f;
    const uint8_t seq_sign = 0x02;

    std::string bufferred_state;

    uint8_t sync_seq_number = 1;


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
    void open_serial(char * serial_name, int boudrate);
    void start_frame_listener();
    void register_mqtt_client(Mqtt* mqtt);
    void send_action(std::string action);
    std::string get_state();
  private:
    
    bool read_frames_loop();
    void responce_to_master();
    void publish_state_via_mqtt(std::string state);
    std::string parse_state(char data);
    void send_command(uint8_t* frame, uint8_t len);
    char calc_checksum(char *p_data, uint8_t length);
};


