#include "software_uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define FREQ_CPU 16000000UL

#define TX_PIN
#define RX_PIN

static uint32_t uartTicksDelay;
static uint8_t numOfBits;
static uint8_t numOfStopBits;
static bool parity;

enum mode_e {
	_8N1,
	_8P1,
	_9N1,
	_9P1,
	_8N2,
	_8P2,
	_9N2,
	_9P2
};



bool uartSftInit(uint32_t bitRate, enum mode_e mode)
{
	uartTicksDelay = (uint32_t)FREQ_CPU / bitRate;	
}

int uartSftTransmit(uint16_t data)
{
	//digitalWrite(TX_PIN, LOW);
	softUARTDelay();


	int i = 0;

	//for (i = 0; i < numOfBits; i++)
		//digitalWrite(TX_PIN, data  & (1<<i));

	if (parity) {
		//digitalWrite(TX_PIN, calculateParity(data));
	}

	for (i = 0; i < numOfStopBits; i++) {
		//digitalWrite(TX_PIN, LOW);
		softUARTDelay();
	}

}

int uartReceive(int *data)
{
	while (digitalRead(RX_PIN));
	softUARTDelay();

	softUARTDelay_2();

	int i;
	memset((void *)data, 0, sizeof(data));

	for (i = 0; i < numOfBits; i++) {
		*data |= (digitalRead(RX_PIN) << i);
		softUARTDelay();
	}
	int retVal = 0;

	if (!digitalRead(RX_PIN))
		retVal = -1;

	softUARTDelay_2();

	if (numOfStopBits > 1)
		softUARTDelay();

	if (!(parity && checkParity(*data)))
		retVal = -1;
		
	return retVal;
}

#ifdef __MAIN__

int main()
{

	return 0;
}

#endif
