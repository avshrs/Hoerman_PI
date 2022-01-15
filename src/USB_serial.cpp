#include "USB_serial.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <termios.h>
#include <iomanip>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/ioctl.h>
#ifdef __linux__
#include <linux/serial.h>
#endif
struct termios newtermios;
void USB_serial::serial_open(const char *serial_name, int boudrate)
{
  
  fd = open(serial_name,O_RDWR | O_NOCTTY);
  if (fd < 0) 
  {
    std::cout << "Error from open serial port" << fd << std::endl;
  }   
  if(tcgetattr(fd, &newtermios) != 0) 
  {
    std::cout << "Error from tcgetattr " << std::endl;
  }

  newtermios.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  // newtermios.c_cflag |= PARENB;  // Set parity bit, enabling parity
  newtermios.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  // newtermios.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
  newtermios.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
  // newtermios.c_cflag |= CS5; // 5 bits per byte
  // newtermios.c_cflag |= CS6; // 6 bits per byte
  // newtermios.c_cflag |= CS7; // 7 bits per byte
  newtermios.c_cflag |= CS8; // 8 bits per byte (most common)
  newtermios.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  // newtermios.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control
  newtermios.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  newtermios.c_lflag &= ~ICANON;
  newtermios.c_lflag &= ~ECHO; // Disable echo
  newtermios.c_lflag &= ~ECHOE; // Disable erasure
  newtermios.c_lflag &= ~ECHONL; // Disable new-line echo
  newtermios.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  newtermios.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  newtermios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  newtermios.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  newtermios.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

  newtermios.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  newtermios.c_cc[VMIN] = 0;
  cfsetispeed(&newtermios,boudrate);
  cfsetospeed(&newtermios, boudrate);
    
}   

void USB_serial::clear_buffer()
{
    tcflush(fd,TCIOFLUSH);
}

void USB_serial::send_brake()
{
    char buf[1] = {0};
    newtermios.c_cflag |= CS7;
    cfsetispeed(&newtermios,B9600);
    cfsetospeed(&newtermios, B9600);
    tcsetattr(fd, TCSANOW, &newtermios);
    usleep(250);
    
    write(fd, buf, 1);
		
		tcdrain( fd );
    
    newtermios.c_cflag |= CS8;
    cfsetispeed(&newtermios,B19200);
    cfsetospeed(&newtermios, B19200);
    tcsetattr(fd, TCSANOW, &newtermios);
    usleep(250);
}

void USB_serial::serial_send(uint8_t *data, int size)
{ 	char buf[15+3] = {0};
	for(int i = 0; i< size; i++)
	{
    
		buf[i] = static_cast<char>(data[i]);
	}

  send_brake();
	write(fd, buf, size);
}

void USB_serial::serial_read(uint8_t *data, int size, uint8_t lead_zero)
{	
	char buf[10] = {0};
  char buf_[1] = {0};
  for(int i = 0 ; i<sizeof(buf); i++){
    read(fd, buf_, 1);  
    if(buf_[0] = '\0'){
      break;
    }
    buf[i] = buf_[0];
  }
  // read(fd, buf, size);

	for(int i=0; i < size; i++)
	{
		data[i] = static_cast<uint8_t>(buf[i]);
    
	}
      for(int i = 0; i < 6  ; i++)
        {
        std::cout << " 0x" << std::setw(2);
        std::cout << std::setfill('0') << std::hex;
        std::cout << static_cast<int>(data[i]);
        }
    std::cout<<std::endl;
}

void USB_serial::serial_close()
{
   close(fd);
}

