#include "core.h"

#include <stdio.h>

#define STAT_FILENAME			"/proc/stat"

/*
 * READ: http://www.linuxhowtos.org/System/procstat.htm?userrate=4
 *
 */

float stat_get(void)
{
	FILE *fp=fopen(STAT_FILENAME, "r");

	long int user, nice, sys, idle, iowait, irq, softirq;
	fscanf(fp, " cpu %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &sys, &idle, &iowait, &irq, &softirq);
	//printf(" cpu %ld %ld %ld %ld ", user, nice, sys, idle);

	fclose(fp);
	return 0;

}


//#define __MAIN__

#ifdef __MAIN__
int main()
{
	stat_get();
	return 0;
}
#endif