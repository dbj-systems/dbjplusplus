#pragma once

// so windows.h it is
// for the time being
#define DBJ_WIN 

#ifdef DBJ_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif
#define STRICT
#define NOSERVICE
// avoid min/max macros 
#define NOMINMAX
#include <windows.h>
#endif
#endif DBJ_WIN

/*
 see the licence blurb at eof
*/

#ifndef __clang__
#ifndef _MSC_VER
#error dbj++  requires Visual C++ 
#endif // !_MSC_VER
#if _MSC_VER < 1911
#error dbj++ requires Visual C++ 14.1 or better
#endif
#endif

#if (WINVER < NTDDI_WIN10_RS3)
#else
#error dbj++ requires Windows builds above REDSTONE 3 or above
#endif

#ifndef UNICODE
#error dbj++ requires UNICODE builds
#endif

#ifdef _SCL_SECURE_NO_WARNINGS
#error dbj++ can not be used with  _SCL_SECURE_NO_WARNINGS defined
#endif

/*
__interface msvc keyword
explained here : https://docs.microsoft.com/en-us/cpp/cpp/interface

we can't have #define implements because of the 
cppWINRT base.h struct of the same name
so we can not do this:

#define implements public
*/

#ifdef __cpp_coroutines
#pragma message ("coroutines available")
#else
#pragma message ("no coroutines")
#endif

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

in case you need/want COM init 
but beware: there should be only one per app
and thread model must be always the same if 
there is another one

#define DBJ_COMAUTOINIT
*/

// #define DBJ_MT
// Multi Threaded Build
// used in dbj_synchro to 
// conditionaly def/undef
// DBJ_AUTO_LOCK
// TODO: not implmented yet
#include "dbj_synchro.h"

#include "dbj_crt.h"
#include "dbj_array.h"
#include "dbj_string_util.h"
#include "dbj_util.h"
#include "dbj_string_compare.h"
#include "dbj_commander.h"
#include "dbj_console_painter.h"
#include "dbj_console.h"
#include "dbj_console_ops.h"
#include "dbj_testing_interface.h"
#include "dbj_startend.h"
#include "dbj_com.h"
#include "dbj_defval.h"
#include "dbj_win32.h"
#include "dbj_native_arr_ref.h"
#include "dbj_app_env.h"
#include "dbj_micro_log.h"
#include "dbj_kalends.h"
#include "dbj_tokenizer.h"


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

/*
2018 MAY 31
Bellow is a solution for call once mechanism in MSVC
*/
#if 0
/*
- anonymous namespace implies default static linkage for variables inside
- worth repeating: this is safe in presence of multiple threads (MT) 
and is supported as a such by all major compilers
- inside is a lambda which is guaranteed to be called only once
- this pattern is also safe to use in header only situations
*/
namespace dbj_once {

	struct singleton final {};

	inline singleton & instance()
	{
		static singleton single_instance = []() -> singleton {
			// this is called only once
			// do some more complex initialization
			// here
#pragma message ("\compiling dbj_once 'call once' lambda ***************************************\n")
DBJ::TRACE("\nVisiting dbj_once 'call once' lambda ***************************************\n");
			return {};
		}();
		return single_instance;
	};

	inline singleton & singleton_middleton = instance();

 } // dbj_once
#endif