#include "read.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>


void Read::serial_open(char *serial_name, int baud)
{
  struct termios newtermios;
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
  // newtermios.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  newtermios.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
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
  // newtermios.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  newtermios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  newtermios.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  newtermios.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

  newtermios.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  newtermios.c_cc[VMIN] = 0;
  cfsetispeed(&newtermios, baud);
  cfsetospeed(&newtermios, baud);





  // newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
  // newtermios.c_iflag=IGNPAR;
  // newtermios.c_oflag=0;
  // newtermios.c_lflag=0;
  // newtermios.c_cc[VMIN]=1;
  // newtermios.c_cc[VTIME]=0;
  // cfsetospeed(&newtermios,baud);
  // cfsetispeed(&newtermios,baud);
  // if (tcflush(fd,TCIFLUSH)==-1) return -1;
  // if (tcflush(fd,TCOFLUSH)==-1) return -1;
  // if (tcsetattr(fd,TCSANOW,&newtermios)==-1) return -1;
}   

void Read::serial_send(char *data, int size)
{
  write(fd, data, size);
}

int Read::serial_read(char *data, int size, int timeout_usec)
{
      fd_set fds;
      struct timeval timeout;
      int count=0;
      int ret;
      int n;
      do { 
        FD_ZERO(&fds);
        FD_SET (fd, &fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = timeout_usec;
        ret = select (FD_SETSIZE,&fds, NULL, NULL, &timeout);
        if (ret==1) {
          n = read (fd, &data[count], size-count);
          
          count+=n;
          data[count]=0;
   }
 } while (count<size && ret==1);
 return count;
}
uint8_t Read::serial_read_byte(char *data, int timeout_usec)
{
      fd_set fds;
      struct timeval timeout;
      
      int ret;
      uint8_t n;
      FD_ZERO(&fds);
      FD_SET (fd, &fds);
      timeout.tv_sec = 0;
      timeout.tv_usec = timeout_usec;
      ret = select (FD_SETSIZE,&fds, NULL, NULL, &timeout);
      if (ret==1) {
        n = read (fd, &data[0], 1);
   }
 
 return n;
}

void Read::serial_close()
{
   close(fd);
}
