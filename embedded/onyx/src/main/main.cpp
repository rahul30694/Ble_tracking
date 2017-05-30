#include "main.h"
#include "time.h"
#include "config.h"
#include "system.h"
#include "serial.h"
#include "esp12.h"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>

using namespace std;

#define TURN_ON_SCALING_FREQ

void terminate(int n);


struct sysConfig_s new_cfg = {
	{
		"LemonDog",
		"WWL001",
		"v1.0",

		"WWL001_ap",
		"123456",

		"SiddharthNet",
		"siddharth",

		{{ 1, 2, 3, 4 }},
		80,

		{{ 0, 0, 0, 0 }},
		0
	},
	0
};

void sys_setup(char *port)
{
	sysConfigInit();
	sysParamInit();
	calculateChecksum(&new_cfg);

	if (loadConfig(&new_cfg)) {
		serialPrintln("Failed to Load Config");
		terminate();
	}
	printSysCfg();

	setup(port);
	delay(100);
}

void sys_loop()
{
	unsigned long _ms = millis();

	static unsigned long _1Hz_interval;
	if (_ms - _1Hz_interval >= 1000) {
		loop_1Hz();
		_1Hz_interval = _ms;
	}

	static unsigned long _2Hz_interval;
	if (_ms - _2Hz_interval >= 500) {
		loop_2Hz();
		_2Hz_interval = _ms;
	}

	static unsigned long _10Hz_interval;
	if (_ms - _10Hz_interval >= 100) {
		loop_10Hz();
		_10Hz_interval = _ms;
	}

	loop_fast();
}

int _init_()
{
	signal(SIGTSTP, SIG_DFL);
	return 0;
}


void event()
{

}

int main(int argc, char *argv[])
{
	if (_init_()) return -1;

	sys_setup(argv[1]);

	while (1) {

	#ifdef TURN_ON_SCALING_FREQ
		scaleDownToFreq(CPU_FREQ);
	#endif

		sys_loop();
		event();
	}
	
	return 0;
}


void terminate(int n)
{
	cout<<"-----------Terminated-------------"<<endl;
	exit(1);
}

