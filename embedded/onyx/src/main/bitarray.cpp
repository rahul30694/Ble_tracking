#include "bitarray.h"


#include <iostream>
#include <stdio.h>
#include <bitset>
#include <stdint.h>
https://www.quora.com/How-can-you-find-out-the-first-0-or-1-bit-set

class bit_reference {

private:

	//friend class bitarray;
	unsigned *p_elem;
	unsigned pos;

	// left undefined
	bit_reference();
	
public:

	bit_reference(unsigned *_p_elem, unsigned _pos)
	{
		p_elem = _p_elem;
		pos = _pos;
	}
	
	bit_reference& operator=(bool value)
	{
		if (value)
			*p_elem |= (1 << pos);
		else
			*p_elem &= ~(1 << pos);
		return *this;
	}

	bit_reference& operator=(const bit_reference &rhs)
	{
		if ((*rhs._p_elem) & (1 << rhs.pos))
			*p_elem |= (1 << pos);
		else
			*p_elem &= ~(1 << pos);
		return *this;
	}

	operator bool() const;
	bool operator~() const;
	~bit_reference() { 	}
};

bit_reference::operator bool() const
{
	return !!((*p_elem) & (1 << pos));
}

bool bit_reference::operator~() const
{
	return !((*p_elem) & (1 << pos));
}

/*
 * Class for bit array data structure
 *
 */

template <unsigned len>
class bitarray {

private:
	unsigned elem[CEIL(len, sizeof(unsigned) * BITS_IN_A_BYTE)];

	void sanitize(void);
	friend class bit_reference;

public:
	bitarray<len>(void) 
	{
		reset();
		sanitize();
	}

	bool operator[](unsigned ndx) const;
	bit_reference operator[](unsigned ndx);

	bitarray& operator&=(const bitarray& rhs);
	bitarray& operator|=(const bitarray& rhs);
	bitarray& operator^=(const bitarray& rhs);

	
	unsigned count(void) const;
	unsigned size(void) const;
	bool any(void) const;
	bool none(void) const;
	bool all(void) const;

	bool test(unsigned ndx) const;
	unsigned find_first(unsigned from_ndx=0) const;

	bitarray& set(unsigned ndx);
	bitarray& reset(unsigned ndx);
	bitarray& flip(unsigned ndx);

	bitarray& set(void);
	bitarray& reset(void);
	bitarray& flip(void);

	void toCString(char *cstr, unsigned cstr_max_len);
};


template <unsigned len>
void bitarray<len>::sanitize(void)
{
	unsigned elem_pos = len >> (2 + sizeof(*elem));
	unsigned pos = (elem_pos << (2 + sizeof(*elem))) ^ len;
	unsigned mask = (~static_cast<unsigned>(0)) >> pos;
	elem[elem_pos] &= mask;
}

template <unsigned len>
bool bitarray<len>::operator [](unsigned ndx) const
{
	unsigned byte = ndx >> (2 + sizeof(*elem));
	unsigned pos = (byte << (2 + sizeof(*elem))) ^ ndx;
	return !!(elem[byte] & (1 << pos));
}

template <unsigned len>
bit_reference bitarray<len>::operator[](unsigned ndx)
{
	unsigned byte = ndx >> ((2 + sizeof(*elem)));
	unsigned pos = (byte << (2 + sizeof(*elem))) ^ ndx;
	return bit_reference(elem+byte, pos);
}

template <unsigned len>
unsigned bitarray<len>::count(void) const
{
	unsigned i;
	unsigned elem_pos, bit, count=0;
	for (i=0; i<len; i++) {
		elem_pos = i >> ((2 + sizeof(*elem)));
		bit = (elem_pos << (2 + sizeof(*elem))) ^ i;
		if (elem[elem_pos] & (1 << bit))
			count++;
	}
	return count;
}

template <unsigned len>
unsigned bitarray<len>::size(void) const
{
	return len;
}

template <unsigned len>
bool bitarray<len>::any(void) const
{
	unsigned i,j;
	unsigned elem_pos, bit, count=0;
	for (i=0; i<(elem_len - 1); i++)
		if (elem[i])
			return true;

	i *= (8*sizeof(*elem));
	for (j=0; i<len; i++, j++)
		if (elem[elem_len - 1] & (1 << j))
			return true;
	return false;
}

template <unsigned len>
bool bitarray<len>::none(void) const
{
	return !any();
}
	
template <unsigned len>
bool bitarray<len>::all(void) const
{
	unsigned i,j;
	unsigned elem_pos, bit, count=0;
	for (i=0; i<(elem_len - 1); i++)
		if (elem[i] != (~(static_cast<int>(0))))
			return false;

	i *= (8*sizeof(*elem));
	for (j=0; i<len; i++, j++)
		if (!(elem[elem_len - 1] & (1 << j)))
			return false;
	return true;
}

template <unsigned len>
unsigned bitarray<len>::find_first(unsigned from_ndx) const
{
	unsigned i;
	for (i=from_ndx; i<len; i++)
		if ((*this)[i])
			return i;
	return len;
}

template <unsigned len>
bitarray<len>& bitarray<len>::set(unsigned ndx)
{
	unsigned pos = 
}

bitarray& reset(unsigned ndx);
bitarray& flip(unsigned ndx);

bitarray& set(void);
bitarray& reset(void);
bitarray& flip(void);


int main()
{
	int a = 0x00;
	bitarray<10> b;


	std :: cout << "value: ";
	if (b[1])
		std :: cout << "1" << std :: endl;
	else
		std :: cout << "0" << std :: endl;
	b[1] = 1;
	b[2] = 1;
	b[3] = 0;
	b[65] = 1;
	std :: cout << "value: ";
	if (b[1])
		std :: cout << "1" << std :: endl;
	else
		std :: cout << "0" << std :: endl;

	std :: cout << "value: ";
	if (b[2])
		std :: cout << "1" << std :: endl;
	else
		std :: cout << "0" << std :: endl;

	std :: cout << "value: ";
	if (b[3])
		std :: cout << "1" << std :: endl;
	else
		std :: cout << "0" << std :: endl;

	std :: bitset<10> arr;
	arr.reset();
	if (arr.any())
		std :: cout << "Some are set" << std :: endl;
	else
		std :: cout << "None is set" << std :: endl;
	arr[1]=true;
	//std :: cout << "Count: " << count(arr) << std :: endl;
	return 0;
}