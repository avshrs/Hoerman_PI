#pragma once
#include "USB_serial.h"
#include <unistd.h>
#include <string>
#include <chrono>

#define BROADCAST_ADDR            0x00
#define MASTER_ADDR               0x80
#define UAP1_ADDR                 0x28

#define UAP1_TYPE                 0x14

#define CMD_SLAVE_SCAN            0x01
#define CMD_SLAVE_STATUS_REQUEST  0x20
#define CMD_SLAVE_STATUS_RESPONSE 0x29

#define RESPONSE_DEFAULT          0x1000
#define RESPONSE_STOP             0x0000
#define RESPONSE_OPEN             0x1001
#define RESPONSE_CLOSE            0x1002
#define RESPONSE_VENTING          0x1010
#define RESPONSE_TOGGLE_LIGHT     0x1008

#define CRC8_INITIAL_VALUE        0xF3

class Hoermann_pi{
    private:
        USB_serial serial;
    
    private:
        const std::string actions[7] = {"stop", 
                                        "open", 
                                        "close", 
                                        "venting", 
                                        "toggle_light", 
                                        "none" };
        const std::string states[8] = {"stoped/partially open", 
                                        "open", 
                                        "closed", 
                                        "venting", 
                                        "opening", 
                                        "closing", 
                                        "error", 
                                        "unknown" };
        
        std::chrono::high_resolution_clock timer;
        using mi = std::chrono::duration<float, std::micro>;
        uint8_t tx_length = 0;
        uint16_t slave_respone_data = RESPONSE_DEFAULT;
        
        uint16_t broadcast_status = 0;
        uint8_t lz = 0;
        uint8_t broadcast_lengh = 0x02; 
        uint8_t reguest_lengh = 0x01; 
        struct TX_Buffer{
            uint8_t buf[6]={0};
            uint8_t len=0;
            std::chrono::high_resolution_clock received_time;
        };
        struct RX_Buffer{
            uint8_t buf[6]={0};
            std::chrono::high_resolution_clock received_time;
        };


    public:
        void init(char* serial_name, int boudrate);
        void run_loop(void);
        std::string get_state();
        void set_state(std::string action);

    private:
        
        TX_Buffer parse_message(RX_Buffer buf);
        void update_broadcast_status(uint8_t* buf);
        
        uint8_t get_length(uint8_t* buf);
        uint8_t get_counter(uint8_t* buf);
        uint8_t calc_crc8(uint8_t* p_data, uint8_t length);

        bool is_broadcast(uint8_t* buf);
        bool is_slave_query(uint8_t* buf);
        bool is_slave_scan(uint8_t* buf);
        bool is_slave_status_req(uint8_t* buf);
        bool is_broadcast_lengh_correct(uint8_t* buf);
        bool is_req_lengh_correct(uint8_t* buf);
        void print_buffer(uint8_t* buf, int len);
        TX_Buffer make_scan_responce_msg(RX_Buffer buf);
        TX_Buffer make_status_req_msg(RX_Buffer buf);
};
