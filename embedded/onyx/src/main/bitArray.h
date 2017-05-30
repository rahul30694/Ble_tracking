/* 
 * File:   bitArray.h
 * Author: abhiarora
 *
 * Created on 2 January, 2016, 10:29 AM
 */

#ifndef BITARRAY_H
#define	BITARRAY_H

#include <stdbool.h>
#include <stdint.h>
#include "utils/utils.h"

#define BITARRAY_MU_TYPE uint8_t //type of minimum unit!

#define BITARRAY_DEF(x,s) BITARRAY_MU_TYPE bitArr_##x[ceil(s, sizeof(BITARRAY_MU_TYPE))]; \
	inline int x##_clearBit(int ndx) { return modifyBitArray(bitArr_##x, s, ndx, 0); } \
	inline int x##_setBit(int ndx) { return modifyBitArray(bitArr_##x, s, ndx, 1); } \
	inline int x##_toggleBit(int ndx) { return modifyBitArray(bitArr_##x, s, ndx, -1); } \
	inline int x##_readBit(int ndx) { return readBitArray(bitArr_##x, s, ndx); } \
	inline int x##_setBitTest(int ndx) { return isBitArrSetTillNdx(bitArr_##x, s, ndx); } \
	inline int x##_clearBitTest(int ndx) { return isBitArrClearTillNdx(bitArr_##x, s, ndx); } \
        inline int x##_clearBitArr(int ndx) { return clearBitArray(bitArr_##x, s, ndx); } \
        inline int x##_setBitArr(int ndx) { return setBitArray(bitArr_##x, s, ndx); } 

#define BITARRAY_EXTERN(x) extern BITARRAY_MU_TYPE bitArr_##x[]; \
		int x##_clearBit(int ndx); \
		int x##_setBit(int ndx); \
		int x##_toggleBit(int ndx); \
		int x##_readBit(int ndx); \
		int x##_setBitTest(int ndx); \
		int x##_clearBitTest(int ndx); \
		int x##_clearBitArr(int ndx); \
		int x##_setBitArr(int ndx);  

        
int modifyBitArray(BITARRAY_MU_TYPE *bitArr, int size, int ndx, int op);
int readBitArray(BITARRAY_MU_TYPE *bitArr, int size, int ndx);

bool isBitArrClearTillNdx(BITARRAY_MU_TYPE *bitArr, int size, int ndx);
bool isBitArrSetTillNdx(BITARRAY_MU_TYPE *bitArr, int size, int ndx);

bool clearBitArray(BITARRAY_MU_TYPE *bitArr, int size, int ndx);
bool setBitArray(BITARRAY_MU_TYPE *bitArr, int size, int ndx);


#define BITARR_setRefId(x,ndx) (x##_setBit(ndx))
#define BITARR_clearRefId(x,ndx) (x##_clearBit(ndx))
#define BITARR_toggleRefId(x,ndx) (x##_toggleBit(ndx))
#define BITARR_readRefId(x,ndx) (x##_readBit(ndx))
#define BITARR_setTestRefId(x,ndx) (x##_setBitTest(ndx))
#define BITARR_clearTestRefId(x,ndx) (x##_clearBitTest(ndx))
#define BITARR_setBitArrRefId(x,ndx) (x##_setBitArr(ndx))
#define BITARR_clearBitArrRefId(x,ndx) (x##_clearBitArr(ndx))

#endif	/* BITARRAY_H */

