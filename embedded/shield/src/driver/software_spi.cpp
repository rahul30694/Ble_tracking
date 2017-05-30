#include "software_spi.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SYS_FREQ 4000000L

enum spiMode_e {
	MODE_0 = 0b00,
	MODE_1,
	MODE_2,
	MODE_3
};


/*
 *    SPI Mode	  Clock Polarity     Clock Edge
 *	       	    (CPOL/CKP)       (CKE/CPHA)
 *	0		0		0
 *	1		0		1
 *	2		1		0
 *	3		1		1
 */

struct {
	union {
		uint8_t mode;
		struct {
			uint8_t CPOL:1;
			uint8_t CPHA:1;
		};
	};
	uint8_t numBits;
	uint32_t sclkPulseWidth;
}__attribute__((packed))spiStatus;


bool spiSftInit(enum spiMode_e mode, uint32_t freq, uint8_t numBits)
{
	if ((numBits != 8) || (numBits != 16))
		return false;

	spiStatus.mode = mode;
	spiStatus.numBits = numBits;
	spiStatus.sclkPulseWidth = SYS_FREQ / freq;

	//digitalWrite(SCK, spiStatus.CPOL);
	//softDelay();
	return true;
}


void softDelay(void)
{
	//uint8_t count = spiStatus.clkBlockDelay;
	//while (count--);
}

/*
 * Design Rules
 * [a].CPHA=0 means sampling on the first clock edge and,
 * while CPHA=1 means sampling on the second clock edge,
 * regardless of whether that clock edge is rising or falling. 
 *
 * [b].CPHA=0, the data must be stable for a half cycle before the first clock cycle.
 */

static inline bool spiTransferBit(bool writeBit)
{
	bool readBit;
	//toggleSLCK(); //Change Sclk to active state
	//spiStatus.CPHA ? SDO = writeBit : readBit = SDI;
	softDelay();
	//toggleSLCK(); //change sclk to idle state
	//spiStatus.CPHA ? readBit = SDI : SDO = writeBit;
	softDelay();
	return readBit;
}

static inline uint16_t spiTransfer(uint16_t writeByte)
{
	uint8_t i;
	uint16_t readByte = 0x0000;
	for (i = 0; i < spiStatus.numBits; i++) {
		readByte <<= 1;
		readByte |= spiTransferBit(writeByte & (1 << (7 - i)));
	}
	return readByte;
}

uint16_t spiWrite(uint16_t data)
{
	return spiTransfer(data);
}

#define SPI_NULL_U16 0x0000
uint16_t spiRead()
{
	return spiTransfer(SPI_NULL_U16);
}
