#include "bitArray.h"

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

// op = 1 => set bit at ndx
// op = 0 => clear bit at ndx
// op = -1 => toggle bit at ndx

#define size_in_bits(x) 				(sizeof(x) << 3)

int modify_bit(uint8_t *arr, int16_t size, int16_t ndx, int8_t op)
{
	if (ndx >= size)
		return -1;

	int byte_ndx = ndx >> 3;
	int bit_ndx = ndx & (byte_ndx << 3);

	if (!op) 			//clear bit op = 0
		bitClear(arr[byte_ndx], bit_ndx);
	else if (op > 0) 	//set bit op = 1
		bitSet(arr[byte_ndx], bit_ndx);
	else 				//toggle bit op = -1
		bitToggle(arr[byte_ndx], bit_ndx);
	return 0;
}

int read_bit(uint8_t *arr, int16_t size, int16_t ndx)
{
	if (ndx >= size)
		return -1;

	int byte_ndx = ndx >> 3;
	int bit_ndx = ndx & (byte_ndx << 3);

	return bitRead(arr[byte_ndx], bit_ndx);
}

bool clear_test(uint8_t *arr, int16_t size, int16_t ndx)
{
	if (ndx >= size)
		ndx = size - 1;

	int byte_ndx = ndx >> 3;
	int last_bit = ndx & (byte_ndx << 3);

	int i;
	for (i = 0; i < byte_ndx; i++) 
		if (arr[i]) return false;

	for (i = 0; i < last_bit; i++)
		if (bitRead(arr[byte_ndx], i)) return false;
	return true;
}

bool set_test(uint8_t *arr, int16_t size, int16_t ndx)
{
	if (ndx >= size)
		ndx = size - 1;
	int byte_ndx = ndx >> 3;
	int last_bit = ndx & (byte_ndx << 3);

	int i;
	for (i = 0; i < byte_ndx; i++)
		if (~arr[i]) return false;

	for (i = 0; i < last_bit; i++)
		if (!(bitRead(arr[byte_ndx], i))) return false;
	return true;
}

bool reset_bit_array(uint8_t *arr, int16_t size, int16_t ndx)
{
	if (ndx >= size)
		ndx = size - 1;

	int byte_ndx = ndx >> 3;
	int last_bit = ndx & (byte_ndx << 3);

	memset(arr, 0, byte_ndx * sizeof(uint8_t));
	int i;
	for (i = 0; i < last_bit; i++)
		bitClear(bitArr[byte_ndx], i);
	return true;
}

bool set_bit_array(uint8_t *arr, int16_t size, int16_t ndx)
{
	if (ndx >= size)
		ndx = size - 1;
	int byte_ndx = ndx >> 3;
	int last_bit = ndx & (byte_ndx << 3);

	memset(arr, 0xFF, elemNum * sizeof(uint8_t));
	
	int i;
	for (i = 0; i < last_bit; i++)
		bitSet(arr[byte_ndx], i);
	return true;
}
