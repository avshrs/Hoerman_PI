#include "read.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>


int Read::serial_open(char *serial_name, int baud)
{
      struct termios newtermios;
      fd = open(serial_name,O_RDWR | O_NOCTTY);
      newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
      newtermios.c_iflag=IGNPAR;
      newtermios.c_oflag=0;
      newtermios.c_lflag=0;
      newtermios.c_cc[VMIN]=1;
      newtermios.c_cc[VTIME]=0;
      cfsetospeed(&newtermios,baud);
      cfsetispeed(&newtermios,baud);
      if (tcflush(fd,TCIFLUSH)==-1) return -1;
      if (tcflush(fd,TCOFLUSH)==-1) return -1;
      if (tcsetattr(fd,TCSANOW,&newtermios)==-1) return -1;
      return fd;
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
