#pragma once

#include "stdio.h"
#include "stdlib.h"
#include <cstring>
#include <iosfwd>

/*
2017-10-18	DBJ created

  DBJ CRT (C Run Time) is inside top level dbj namespace
*/
namespace dbj {

	/* dbj.org avoid's macros as much as possible */

	static auto MIN = [](auto a, auto b) { return (((a) < (b)) ? (a) : (b)); };
	static auto MAX = [](auto a, auto b) { return (((a) > (b)) ? (a) : (b)); };
	template < typename T, size_t N > 
		constexpr size_t countof(T const (&array)[N]) { return N; }
	/*
	in here we cater for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t
	*/
	/*
	(c) 2017 by dbj.org
	"zero" time modern C++ versions of str(n)len
	this should speed up any modern C++ code ... perhaps quite noticeably

	standard pointer versions of strlen and strnlen are not used by these two
	which are for arrays only
	note: str(n)len is charr array length  -1 because it does not count the null byte at the end
	note: strnlen is a GNU extension and also specified in POSIX (IEEE Std 1003.1-2008). 
	If strnlen is not available for char arrays
	use the dbj::strnlen replacement.
	*/
	template<typename T, size_t N>
	static inline size_t strnlen(
		const T(&carr)[N], 
		const size_t & maxlen,
		typename 
		std::enable_if<
		std::is_same<T, char>::value || 
		std::is_same<T, wchar_t>::value ||
		std::is_same<T, char16_t>::value ||
		std::is_same<T, char32_t>::value 
		>::type * = 0) 
	{
		return MIN(N, maxlen) - 1;
	}
	/*
	strnlen is a GNU extension and also specified in POSIX(IEEE Std 1003.1 - 2008).
	If std::strnlen is not available, which can happen when such extension is not supported,
	use the following replacement for charcter arrays.

	note: iosfwd include file contains char_traits we need
	*/
	template<typename T, size_t N>
	static inline size_t strlen(
		const T(&carr)[N],
		typename
			std::enable_if<
			std::is_same<T, char>::value ||
			std::is_same<T, wchar_t>::value ||
			std::is_same<T, char16_t>::value ||
			std::is_same<T, char32_t>::value
			>::type * = 0) 
		{
			return N - 1;
		}
	/*
	Pointer to character arrays support
	std lib defines strlen for char * and wchr_t *
	note: iosfwd include file contains char_traits we need
	*/
	static inline size_t strlen(const char *    cp) { return std::strlen(cp); }
	static inline size_t strlen(const wchar_t * cp) { return std::wcslen(cp);  }
	static inline size_t strlen(const char16_t * cp) { return std::char_traits<char16_t>::length(cp);}
	static inline size_t strlen(const char32_t * cp) { return std::char_traits<char32_t>::length(cp); }

	static inline size_t strnlen(const char * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<char>::length(cp);
		return (cpl > maxlen ? maxlen : cpl);
	}
	static inline size_t strnlen(const wchar_t * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<wchar_t>::length(cp);
		return (cpl > maxlen ? maxlen : cpl);
	}
	static inline size_t strnlen(const char16_t * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<char16_t>::length(cp);
		  return ( cpl > maxlen ? maxlen : cpl );
	}
	static inline size_t strnlen(const char32_t * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<char32_t>::length(cp);
		return (cpl > maxlen ? maxlen : cpl);
	}
} // dbj

/* standard suffix for every header here */
#define DBJVERSION __DATE__ __TIME__
#pragma message( "--------------> Compiled: " __FILE__ ", Version: " DBJVERSION)
#pragma comment( user, "(c) " STR(__DATE__ +7) " by dbj@dbj.org | Version: " DBJVERSION ) 
#undef DBJVERSION
/*
Copyright 2017 by dbj@dbj.org

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/*
The only requirement for this file is not to have any #include's
*/
