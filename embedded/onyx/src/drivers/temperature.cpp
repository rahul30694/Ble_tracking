#include "temperature.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>

std :: ifstream tFile;

int mount_temperature_sensor(void)
{
	tFile.open("/sys/class/thermal/thermal_zone0/temp");
	if (tFile.fail())
		return -1;
	return 0;
}

int get_temperature(float &t)
{
	if (!tFile.is_open())
		return -1;
	tFile.seekg(0, tFile.beg);

	int temp;
	tFile >> temp;
	t = temp / 1000;
	return 0;
}

#define MAX_TEMPERATURE_CSTR_LEN					20
char __temperature_cstr[MAX_TEMPERATURE_CSTR_LEN];
char *get_temperature_cstr(void)
{
	*__temperature_cstr = '\0';
	float t;
	if (get_temperature(t))
		goto RET;
	snprintf(__temperature_cstr, MAX_TEMPERATURE_CSTR_LEN, "%.2f C", t);
RET:
	return __temperature_cstr;
}

void umount_temperature_sensor(void)
{
	if (!tFile.is_open())
		return;
	tFile.close();
}


int main()
{
	if (mount_temperature_sensor()) {
		std :: cerr << "Failed in mounting sensor";
		return -1;
	}

	while (1) {
		float temp;
		if (get_temperature(temp))
			continue;
		std :: cout << "Temp is " << get_temperature_cstr() << std :: endl;
		sleep(2);
	}
}
