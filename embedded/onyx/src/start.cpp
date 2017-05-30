#include "main.h"
#include "esp12.h"
#include "time.h"


void setup(char *port)
{
	esp12_init(port);
}


void loop_fast()
{
	esp12_basic_configure();
	esp12_update();
}

void loop_1Hz()
{

}

void loop_2Hz()
{

}

void loop_10Hz()
{

}