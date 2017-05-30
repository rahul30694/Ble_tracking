#include "Arduino.h"
#include "serial.h"

#include <stdint.h>

void pinMode(uint8_t pin, uint8_t mode)
{
	serialPrint("Pin Number - %u is set to ", unsigned(pin));
	if (mode == INPUT)
		serialPrintln("INPUT");
	else
		serialPrintln("OUTPUT");
}


void digitalWrite(uint8_t pin, uint8_t val)
{
	serialPrint("Pin Number - %u is ", unsigned(pin));
	if (val == HIGH)
		serialPrintln("HIGH");
	else
		serialPrintln("LOW");
}

int digitalRead(uint8_t pin)
{
	serialPrintln("Pin Number - %u can't be read", unsigned(pin));
	return LOW;
}
