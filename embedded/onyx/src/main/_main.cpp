#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <signal.h>
#include <unistd.h>
#include <pthread.h>


#define TURN_ON_SCALING_FREQ

void terminate(int n);


void main_setup()
{
	
}

void main_task()
{
	
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

	main_setup();

	while (1) {

	#ifdef TURN_ON_SCALING_FREQ
		//scaleDownToFreq(CPU_FREQ);
	#endif

		main_task();
		event();
	}
	
	return 0;
}


void terminate(int n)
{
	std :: cout<<"-----------Terminated-------------"<< std :: endl;
	exit(1);
}

