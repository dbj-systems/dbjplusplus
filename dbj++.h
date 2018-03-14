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

#ifdef _SCL_SECURE_NO_WARNINGS
#error dbj++ can not be used with  _SCL_SECURE_NO_WARNINGS defined
#endif

#ifdef DBJ_WIN
#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOMINMAX
#include <windows.h>
#endif

#ifndef _GDIPLUS_H
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")
#endif // _GDIPLUS_H
#else
#include <algorithm>
#endif DBJ_WIN

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
//strstream is deprecated use stringstream  instead
// #include <strstream> 
#include <sstream> 
#include <optional>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <variant>
#include <any>
#include <string_view>


#if _MSC_VER < 1911
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

namespace dbj {
	constexpr const char * COMPANY = "DBJ.Systems Ltd.";

	constexpr const char (&YEAR)[] { __DATE__[7],__DATE__[8],__DATE__[9],__DATE__[10], '\0' };
#if 0
	inline std::string  filename(const std::string  &  file_path) {
		auto pos = file_path.find_last_of('\\');
		return 
			( std::string::npos != pos 
			    ? file_path.substr( pos, file_path.size())
			  : file_path
			) ; 
	}
	inline std::string fileline ( const std::string & file_path, unsigned line_ , const std::string & suffix = 0 ) {
		return filename(file_path) + "(" + std::to_string(line_) + ")"
			  + ( suffix.empty() ? "" : suffix ) ;
	}
#endif
}

/*
dbj begins here

__interface msvc keyword
explained here : https://docs.microsoft.com/en-us/cpp/cpp/interface

we can't have #define implements because of the cppWINRT base.h struct of the same name
#define implements public
*/
// inline is the keyword, in C++ and C99.
#define DBJ_INLINE inline 

#ifdef __cpp_lib_is_final
#define DBJ_FINAL final
#else
#define DBJ_FINAL
#endif




#include "dbj_crt.h"
#include "dbj_testing.h"
#include "dbj_commander.h"
#include "dbj_console_painter.h"
#include "dbj_console.h"
#include "dbj_testing_interface.h"
#include "dbj_startend.h"
#include "dbj_com.h"
#include "dbj_defval.h"
#include "dbj_win32.h"



/*
 TODO: the whole dbj++ has to be configured somewhere?
*/
#ifndef DBJ_BUILD_STAMP
#define DBJ_BUILD_STAMP __FILE__ "(c) " __DATE__ " by dbj@dbj.org | Version: " __DATE__ __TIME__ 
/* standard suffix for every dbj header */
#pragma comment( user, DBJ_BUILD_STAMP ) 
#endif
/*
Copyright 2017,2018 by dbj@dbj.org

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

