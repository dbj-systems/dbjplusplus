#pragma once

/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"

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
// #ifdef _MSC_VER
#ifdef __cpp_coroutines
#pragma message ( __FILE__ " -- coroutines available in this build")
#else
#pragma message (__FILE__ " -- no coroutines in this build ...")
#endif
/*
#ifdef __cpp_lib_is_final
#define DBJ_FINAL final
#else
#define DBJ_FINAL
#endif
*/
//
#ifdef __cplusplus
/*
this is for variables only
example
long DBJ_MAYBE(var) {42L} ;
after expansion:
long var [[maybe_unused]] {42L} ;
*/
#define DBJ_MAYBE(x) x [[maybe_unused]]
#define DBJ_NOUSE(...) (void)noexcept(ST(__VA_ARGS__))
// for variables and expressions
// guaranteed no evaluation
// guaranteed zero bytes overhead
// standard c++ 
// works in any space
// https://godbolt.org/z/jGC98L
#define DBJ_VANISH(...) static_assert( (noexcept(__VA_ARGS__),true) );
#else
// inferior macro but also works with C projects
#define DBJ_NOUSE(x) static void * dummy___COUNTER__ = (void*)sizeof(x)
#endif

#ifdef DBJ
#error "#define DBJ ::dbj already defined"
#else
#define DBJ ::dbj
#endif

/*
in case you need/want COM init 
but beware: there should be only one per app
and thread model must be always the same if 
there is another one

#define DBJ_COMAUTOINIT
*/
/*
https://stackoverflow.com/questions/46891586/how-to-disable-visual-studio-warning-c4244-for-stdvector-copy-or-assign-with-i
*/
#pragma warning( push )
#pragma warning( disable : 4244 )

// #define DBJ_MT
// Multi Threaded Build
// used in dbj_synchro to 
// conditionaly def/undef
// DBJ_AUTO_LOCK
// TODO: not implemented yet
#include "dbj_synchro.h"
#include "dbj_constexpr.h"
#include "dbj_error_concept.h"
#include "dbj_crt.h"
#include "dbj_array.h"
#include "dbj_string_util.h"
#include "dbj_util.h"
#include "dbj_uuid.h"
#include "dbj_string_compare.h"
#include "dbj_commander.h"
// #include "dbj_console_painter.h"
#include "./console/dbj_console.h"
#include "./console/dbj_console_ops.h"
#include "./testfwork/dbj_testing_interface.h"
#include "dbj_startend.h"
#include "dbj_defval.h"
#include "dbj_win32.h"
#include "dbj_native_arr_ref.h"
#include "dbj_app_env.h"
#include "dbj_kalends.h"
#include "dbj_tokenizer.h"
#include "dbj_key_value_store.h"
#include "dbj_main.h"
#include "dbj_optional_handler.h"

#pragma warning( pop )

/* 
standard suffix for every dbj header 
*/
#pragma comment( user, DBJ_BUILD_STAMP ) 

#if 0
/*
2018 MAY 31
Bellow is a solution for call once mechanism in MSVC
this is here as it was not certain how to achieve this using
the current version of MSCV compiler

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