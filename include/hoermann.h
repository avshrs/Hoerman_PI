#include "USB_serial.h"
#include <unistd.h>


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

        uint8_t rx_buffer[15+3] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool rx_message_ready = false;
        uint8_t tx_buffer[15+3] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool tx_message_ready = false;
        uint8_t tx_length = 0;
        uint16_t slave_respone_data = RESPONSE_DEFAULT;
        uint16_t broadcast_status = 0;

        
    public:
        void init(char* serial_name, int boudrate);
        void run_loop(void);
        std::string get_state(char data);
        void set_state(std::string action);

    private:
        uint8_t calc_crc8(uint8_t *p_data, uint8_t length);
        void parse_message(void);

};
