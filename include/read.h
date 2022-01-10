#pragma once

class Read
{
  public:
    int fd;
    int serial_open(char *serial_name, int baud);
    void serial_send(char *data, int size);
    int serial_read(char *data, int size, int timeout_usec);
    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    

};
