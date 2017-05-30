#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <stdint.h>
#include "compiler_abstraction.h"

void assert_callback(uint16_t line_num, const uint8_t *file_name);


#define ASSERT(expr) \
if (expr)                                                                     \
{                                                                             \
}                                                                             \
else                                                                          \
{                                                                             \
  assert_callback((uint16_t)__LINE__, (uint8_t *)__FILE__);               \
}


__WEAK void assert_callback(uint16_t line_num, const uint8_t *file_name);


#endif
