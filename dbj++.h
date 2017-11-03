#pragma once
#ifndef _MSC_VER
#error This code requires Visual C++ 
#endif // !_MSC_VER
#if _MSC_VER < 1911
#error This code requires Visual C++ 14.1 or better
#endif

#ifndef UNICODE
#error __FILE__ requires UNICODE builds
#endif

#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <variant>

#if _MSC_VER > 1911
/*This code requires Visual C++ 14.1 or better*/
		#ifndef _HAS_CXX17
			#error __FILE__ requires C++17
		#endif
#endif

#ifndef DBJ_STRINGIFY
#define DBJ_STRINGIFY(s) # s
#define DBJ_EXPAND(s) DBJ_STRINGIFY(s)
#endif
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif




#ifndef __YEAR__
// Example of __DATE__ string: "Jul 27 2012"
//                              01234567890
#define __YEAR__ (__DATE__ + 7)
namespace {
	constexpr const char YEAR[] = { __YEAR__[0],__YEAR__[1],__YEAR__[2],__YEAR__[3] };
}
#endif

/*
dbj begins here

__interface msvc keyword
explained here : https://docs.microsoft.com/en-us/cpp/cpp/interface
*/
#define implements public
// inline is the keyword, in C++ and C99.
#define DBJ_INLINE inline 

#ifdef __cpp_lib_is_final
#define DBJ_FINAL final
#else
#define DBJ_FINAL
#endif


// Inspired by MODERN v1.26 - http://moderncpp.com
#pragma region Independent debug things
#ifdef _DEBUG
#define DBJ_ASSERT assert
#define DBJ_VERIFY_(result, expression) DBJ_ASSERT(result == expression)

namespace {
	template <typename ... Args>
	DBJ_INLINE void DBJ_TRACE(wchar_t const * const message, Args ... args) noexcept
	{
		/* NOTE: 512 happpens to be the BUFSIZ */
		wchar_t buffer[512] = {};
		assert(-1 != _snwprintf_s(buffer, 512, 512, message, (args) ...));
		::OutputDebugStringW(buffer);
	}

	template <typename ... Args>
	DBJ_INLINE void DBJ_TRACE(const char * const message, Args ... args) noexcept
	{
		char buffer[512] = {};
		assert(-1 != _snprintf_s(buffer, 512, 512, message, (args) ...));
		::OutputDebugStringA(buffer);
	}
}
#else
// code dissapears
#define DBJ_ASSERT __noop
// code stays
#define DBJ_VERIFY(expression) (expression)
// code stays
#define DBJ_VERIFY_(result, expression) (expression)
// code dissapears
#define DBJ_TRACE __noop
#endif
#pragma endregion

namespace dbj {
	constexpr auto COMPANY = "DBJ.Systems Ltd."; 
	constexpr auto YEAR = (__DATE__ + 7);
}

#include "dbj_crt.h"
#include "dbj_testing.h"
#include "dbj_commander.h"
#include "dbj_console_painter.h"
#include "dbj_console.h"
#include "dbj_testing_interface.h"
#include "dbj_startend.h"
#include "dbj_com.h"
#include "dbjio.h"
#include "dbj_defval.h"
#include "dbj_win32.h"

/* standard suffix for this header only every header here */
#pragma message( __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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

