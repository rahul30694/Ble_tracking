#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdint.h>

#define MAX_FIELDS 			06
#define MAX_SUPPORTED_YR 	64

enum {
	YEAR=0,
	MONTH,
	DATE,

	HOUR,
	MIN,
	SEC
};

union rtcDateTime_u {
	uint8_t byte[MAX_FIELDS];
	struct {
		uint8_t YY;
		uint8_t MM;
		uint8_t DD;

		uint8_t hh;
		uint8_t mm;
		uint8_t ss;
	};
};

union rtcPackedTime_u {
	struct {
		uint8_t YY:6;
		uint8_t MM:4;
		uint8_t DD:5;

		uint8_t hh:5;
		uint8_t mm:6;
		uint8_t ss:6;
	};
	uint32_t packedTime;
}__attribute__((packed));

typedef union rtcPackedTime_u rtcPackedTime_t;
typedef union rtcDateTime_u rtcDateTime_t;

//extern const rtcDateTime_t RTCDATE2001JAN01;

#endif