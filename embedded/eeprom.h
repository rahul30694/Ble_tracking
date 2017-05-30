#ifndef _EEPROM_H_
#define _EEPROM_H_

#define PAGE_SIZE_IN_BYTES		8
#define NUM_PAGES_IN_EPPROM		256

int eeprom_init(void);
void eeprom_clear(void);
int eeprom_write(unsigned page_num, unsigned offset, const void *data, unsigned len);
int eeprom_read(unsigned page_num, unsigned offset, void *data, unsigned len);
void eeprom_deinit(void);

#endif