#pragma once
#include <stdint.h>
#include <string>
#include "vars.h"
class USB_serial
{
  public:
    int fd;
    
    void serial_open(const char* serial_name, int boudrate);
    void serial_send(uint8_t *data, int size);
    void serial_read(uint8_t *data, int size, uint8_t lead_zero);
    void clear_buffer();
    void send_brake();

    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    int convert_baudrate(unsigned int baudrate);
    


};

