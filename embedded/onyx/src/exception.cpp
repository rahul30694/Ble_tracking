#include "exception.h"

#include <stdio.h>


xstd :: exception :: exception() noexcept
{

}

xstd :: exception :: exception(const exception &e) noexcept
{
	strcpy(exception_info, e.exception_info);
}

exception& xstd :: exception :: operator = (const exception &e) noexcept
{
	strcpy(exception_info, e.exception_info);
}

xstd :: exception :: ~exception()
{

}

const char * xstd :: exception :: what() noexcept
{
	printf("%s\n", exception_info);
}