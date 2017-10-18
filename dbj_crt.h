#pragma once

#include "stdio.h"
#include "stdlib.h"
#include <cstring>

/*
2017-10-18	DBJ created

  DBJ CRT (C Run Time) is inside top level anonymous namespace
  top level anonymous namespace makes symbols inside not globals but
  stlil visible to client code as if it is global.
  your compiler must have very easy time discarding the compete namespace
  if nothing is used from it.
*/
namespace {

	/* dbj.org avoid's macros as much as possible */

	auto MIN = [](auto a, auto b) { return (((a) < (b)) ? (a) : (b)); };
	auto MAX = [](auto a, auto b) { return (((a) > (b)) ? (a) : (b)); };
	template < typename T, size_t N > constexpr size_t countof(T const (&array)[N]) { return N; }
	/*
	(c) 2017 by dbj.org
	"zero" time modern C++ versions of str(n)len
	this should speed up any modern C++ code ... perhaps quite noticeably

	standard char * versions of strlen and strnlen are not used by these two

	note: str(n)len is charr array length  -1 because it does not count the null byte at the end
	*/
	template<size_t N>
	static inline size_t strnlen(const char(&carr)[N], const size_t & maxlen) {
		return MIN(N, maxlen) - 1;
	}

	template<size_t N>
	static inline size_t strlen(const char(&carr)[N]) {
		return N - 1;
	}

} // eof global anonymous namespace
#if 0
#ifdef DBJ_TESTING_EXISTS
DBJ_TEST_UNIT("dbj crt") {
	printf("\n(c) %s by dbj.org\t\tcompiler is GCC [%s]\n", __YEAR__, __VERSION__);

	char prompt[] = "0123456789";
	char * promptPTR = &prompt[0];

	// char arrays are using dbj.org versions     
	printf("\nfor char array [%s]\tstrlen(): [%zu]\t countof(): %zu", prompt, strlen(prompt), countof(prompt));
	printf("\nfor char array [%s]\tstrlen(): [%zu]\t countof(): %zu", prompt, strnlen(prompt, BUFSIZ), countof(prompt));

	// using CRT strlen for char *
	printf("\nchar pointer strlen  of [%s] is: %zu", prompt, strlen(promptPTR));
	// using CRT strnlen for char *
	// note: std has no  strnlen ... yet
	printf("\nchar pointer strnlen of [%s] is: %zu", prompt, strnlen(promptPTR, BUFSIZ));

}
#endif // DBJ_TESTING_EXISTS
#endif // 0
/*
*/
/* standard suffix for every header here */
#define DBJVERSION __DATE__ __TIME__
#define __YEAR__ STR(__DATE__ +7)
#pragma message( "--------------> Compiled: " __FILE__ ", Version: " DBJVERSION)
#pragma comment( user, "(c) " STR(__YEAR__) " by dbj@dbj.org | Version: " DBJVERSION ) 
#undef __YEAR__
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
