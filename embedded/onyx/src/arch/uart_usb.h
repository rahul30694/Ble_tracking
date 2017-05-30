#ifndef _UART_USB_H_
#define _UART_USB_H_

#include <termios.h> /* POSIX Terminal Control Definitions */
#include <stddef.h>


class SerialPort {
private:
	int _fd;

	int init(unsigned int baudRate);

public:
	SerialPort() { _fd=-1; }
	
	SerialPort(char *port) : _fd(-1) { open(port); }

	int open(char *port);
	bool isInit() { return (_fd >=1); }

	int write(void *data, size_t bytes);
	int read(void *data, size_t bytes);

	int putchar(char *c) { return write(c, 1); }
	int getchar(char *c) { return read(c, 1); }

	int close();

	~SerialPort() { close(); }
};

#endif