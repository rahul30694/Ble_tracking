#include "eeprom.h"

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <string>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <assert.h>

#define PSTORAGE_META_PAGE_OFFSET 			0
#define MODULE_INFO_PAGE_OFFSET				1
#define FIRST_MODULE_PAGE_OFFSET			6

#define MAX_NUM_MODULES						6
#define MAX_BLKS_PER_MODULE					100
#define MAX_BLK_SIZE_IN_BYTES				128

#define PSTORAGE_META_SIZE					PAGE_SIZE_IN_BYTES
#define MODULE_INFO_BLK_SIZE				PAGE_SIZE_IN_BYTES


#define PSTORAGE_SIGNATURE					0xABCD
#define VERSION_NUMBER						0x11
#define MODULE_UID							0xEF


union pstorage_meta_u {
	uint8_t bytes[PSTORAGE_META_SIZE];
	struct {
		uint16_t signature;
		uint8_t version;
		uint8_t next_free_mod;
		uint32_t next_free_addr;
	};
}__attribute__((__packed__));

// static_assert(sizeof(union module_flags_u) == PAGE_SIZE_IN_BYTES, "Module Flags are not page aligned");

// http://www.thegeekstuff.com/2010/10/linux-error-codes
#define GENERIC_ERROR_BASE		(0x00)

#define _EAGAIN					(GENERIC_ERROR_BASE + 1)
#define _EINVAL					(GENERIC_ERROR_BASE + 2)
#define _EBUSY 					(GENERIC_ERROR_BASE + 3)
#define _EPERM 					(GENERIC_ERROR_BASE + 4)
#define _ENOTSUPPORTED 			(GENERIC_ERROR_BASE + 5)
#define _ENOTFOUND 				(GENERIC_ERROR_BASE + 6)
#define _EINTERNAL				(GENERIC_ERROR_BASE + 7)
#define _ENOMEM 				(GENERIC_ERROR_BASE + 8)
#define _ETIME 	 				(GENERIC_ERROR_BASE + 9)
#define _EIO 					(GENERIC_ERROR_BASE + 10)
#define _EOVERFLOW				(GENERIC_ERROR_BASE + 11)
#define _EINPROGRESS			(GENERIC_ERROR_BASE + 12)

#define MAX_MODULE_ID_SIZE		4

union module_info_u {
	uint8_t bytes[MODULE_INFO_BLK_SIZE];
	struct {
		uint8_t mod_uid;

		uint16_t mod_start_address;
		uint8_t num_blks;
		uint8_t blk_size;

		union {
			uint8_t flags;
			struct {
				uint8_t allocated:1;
				uint8_t worn_out:1;
				uint8_t replaced:1;

				uint8_t open:1;
				uint8_t dirty:1;
				uint8_t :3;
			};
		};
	};
} __attribute__((__packed__));

// static_assert(sizeof(module_info_s) == PAGE_SIZE_IN_BYTES, "Module Flags are not page aligned");

struct pstorage_cache_s {
	uint8_t version;
	uint8_t next_free_mod;
	uint32_t next_free_addr;

	union module_info_u mod_info[MAX_NUM_MODULES];

	union {
		uint16_t flags;
		struct {
			uint8_t isInitThisCycle:1;
			uint8_t init:1;

			uint8_t m0_dirty:1;
			uint8_t m1_dirty:1;
			uint8_t m2_dirty:1;
			uint8_t m3_dirty:1;
			uint8_t m4_dirty:1;
			uint8_t m5_dirty:1;
		};
	};
};

struct pstorage_cache_s pstorage_cache;

void pstorage_print_cache()
{
	if (!pstorage_cache.init)
		return;
	std::cout << "Pstorage Inited" << std::endl;
	std::cout << "Version: 0x" << std::hex << (unsigned)pstorage_cache.version << std::endl;
	std::cout << "Next Free Module Number: 0x" << std::hex << (unsigned)pstorage_cache.next_free_mod << std::endl;
	std::cout << "Next Free Address: 0x" << std::hex << (unsigned)pstorage_cache.next_free_addr << std::endl;
	std::cout << "isInitThisCycle: " << (pstorage_cache.isInitThisCycle ? "Yes" : "No") << std::endl;
}

	
unsigned pstorage_burn(void)
{
	uint8_t raw_page[PSTORAGE_META_SIZE];
	memset(raw_page, 0, sizeof(raw_page));

	union pstorage_meta_u *p_meta = reinterpret_cast<union pstorage_meta_u *>(raw_page);

	p_meta->signature = PSTORAGE_SIGNATURE;
	p_meta->version = VERSION_NUMBER;
	p_meta->next_free_mod = 0;
	p_meta->next_free_addr = ((MODULE_INFO_PAGE_OFFSET + MAX_NUM_MODULES) * PAGE_SIZE_IN_BYTES);

	if (eeprom_write(PSTORAGE_META_PAGE_OFFSET, 0, raw_page, PSTORAGE_META_SIZE)) {
		std::cerr << "Error: In Writing EEPROM" << std::endl;
		return _EIO;
	}


	// Writing to Info offset
	memset(raw_page, 0, sizeof(raw_page));

	union module_info_u *p_info = reinterpret_cast<union module_info_u *>(raw_page);

	unsigned i;
	for (i=0; i<MAX_NUM_MODULES; i++) {
		memset(p_info->bytes, 0, sizeof(p_info->bytes));

		if (eeprom_write(MODULE_INFO_PAGE_OFFSET + i, 0, raw_page, MODULE_INFO_BLK_SIZE)) {
			std::cerr << "Error: In Writing EEPROM" << std::endl;
			return _EIO;
		}
	}

	return 0;	
}

unsigned page_alligned_blk_read(unsigned mod_id, unsigned blk_id, unsigned offset, void *data, unsigned len)
{
	union module_info_u *p_mod = pstorage_cache.mod_info + mod_id;
	if (p_mod->num_blks <= blk_id)
		return _ENOTFOUND;

	unsigned mod_start_addr = p_mod->mod_start_address;
	unsigned blk_addr = mod_start_addr + (p_mod->blk_size * blk_id) + offset;

	unsigned blk_alligned_correction = blk_addr % PAGE_SIZE_IN_BYTES;
	blk_addr -= blk_alligned_correction;

	unsigned len_alligned_correction = len % PAGE_SIZE_IN_BYTES;
	if (len_alligned_correction)
		len += (PAGE_SIZE_IN_BYTES - len_alligned_correction);
	len += blk_alligned_correction;

	uint8_t page_alligned_data[len];
	uint8_t *p_alligned_data = page_alligned_data;

	unsigned page_num = blk_addr / PAGE_SIZE_IN_BYTES;
	unsigned total_page = len / PAGE_SIZE_IN_BYTES;

	unsigned i;
	for (i=0; i<(total_page); i++) {
		if (eeprom_read(page_num, 0, p_alligned_data, PAGE_SIZE_IN_BYTES))
			return _EIO;
		page_num++;
		p_alligned_data += PAGE_SIZE_IN_BYTES;
	}
	memcpy(data, page_alligned_data + blk_alligned_correction, len - 
						len_alligned_correction + blk_alligned_correction);
	return 0;
}

unsigned page_alligned_blk_write(unsigned mod_id, unsigned blk_id, unsigned offset, void *data, unsigned len)
{
	return _ENOTSUPPORTED;
}

unsigned cache_pstorage_in_ram(bool isInitThisCycle)
{
	uint8_t raw_page[PAGE_SIZE_IN_BYTES];
	memset(&pstorage_cache, 0, sizeof(pstorage_cache));

	if (eeprom_read(PSTORAGE_META_PAGE_OFFSET, 0, raw_page, PSTORAGE_META_SIZE)) {
		std::cerr << "Error in reading EEPROM" << std::endl;
		return _EIO;
	}

	union pstorage_meta_u *p_meta = reinterpret_cast<union pstorage_meta_u *>(raw_page);
	pstorage_cache.version = p_meta->version;
	pstorage_cache.next_free_addr = p_meta->next_free_addr;
	pstorage_cache.next_free_mod = p_meta->next_free_mod;

	unsigned i;
	for (i=0; i<MAX_NUM_MODULES; i++) {
		memset(raw_page, 0, sizeof(raw_page));

		if (eeprom_read(MODULE_INFO_PAGE_OFFSET + i, 0, raw_page, sizeof(raw_page))) {
			std::cerr << "Error in reading EEPROM" << std::endl;
			return _EIO;
		}
		memcpy(reinterpret_cast<void *>(pstorage_cache.mod_info + i), raw_page, sizeof(*pstorage_cache.mod_info));
	}

	pstorage_cache.isInitThisCycle = isInitThisCycle;
	pstorage_cache.init = true;
	
	return 0;
}

unsigned sync_cache_to_storage(unsigned mod_id)
{
	uint8_t raw_page[PAGE_SIZE_IN_BYTES];

	union pstorage_meta_u *p_meta = reinterpret_cast<union pstorage_meta_u *>(raw_page);
	p_meta->signature = PSTORAGE_SIGNATURE;
	p_meta->version = VERSION_NUMBER;
	p_meta->next_free_mod = pstorage_cache.next_free_mod;
	p_meta->next_free_addr = pstorage_cache.next_free_addr;

	if (eeprom_write(PSTORAGE_META_PAGE_OFFSET, 0, raw_page, PSTORAGE_META_SIZE)) {
		std::cerr << "Error:In Writing EEPROM" << std::endl;
		return _EIO;
	}

	unsigned i;
	for (i=0; i<MAX_NUM_MODULES; i++) {
		union module_info_u *p_info = pstorage_cache.mod_info + i;

		if (eeprom_write(MODULE_INFO_PAGE_OFFSET + i, 0, p_info->bytes, MODULE_INFO_BLK_SIZE)) {
			std::cerr << "Error: In Writing EEPROM" << std::endl;
			return _EIO;
		}
	}

	return 0;
}

unsigned pstorage_init()
{
	unsigned error=0;
	if (eeprom_init()) {
		std::cerr << "eeprom init failed" << std::endl;
		return _EIO;
	}

	uint8_t meta_raw[PSTORAGE_META_SIZE];

	if (eeprom_read(PSTORAGE_META_PAGE_OFFSET, 0, meta_raw, PAGE_SIZE_IN_BYTES)) {
		std :: cerr << "Reading Failed" << std :: endl;
		return _EIO;
	}

	union pstorage_meta_u *p_meta = reinterpret_cast<union pstorage_meta_u *>(meta_raw);

	bool isInitThisCycle=false;
	do {
		if (p_meta->signature == PSTORAGE_SIGNATURE)
			break;
		std::cout << "Formating pstorage..." << std::endl;
		if (error = pstorage_burn())
			break;

		isInitThisCycle=true;
		break;
	} while(0);

	if (error)
		return error;

	if (error = cache_pstorage_in_ram(isInitThisCycle))
		return error;
	
	pstorage_print_cache();
	return 0;
}

void pstorage_deinit()
{
	pstorage_cache.init=false;
	eeprom_deinit();
}



unsigned pstorage_module_open(unsigned mod_id)
{
	if (!pstorage_cache.mod_info[mod_id].allocated)
		return _ENOTFOUND;
	pstorage_cache.mod_info[mod_id].open = 1;
	return 0;
}

unsigned pstorage_module_close(unsigned mod_id)
{
	if (!pstorage_cache.mod_info[mod_id].allocated)
		return _ENOTFOUND;
	if (!pstorage_cache.mod_info[mod_id].open)
		return _ENOTFOUND;
	pstorage_cache.mod_info[mod_id].open = 0;
	return 0;
}

unsigned pstorage_module_create(unsigned *p_mod_id, unsigned num_blks, unsigned blk_size)
{
	unsigned error=0;
	unsigned mod_id = pstorage_cache.next_free_mod;
	if (mod_id >= MAX_NUM_MODULES)
		return _ENOMEM;

	std::cout << "Old Blk Size " << std::dec << blk_size << std::endl;
	unsigned aligned_correction = blk_size % PAGE_SIZE_IN_BYTES;
	if (aligned_correction)
		blk_size += (PAGE_SIZE_IN_BYTES - aligned_correction);

	if (blk_size > MAX_BLK_SIZE_IN_BYTES)
		return _ENOMEM;
	std::cout << "NEW Blk Size " << std::dec << blk_size << std::endl;

	// TODO: Handle low memory case!
	uint32_t mod_start_address = pstorage_cache.next_free_mod;

	pstorage_cache.mod_info[mod_id].mod_start_address = mod_start_address;
	pstorage_cache.mod_info[mod_id].num_blks = num_blks;
	pstorage_cache.mod_info[mod_id].blk_size = blk_size;

	pstorage_cache.mod_info[mod_id].mod_uid = MODULE_UID;
	pstorage_cache.mod_info[mod_id].flags = 0;
	pstorage_cache.mod_info[mod_id].allocated = 1;

	pstorage_cache.next_free_mod = mod_start_address + (blk_size*num_blks);
	pstorage_cache.next_free_mod = mod_id + 1;

	if (error = sync_cache_to_storage(mod_id))
		return error;
	*p_mod_id = mod_id;
	return 0;
}

unsigned pstorage_read_blk(unsigned mod_id, unsigned blk_id, void *data)
{
	if (!pstorage_cache.mod_info[mod_id].open)
		return _ENOTFOUND;

	return page_alligned_blk_read(mod_id, blk_id, 0, data, pstorage_cache.mod_info[mod_id].blk_size);
}

unsigned pstorage_write_blk(unsigned mod_id, unsigned blk_id, void *data)
{
	if (!pstorage_cache.mod_info[mod_id].open)
		return _ENOTFOUND;

	union module_info_u *p_mod = pstorage_cache.mod_info + mod_id;
	if (p_mod->num_blks <= blk_id)
		return _ENOTFOUND;

	unsigned mod_addr = p_mod->mod_start_address;
	unsigned blk_addr = mod_addr + (p_mod->blk_size * blk_id);

	std::cout << "Writing to addr: " << std::dec << blk_addr << std::endl;
	
	uint8_t *p_data = reinterpret_cast<uint8_t *>(data);
	unsigned i;
	for (i=0; i<(p_mod->blk_size / PAGE_SIZE_IN_BYTES); i++) {
		if (eeprom_write(blk_addr / PAGE_SIZE_IN_BYTES, 0, p_data, PAGE_SIZE_IN_BYTES))
			return _EIO;
		blk_addr += PAGE_SIZE_IN_BYTES;
		p_data += PAGE_SIZE_IN_BYTES;
	}

	
	return 0;
}

int main()
{
	if (pstorage_init()) {
		std::cerr << "Error In Init" << std::endl;
		return -1;
	}
	

	unsigned mod_id;

	if (pstorage_module_create(&mod_id, 10, 20)) {
		std::cerr << "Error in Creating Modules" << std::endl;
		return -1;
	}

	if (pstorage_module_open(mod_id)) {
		std::cerr << "Error in Opening Modules" << std::endl;
		return -1;
	}

	std::cout << "Writing... To Storage" << std::endl;

	char abhi[] = "123456789012345678901234";

	if (pstorage_write_blk(mod_id, 2, abhi)) {
		std::cerr << "Error in Writing Pstorage" << std::endl;
		return -1;
	}
	memset(abhi, 0, sizeof(abhi));
	if (pstorage_read_blk(mod_id, 2, abhi)) {
		std::cerr << "Error in Reading Pstorage" << std::endl;
		return -1;
	}
	std::cout << "Abhi: " << abhi << std::endl;

	pstorage_deinit();
	return 0;
}





