#include "assert.h"



void assert_callback(uint16_t line_num, const uint8_t * file_name)
{
    (void) file_name; /* Unused parameter */
    (void) line_num;  /* Unused parameter */
 
    while (1) ;
}

