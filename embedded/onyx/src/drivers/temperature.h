#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

int mount_temperature_sensor(void);
int get_temperature(float &t);
void umount_temperature_sensor(void);
char *get_temperature_cstr(void);

#endif