#include "uart_usb.h"
#include "serial.h"
	


#include <fcntl.h>   /* File Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions */
#include <errno.h>   /* Error Number Definitions */

#include <stdio.h>	
#include <stdlib.h>


int SerialPort::open(char *port)
{
	if (!	port) {
		serialPrintln("No Stream Found!");
		exit(1);
	}

	int fd = ::open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd < 1) {
		serialPrintln("Error in Opening the Devices\n");
		exit(1);
	}

	_fd=fd;

	return init(B115200);
}

int SerialPort::init(unsigned int baudRate)
{
	struct termios modem;
	tcgetattr(_fd, &modem);

	cfsetispeed(&modem, baudRate);
	cfsetospeed(&modem, baudRate);

	modem.c_cflag &= ~PARENB;
	modem.c_cflag &= ~CSTOP;
	modem.c_cflag &= ~CSIZE;
	modem.c_cflag |= CS8;

	modem.c_cflag &= ~CRTSCTS;
	modem.c_cflag |= CREAD | CLOCAL;

	modem.c_iflag &= ~(IXON | IXOFF | IXANY);
	modem.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	//_modem.c_cc[VMIN]=2;
	modem.c_cc[VTIME]=0;

	tcsetattr(_fd, TCSANOW, &modem);

	return 0;
}

int SerialPort::close()
{
	if (_fd < 1)
		return -1;
	
	return ::close(_fd);
}

int SerialPort::write(void *data, size_t bytes)
{
	if (_fd < 1)
		return -1;

	return ::write(_fd, data, bytes);
}

int SerialPort::read(void *data, size_t bytes)
{
	if (_fd < 1)
		return -1;

	return ::read(_fd, data, bytes);
}

