#pragma once
#include <stdint.h>
#include "vars.h"
class USB_serial
{
  public:
    int fd;
    void serial_open(char *serial_name, int baud);
    void serial_open2(const char *device, int baudrate, bool rtscts, struct termios *old);
    void serial_send(uint8_t *data, int size);
    void serial_read(uint8_t *data, int size);
    void serial_read2(RX_Buffer * rx_buf);
    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    int convert_baudrate(unsigned int baudrate);
    


};
