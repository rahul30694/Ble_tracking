#include "eeprom.h"

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string.h>
#include <string>

const char *eeprom_file_name = "eeprom.bin";


std::fstream eeprom_file;


int eeprom_init(void)
{
	eeprom_file.open(eeprom_file_name, std::fstream::binary | std::fstream::out | std::fstream::in | std::fstream::ate);
	if (!eeprom_file.is_open())
		return -1;
	eeprom_file.seekp(PAGE_SIZE_IN_BYTES * NUM_PAGES_IN_EPPROM, std::fstream::beg);
	eeprom_file.write("\0", 1);
	eeprom_file.seekp(0);
	eeprom_file.seekg(0);
	return 0;
}

void eeprom_clear()
{
	eeprom_file.seekg(0, std::fstream::end);
	unsigned epprom_size = eeprom_file.tellg();

	std :: cout << "Epprom Size is " << epprom_size << std :: endl;
	std :: cerr << "Clearing..." << std :: endl;

	const char clear_data = 0x00;

	unsigned i;
	for (i=0; i<epprom_size; i++) {
		eeprom_file.seekp(epprom_size - i);
		eeprom_file.write(&clear_data, 1);
	}
	eeprom_file.seekg(0, std :: fstream :: beg);
}


int eeprom_write(unsigned page_num, unsigned offset, const void *data, unsigned len)
{
	if ((PAGE_SIZE_IN_BYTES - offset) < len)
		return -1;
	std::cout << "len " << len << std :: endl;
	uint32_t page_addr = page_num * PAGE_SIZE_IN_BYTES;
	uint32_t offset_in_page = offset;

	eeprom_file.seekp(page_addr + offset_in_page, std::fstream::beg);
	eeprom_file.write(reinterpret_cast<const char *>(data), len);
	eeprom_file.sync();

	if (eeprom_file.good())
		return 0;
	return -1;
}

int eeprom_read(unsigned page_num, unsigned offset, void *data, unsigned len)
{
	if ((PAGE_SIZE_IN_BYTES - offset) > len)
		return -1;

	uint32_t page_addr = page_num * PAGE_SIZE_IN_BYTES;
	uint32_t offset_in_page = offset;

	eeprom_file.seekg(page_addr + offset_in_page);
	eeprom_file.read(reinterpret_cast<char *>(data), len);

	if (eeprom_file.good())
		return 0;
	return -1;
}

void eeprom_deinit()
{
	eeprom_file.close();
}
// #define __MAIN__

#ifdef __MAIN__
int main()
{
	const char *a = "abhi";

	if (eeprom_init()) {
		std :: cerr << "Error in init epprom" << std :: endl;
		return -1;
	}

	eeprom_clear();

	if (eeprom_write(0, 2, a, strlen(a))) {
		std :: cerr << "Error in writing to epprom" << std :: endl;
		return -1;
	}


	eeprom_deinit();

	return 0;
}
#endif