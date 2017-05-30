#include "systemlogline.h"


#define __MAIN__


#ifdef __MAIN__

int main()
{
	init_logline("onyx", false, true, false);
	mask_logline_upto(LOG_DEBUG);
	LOGLINE_EMERGENCY("Hello World");

	close_logline();
}

#endif