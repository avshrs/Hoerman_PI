#pragma once
#include <stdint.h>
#include <string>
#include "vars.h"
class USB_serial
{
  public:
    int fd;
    termios tty;
    void serial_open(const char* serial_name);
    void set_cs8_19200();
    void set_cs7_9600();
    void serial_open2(const char *device, int baudrate, bool rtscts, struct termios *old);
    void serial_send(uint8_t *data, int size);
    void serial_read(uint8_t *data, int size);
    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    int convert_baudrate(unsigned int baudrate);
    


};

