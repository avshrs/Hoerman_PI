#pragma once
#include <stdint.h>
class Read
{
  public:
    int fd;
    void serial_open(char *serial_name, int baud);
    void serial_open2(const char *device, int baudrate, bool rtscts, struct termios *old);
    void serial_send(char *data, int size);
    int serial_read(char *data, int size);
    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    int convert_baudrate(unsigned int baudrate);
    


};
