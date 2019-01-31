#pragma once

/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"
#include "./win/dbj_win_inc.h"

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

#if (!defined(UNICODE)) || (! defined(_UNICODE))
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
this is for variables only
example
long DBJ_MAYBE(var) {42L} ;
after expansion:
long var [[maybe_unused]] {42L} ;
*/
#define DBJ_MAYBE(x) x [[maybe_unused]]

/*
for variables and expressions
guaranteed no evaluation
guaranteed zero bytes overhead
standard c++ 
works in any space
https://godbolt.org/z/jGC98L
*/
#define DBJ_NOUSE(...) static_assert( (noexcept(__VA_ARGS__),true) );

// #define DBJ_VANISH(...)

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
#include "./core/dbj++core.h"
//
#include "./err/dbj_err.h"
//
#include "./util/dbj++util.h"
#include "./console/dbj++con.h"
#include "./testfwork/dbj_testing_interface.h"
#include "./numeric/dbj_numeric.h"
#include "./win/dbj++win.h"

#pragma warning( pop )

/* standard suffix for every dbj header */
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