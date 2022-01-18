#pragma once
#include <stdint.h>
#include <string>
#include "vars.h"
class USB_serial
{
  public:
    int fd;
    int lead_z = 0 ;
    void serial_open(const char* serial_name, int boudrate, int lead_zero );
    void serial_send(TX_Buffer &tx_buffer);
    void serial_read(RX_Buffer &rx_buffer);
    void clear_buffer();
    void send_brake();

    uint8_t serial_read_byte(char *data, int timeout_usec);
    void serial_close();
    int convert_baudrate(unsigned int baudrate);
    


};

