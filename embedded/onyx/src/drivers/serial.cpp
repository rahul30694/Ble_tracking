#include "serial.h"
#include "print.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define _BSD_SOURCE
#define _SVID_SOURCE

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>


/*
 * Read more about Termios: http://man7.org/linux/man-pages/man3/termios.3.html
 */

 #define MAX_INPUT_QUEUE_LEN		1000

class Serial : public Print{

private:
	int fd;

	int map_to_termios_buad(unsigned baud_rate);
	uint8_t map_to_c_cflag(uint8_t cfg, bool flow_control);

public:
	explicit Serial(const char *dev_path);
	~Serial()
	{
		if (fd > 0)
			::close(fd);
	}

	//int open(const char *dev_path);

	int begin(unsigned baud_rate) { return begin(baud_rate, SERIAL_8N1); }
	int begin(unsigned baud_rate, uint8_t cfg);

	void end(void);

	int write(uint8_t c);

	int available(void);
	void flush(void);
	char read(void);
	bool find(const char *cstr);

/*
	size_t print(const char *str);
	size_t print(const int val);
	size_t print(const unsigned val);
	size_t print(const long val);
	size_t print(const unsigned long val);

	size_t println(const char *str);
	size_t println(const int val);
	size_t println(const unsigned val);
	size_t println(const long val);
	size_t println(const unsigned long val);
*/
};

int Serial::write(uint8_t c)
{
	return ::write(fd, &c, 1);
}

int Serial::map_to_termios_buad(unsigned baud_rate)
{
	if (baud_rate == 0)
		return B0;
	else if (baud_rate == 50)
		return B50;
	else if (baud_rate == 75)
		return B75;
	else if (baud_rate == 110)
		return B110;
	else if (baud_rate == 134)
		return B134;
	else if (baud_rate == 150)
		return B150;
	else if (baud_rate == 200)
		return B200;
	else if (baud_rate == 300)
		return B300;
	else if (baud_rate == 600)
		return B600;
	else if (baud_rate == 1200)
		return B1200;
	else if (baud_rate == 1800)
		return B1800;
	else if (baud_rate == 2400)
		return B2400;
	else if (baud_rate == 4800)
		return B4800;
	else if (baud_rate == 9600)
		return B9600;
	else if (baud_rate == 19200)
		return B19200;
	else if (baud_rate == 38400)
		return B38400;
	else if (baud_rate == 57600)
		return B57600;
	else if (baud_rate == 115200)
		return B115200;
	else if (baud_rate == 230400)
		return B230400;
	else 
		return -1;

}

uint8_t Serial::map_to_c_cflag(uint8_t cfg, bool flow_control)
{
	// Parity
	uint8_t c_cflag = 0x00;

	uint8_t parity = (cfg >> 4) & 0b11;
	uint8_t stop_bits = (cfg >> 3) & 0b1;
	uint8_t char_size = (cfg >> 1) & 0b11;

	if (parity == 0b10) // Even Parity
		c_cflag |= PARENB;
	else if (parity == 0b11)
		c_cflag |= PARENB | PARODD;

	if (stop_bits == 1)
		c_cflag |= CSTOPB;

	if (char_size == 0b00)
		c_cflag |= CS5;
	else if (char_size == 0b01)
		c_cflag |= CS6;
	else if (char_size == 0b10)
		c_cflag |= CS7;
	else if (char_size == 0b11)
		c_cflag |= CS8;
	else	// Default Case
		c_cflag |= CS8;

	if (flow_control)
		c_cflag |= CRTSCTS;

	return c_cflag;
}

Serial::Serial(const char *dev_path)
{
	fd = open(dev_path, O_RDWR | O_NOCTTY);
}

int Serial::begin(unsigned baud_rate, uint8_t cfg)
{
	struct termios setting, verify_setting;
	if (fd < 0)
		return -1;

	int termios_buad = map_to_termios_buad(baud_rate);
	if (termios_buad < 0)
		return -1;
        
    memset(&setting, 0, sizeof(setting));
    setting.c_cflag = CS8 | CLOCAL | CREAD;
    setting.c_iflag = 0;
    setting.c_oflag = 0;
    setting.c_lflag = 0;      // Noncanonical mode
         
    setting.c_cc[VTIME] = 0;  // Timeout in deciseconds for noncanonical read
    setting.c_cc[VMIN] = 0;   // Minimum number of characters for noncanonical read

    if (cfsetospeed(&setting, termios_buad))
    	return -1;
 	if (cfsetispeed(&setting, termios_buad))
 		return -1;

    if (tcflush(fd, TCIOFLUSH))			// Flush non-tramsmitted and unread data!
    	return -1;
    if (tcsetattr(fd, TCSANOW, &setting)) // Set attribute now!
    	return -1;
    if (tcgetattr(fd, &verify_setting))	// Get attribute now!
    	return -1;
    // return 0;
    if (memcmp(&setting, &verify_setting, sizeof(setting)))
    	return -1;
    return 0;
}


void Serial::end()
{
	if (fd >= 0)
		close(fd);
}

void Serial::flush()
{
	while (tcdrain(fd));
}

int Serial::available()
{
	int in_bytes;
	ioctl(fd, FIONREAD, &in_bytes);
	return in_bytes;
}


char Serial::read()
{
	char byte;
	if (::read(fd, &byte, 1) == 0)
		return -1;
	return byte;
}	

bool Serial::find(const char *target)
{
	char message[MAX_INPUT_QUEUE_LEN+1];
	int num_read=0;
	int target_len = strlen(target);

	while (true) {
		if (available() <= 0)
			continue;

		if (num_read >= MAX_INPUT_QUEUE_LEN)
			return false;

		message[num_read++] = read();
		message[num_read] = '\0';

		char *look_ptr = (message + num_read - target_len) < message ? message : (message + num_read - target_len);
		if (!strcmp(look_ptr, target))
			break;
	}
	std :: cout << "Message: " << message << std :: endl;
}

/*

size_t Serial::print(const char *str)
{
	if (fd < 0)
		return 0;
	return write(fd, str, strlen(str));
}

size_t Serial::print(const int val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 2];
	sprintf(int_to_str, "%d", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::print(const unsigned val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 2];
	sprintf(int_to_str, "%u", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::print(const long val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 2];
	sprintf(int_to_str, "%ld", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::print(const unsigned long val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 2];
	sprintf(int_to_str, "%lu", val);
	return write(fd, int_to_str, strlen(int_to_str));
}


size_t Serial::println(const char *str)
{
	if (fd < 0)
		return 0;
	int str_len = strlen(str);

	char str_nl[str_len + 1];
	strcpy(str_nl, str);
	str_nl[str_len++] = '\n';
	str_nl[str_len] = '\0';

	return write(fd, str_nl, str_len);
}

size_t Serial::println(const int val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 3];
	sprintf(int_to_str, "%d\n", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::println(const unsigned val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 3];
	sprintf(int_to_str, "%u\n", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::println(const long val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 3];
	sprintf(int_to_str, "%ld\n", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

size_t Serial::println(const unsigned long val)
{
	if (fd < 0)
		return 0;
	char int_to_str[(sizeof(val)*8) + 3];
	sprintf(int_to_str, "%lu\n", val);
	return write(fd, int_to_str, strlen(int_to_str));
}

*/


const char *str_ok = "OK";
#define MAX_MESSAGE_SIZE	1000

int readTillOK(Serial& Serial)
{
	char message[MAX_MESSAGE_SIZE+1];
	int num_read=0;
	while (true) {
		if (Serial.available() <= 0)
			continue;
		if (num_read >= MAX_MESSAGE_SIZE)
			return -1;
		message[num_read++] = Serial.read();
		message[num_read] = '\0';
		char *potential_ok = (message + num_read - 2) < message ? message : (message + num_read - 2);
		if (!strcmp(potential_ok, str_ok))
			break;
	}
	std :: cout << "Message: " << message << std :: endl;
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	Serial Serial(argv[1]);

	if (Serial.begin(115200, SERIAL_8N1)) {
		std :: cerr << "Fail to connect" << std :: endl;
		return -1;
	}

/*
	Serial.print("printmodulus 100 set drop\r\n");
	Serial.find("OK"); //readTillOK(Serial);
	Serial.print("printmask\r\n");
	Serial.find("OK"); //readTillOK(Serial);
	Serial.print("gyror_trigger gyrop_trigger or set drop\r\n");
	Serial.find("OK"); //readTillOK(Serial);
	Serial.print("printtrigger printmask set drop\r\n");
	Serial.find("OK"); //readTillOK(Serial);
	//Serial.print("1 compass.p\r\n");
*/
	//Serial.print("restart\r\n");
	//Serial.find("OK"); //readTillOK(Serial);

	//Serial.print("reset\r\n");
	//Serial.find("OK"); //readTillOK(Serial);
	//usleep(100000);

	//Serial.print("roll di.\r\n");
	//Serial.find("OK"); //readTillOK(Serial);

	Serial.print("yaw di.\r\n");
	Serial.find("OK"); //readTillOK(Serial);

	//Serial.find("OK"); //readTillOK(Serial);
	while (true) {
		if (Serial.available() <= 0)
			continue;
		std :: cout << Serial.read();
	}
	return 0;	
}