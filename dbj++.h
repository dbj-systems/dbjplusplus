#pragma once
/*
 see the licence blurb at eof
*/

#ifndef _MSC_VER
#error This code requires Visual C++ 
#endif // !_MSC_VER
#if _MSC_VER < 1911
#error This code requires Visual C++ 14.1 or better
#endif

#ifndef UNICODE
#error dbj++ requires UNICODE builds
#endif

#ifdef _SCL_SECURE_NO_WARNINGS
#error dbj++ can not be used with  _SCL_SECURE_NO_WARNINGS defined
#endif

// avoid min/max macros 
#define NOMINMAX

// DBJ_WIN to be defined on CL command line
// or not
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
#endif DBJ_WIN

#if 0
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <algorithm>
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

#ifndef _HAS_CXX17
#error __FILE__ requires C++17
#endif

#endif

/*
dbj++ begins here

this should be merged into dbj_crt.h
*/
#ifndef DBJ_STRINGIFY
	#define DBJ_STRINGIFY(s) # s
	#define DBJ_EXPAND(s) DBJ_STRINGIFY(s)
	#define DBJ_CONCAT_IMPL( x, y ) x##y
	#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )
	
	#define DBJ_COMPANY "DBJ.Systems Ltd."
	#define DBJ_YEAR ( __DATE__ + 7 ) 
	#define DBJ_BUILD_STAMP "(c) " __DATE__ " by " DBJ_COMPANY "| Version: [" __DATE__ "][" __TIME__ "]" 
#endif


/*


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

#ifndef DBJ_UNUSED
/*
this is for variables only
example 
long DBJ_UNUSED(var) {42L} ;
after expansion:
long var [[maybe_unused]] {42L} ;
*/
#define DBJ_UNUSED(x) x [[maybe_unused]]
#endif

/*
also defines DBJ_ASSERT, DBJ_VERIFY
and DBJ::TRACE
*/			 
#include "dbj_crt.h"
#include "dbj_synchro.h"
#include "dbj_string_util.h"
#include "dbj_util.h"
#include "dbj_string_compare.h"
#include "dbj_commander.h"
#include "dbj_console_painter.h"
#include "dbj_console.h"
#include "dbj_testing_interface.h"
#include "dbj_startend.h"
#include "dbj_com.h"
#include "dbj_defval.h"
#include "dbj_win32.h"
#include "dbj_native_arr_ref.h"
#include "dbj_app_env.h"


/* 
standard suffix for every dbj header 
*/
#pragma comment( user, DBJ_BUILD_STAMP ) 
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

