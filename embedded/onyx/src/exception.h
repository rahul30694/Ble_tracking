#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#define MAX_EXCEPTION_INFO_LEN			20

namespace xstd {
	class exception {

		char exception_info[MAX_EXCEPTION_INFO_LEN];
	public:
		exception() noexcept;
		exeception(const exception &) noexcept;

		exception & operator=(const exception &) noexcept;

		virtual ~exception();
		virtual const char *what() const noexcept;
	};
} 

#endif