#include "time.h"
//#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TIME_STR_LEN			30

#define SECS_IN_A_MIN 				60
#define MINS_IN_A_HOUR 				60
#define HOURS_IN_A_DAY 				24
#define MONTHS_IN_A_YEAR			12
#define DAYS_IN_A_WEEK 				07

#define isLeapYear(yy) 				((!(yy & 0x3)) && (((yy % 25)) || (!(yy & 15))))
#define DAYS_IN_YR(yy) 				(isLeapYear(yy) ? 366 : 365)
#define DAYS_IN_FEB(yy) 			(isLeapYear(yy) ? 29 : 28)
#define DAYS_IN_MN_OF_YR(mm,yy) 	((mm == 1) ? DAYS_IN_FEB(yy) : daysInMM[mm])

#define getEpochDay() 				(RTCDATE2001JAN01.DD)
#define getEpochMonth() 			(RTCDATE2001JAN01.MM)
#define getEpochYear() 				(RTCDATE2001JAN01.YY)

const rtcDateTime_t RTCDATE2001JAN01 = {
	.YY = 1, .MM = 0, .DD = 1,
	.hh = 0, .mm = 0, .ss = 0
};

const char *const month_str[MONTHS_IN_A_YEAR] = { 
	"Jan", "Feb", "Mar", 
	"Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", 
	"Oct", "Nov", "Dec"
};

const char *const week_str[DAYS_IN_A_WEEK] = {
	"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"
};

uint8_t daysInMM[MONTHS_IN_A_YEAR] = {
	31, 28, 31, 
	30, 31, 30, 
	31, 31, 30, 
	31, 30, 31
};

union _ui_rtcDateTime_u {
	unsigned byte[MAX_FIELDS];
	struct {
		unsigned YY;
		unsigned MM;
		unsigned DD;

		unsigned hh;
		unsigned mm;
		unsigned ss;
	};	
};

static union _ui_rtcDateTime_u _temp;

const uint32_t year_mask = 0xFC000000;
const uint32_t month_mask = 0x03C00000;
const uint32_t day_mask = 0x003E0000;

const uint32_t hour_mask = 0x0001F000;
const uint32_t min_mask = 0x00000FC0;
const uint32_t sec_mask = 0x0000003F;

void convToCompactTime(rtcDateTime_t *t)
{
	t->YY = (uint8_t)_temp.YY;
	t->MM = (uint8_t)_temp.MM;
	t->DD = (uint8_t)_temp.DD;

	t->hh = (uint8_t)_temp.hh;
	t->mm = (uint8_t)_temp.mm;
	t->ss = (uint8_t)_temp.ss;
}

void cpyToTempTime(rtcDateTime_t *t)
{
	_temp.YY = t->YY;
	_temp.MM = t->MM;
	_temp.DD = t->DD;

	_temp.hh = t->hh;
	_temp.mm = t->mm;
	_temp.ss = t->ss;
}


int addDaysToRTCDateTime(rtcDateTime_t *t, int daysToAdd)
{
	while (daysToAdd--) {
		t->DD++;
		if (t->DD <= DAYS_IN_MN_OF_YR(t->MM, t->YY))
			continue;
		t->DD = 1;
		if ((++t->MM) < MONTHS_IN_A_YEAR)
			continue;
		t->MM = 0;
		t->YY++;
	}
	return 0;
}

int subDaysToRTCDateTime(rtcDateTime_t *t, int daysToAdd)
{
	int DD = t->DD;
	int MM = t->MM;
	int YY = t->YY;
	while (daysToAdd++ < 0) {
		DD--;
		if (DD > 0)
			continue;
		if ((--MM) < 0) {
			MM = 11;
			YY--;
		}
		DD = DAYS_IN_MN_OF_YR(MM, YY);
	}
	t->DD = DD;
	t->MM = MM;
	t->YY = YY;
	return 0;
}


int addSecToRTCDateTime(rtcDateTime_t *t, int32_t sec)
{
	int32_t s = t->ss;

	s += sec;
	t->ss = s % 60;
	s /= SECS_IN_A_MIN;

	s += t->mm;
	t->mm = s % MINS_IN_A_HOUR;
	s /= MINS_IN_A_HOUR;

	s += t->hh;
	t->hh = s % HOURS_IN_A_DAY;
	s /= HOURS_IN_A_DAY;

	int daysToAdd = s;
	if (daysToAdd >= 0)
		return addDaysToRTCDateTime(t, daysToAdd);
	return subDaysToRTCDateTime(t, daysToAdd);
}

int timeCmp(const rtcDateTime_t *t1, const rtcDateTime_t *t2)
{
	return memcmp(t1, t2, sizeof(rtcDateTime_t));
}

uint32_t daysSinceEpoch(rtcDateTime_t *t)
{
	uint32_t days = 0;
	int i;
	for (i=RTCDATE2001JAN01.YY; i < t->YY; i++)
		days += isLeapYear(i) ? 366 : 365;
	for (i=RTCDATE2001JAN01.MM; i<t->MM; i++)
		days += DAYS_IN_MN_OF_YR(i, t->YY);
	days += (t->DD - RTCDATE2001JAN01.DD);
	return days;
}


int32_t timeDifference(rtcDateTime_t *t1, rtcDateTime_t *t2)
{
	int32_t sec = t1->ss - t2->ss;
	sec += (t1->mm - t2->mm) * SECS_IN_A_MIN;
	sec += (t1->hh - t2->hh) * MINS_IN_A_HOUR * SECS_IN_A_MIN;

	int daysToAdd = daysSinceEpoch(t1) - daysSinceEpoch(t2);
	
	sec += (daysToAdd * HOURS_IN_A_DAY * MINS_IN_A_HOUR * SECS_IN_A_MIN);
	return sec;
}


int print_time_gmt(rtcDateTime_t *rtc)
{
	return printf("%02u %s 20%02u %02u:%02u:%02u GMT\n", (int)rtc->DD, month_str[rtc->MM], 
		(int)rtc->YY,  (int)rtc->hh,  (int)rtc->mm,  (int)rtc->ss);
}

void getTimeCStr(char *dest, rtcDateTime_t *t)
{
	snprintf(dest, MAX_TIME_STR_LEN, "%02u %s 20%02u %02u:%02u:%02u GMT\n", (int)t->DD, month_str[t->MM], 
		(int)t->YY,  (int)t->hh,  (int)t->mm,  (int)t->ss);
}


uint32_t getPackedTime(rtcDateTime_t *t)
{
	rtcPackedTime_t pT;
	pT.ss = t->ss;
	pT.mm = t->mm;
	pT.hh = t->hh;

	pT.DD = t->DD;
	pT.MM = t->MM;
	pT.YY = t->YY;

	return pT.packedTime;
}

void unpackPackedTime(rtcDateTime_t *t, uint32_t packedTime)
{
	rtcPackedTime_t pT;
	pT.packedTime = packedTime;

	t->ss = pT.ss;
	t->mm = pT.mm;
	t->hh = pT.hh;

	t->DD = pT.DD;
	t->MM = pT.MM;
	t->YY = pT.YY;
}

uint16_t getSecondsSinceHour(rtcDateTime_t *t)
{
	return ((t->mm*SECS_IN_A_MIN) + t->ss);
}

uint32_t getSecondsSinceMidnight(rtcDateTime_t *t)
{
	return ((t->hh * MINS_IN_A_HOUR + t->mm) * SECS_IN_A_MIN + t->ss);
}

void timeObjFromCStr(rtcDateTime_t *t, const char *timeStr)
{
	cpyToTempTime(t);
	char month[10];
	sscanf(timeStr, "%02u %s 20%02u %02u:%02u:%02u GMT\n", &_temp.DD, month, &_temp.YY, &_temp.hh, 
		&_temp.mm, &_temp.ss);

	int i;
	for (i=0; i<MONTHS_IN_A_YEAR; i++)
		if (!strncmp(month, month_str[i], 3))
			break;
	if (i >= MONTHS_IN_A_YEAR)
		return;
	_temp.MM = i;
	convToCompactTime(t);
	return;
}

int timeFormatCheck(rtcDateTime_t *t)
{
	if (t->YY >= MAX_SUPPORTED_YR)
		return -1;
	if (t->MM >= MONTHS_IN_A_YEAR)
		return -1;
	if (t->DD > DAYS_IN_MN_OF_YR(t->MM, t->YY))
		return -1;

	if ((t->ss >= SECS_IN_A_MIN) || (t->mm >= MINS_IN_A_HOUR) || (t->hh >= HOURS_IN_A_DAY))
		return -1;
	return 0;
}

#define _MAIN_

#ifdef _MAIN_
#include <unistd.h>

int main()
{
	rtcDateTime_t t = {16, 0, 1, 0, 0, 0};
	rtcDateTime_t t2 = t;
	while (1) {
		print_time_gmt(&t);
		addSecToRTCDateTime(&t, 141091686);
		uint32_t pack = getPackedTime(&t);
		unpackPackedTime(&t, pack);
		print_time_gmt(&t);
		//addSecToRTCDateTime(&t, -141091686);
		//print_time_gmt(&t);
		printf("After comparing - %d\n", timeCmp(&t, &t));
		usleep(0);
		printf("Time Diff - %d\n", timeDifference(&t, &t2));
		return 0;
	}
	return 0;
}
#endif

