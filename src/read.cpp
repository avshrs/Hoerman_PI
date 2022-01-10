#include "read.h"
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

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

void Read::serial_open(char *serial_name, int baud)
{
  // struct termios newtermios;
  // fd = open(serial_name,O_RDWR | O_NOCTTY);
  // if (fd < 0) 
  // {
  //   std::cout << "Error from open serial port" << fd << std::endl;
  // }   
  // if(tcgetattr(fd, &newtermios) != 0) 
  // {
  //   std::cout << "Error from tcgetattr " << std::endl;
  // }

  // newtermios.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  // // newtermios.c_cflag |= PARENB;  // Set parity bit, enabling parity
  // newtermios.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  // // newtermios.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
  // newtermios.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
  // // newtermios.c_cflag |= CS5; // 5 bits per byte
  // // newtermios.c_cflag |= CS6; // 6 bits per byte
  // // newtermios.c_cflag |= CS7; // 7 bits per byte
  // newtermios.c_cflag |= CS8; // 8 bits per byte (most common)
  // newtermios.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  // // newtermios.c_cflag |= CRTSCTS;  // Enable RTS/CTS hardware flow control
  // newtermios.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  // newtermios.c_lflag &= ~ICANON;
  // newtermios.c_lflag &= ~ECHO; // Disable echo
  // newtermios.c_lflag &= ~ECHOE; // Disable erasure
  // newtermios.c_lflag &= ~ECHONL; // Disable new-line echo
  // newtermios.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  // newtermios.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  // newtermios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  // newtermios.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  // newtermios.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT IN LINUX)
  // // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT IN LINUX)

  // newtermios.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  // newtermios.c_cc[VMIN] = 0;
  // cfsetispeed(&newtermios,baud);
  // cfsetospeed(&newtermios, baud);




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
      if (tcflush(fd,TCIFLUSH)==-1) std::cout << "error1" << std::endl;
      if (tcflush(fd,TCOFLUSH)==-1) std::cout << "error2" << std::endl;
      if (tcsetattr(fd,TCSANOW,&newtermios)==-1) std::cout << "error3" << std::endl;
      

      
}   


void Read::serial_open2(const char *device, int baudrate, bool rtscts, struct termios *old)
{
	struct termios new_ter;
	int b;

	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	
	if (fd == -1)
		std::cout << "open port error" << std::endl;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);

	if (old != NULL)
		tcgetattr(fd, old);

	b = convert_baudrate(baudrate);

	if (b == -1) {

		struct serial_struct ss;

		if (ioctl(fd, TIOCGSERIAL, &ss) == -1) {
			close(fd);
			std::cout << "ioctl error" << std::endl;
		}

		ss.flags = (ss.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
		ss.custom_divisor = (ss.baud_base + (baudrate / 2)) / baudrate;

		if (ioctl(fd, TIOCSSERIAL, &ss) == -1) {
			close(fd);
			std::cout << "ioctl 2 error" << std::endl;
		}

		if (ss.baud_base / ss.custom_divisor != baudrate)
			fprintf(stderr, "Baud rate set to %d\n", ss.baud_base / ss.custom_divisor);

		b = B38400;

	}

	new_ter.c_cflag = b | CS8 | CREAD;

	if (rtscts)
		new_ter.c_cflag |= CRTSCTS;
	else
		new_ter.c_cflag |= CLOCAL;

	new_ter.c_iflag = IGNPAR;
	new_ter.c_oflag = 0;
	new_ter.c_lflag = 0;
	new_ter.c_cc[VMIN] = 1;
	new_ter.c_cc[VTIME] = 0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &new_ter);

	
}



void Read::serial_send(char *data, int size)
{
  write(fd, data, size);
}

void Read::serial_read(char *data, int size)
{
     read(fd, data, size);
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

int Read::convert_baudrate(unsigned int baudrate)
{
	switch (baudrate) {
		case 50: return B50;
		case 75: return B75;
		case 110: return B110;
		case 134: return B134;
		case 150: return B150;
		case 200: return B200;
		case 300: return B300;
		case 600: return B600;
		case 1200: return B1200;
		case 1800: return B1800;
		case 2400: return B2400;
		case 4800: return B4800;
		case 9600: return B9600;
		case 19200: return B19200;
		case 38400: return B38400;
		case 57600: return B57600;
		case 115200: return B115200;
#ifdef B230400
		case 230400: return B230400;
#endif
#ifdef B460800
		case 460800: return B460800;
#endif
#ifdef B500000
		case 500000: return B500000;
#endif
#ifdef B576000
		case 576000: return B576000;
#endif
#ifdef B921600
		case 921600: return B921600;
#endif
#ifdef B1000000
		case 1000000: return B1000000;
#endif
#ifdef __linux__
		default: return -1;
#else
		default:
			fprintf(stderr, "Unknown baud rate %d\n", baudrate);
			exit(1);
#endif
	}
}