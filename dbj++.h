#pragma once

#ifndef UNICODE
#error __FILE__ requires UNICODE builds
#endif

#ifndef _MSC_VER
#error This code requires Visual C++ 
#else
#define _VERSION__ _MSC_VER
#endif // !_MSC_VER

#if _MSC_VER < 1911
#error This code requires Visual C++ 14.1 or better
#endif
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <array>
#include <vector>
/*
dbj begins here
*/
/*  __interface msvc keyword
explained here : https://docs.microsoft.com/en-us/cpp/cpp/interface
*/
#define implements public
// inline is the keyword, in C++ and C99.
// #define DBJ_INLINE inline
#define DBJ_INLINE static __forceinline 

#ifdef __cpp_lib_is_final
#define DBJ_FINAL final
#else
#define DBJ_FINAL
#endif

#define STR(X) #X

// Taken from MODERN v1.26 - http://moderncpp.com
// Copyright (c) 2015 Kenny Kerr
#pragma region Independent debug things
#ifdef _DEBUG
#define DBJ_ASSERT assert
#define DBJ_VERIFY_(result, expression) DBJ_ASSERT(result == expression)

template <typename ... Args>
DBJ_INLINE void DBJ_TRACE(wchar_t const * const message, Args ... args) noexcept
{
	wchar_t buffer[512] = {};
	assert(-1 != _snwprintf_s(buffer, 512, 512, message, (args) ...));
	::OutputDebugStringW(buffer);
}

template <typename ... Args>
DBJ_INLINE void DBJ_TRACE(const char * const message, Args ... args) noexcept
{
	wchar_t buffer[512] = {};
	assert(-1 != _snprintf_s(buffer, 512, 512, message, (args) ...));
	::OutputDebugStringA(buffer);
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
	constexpr auto COMPANY = "dbj.systems Ltd."; // your company here
	constexpr auto YEAR = (__DATE__ + 7);
}

#include "dbj_crt.h"
#include "dbj_testing.h"
#include "dbj_startend.h"
#include "dbj_com.h"
#include "dbj_console.h"
#include "dbjio.h"

/* standard suffix for every header here */
#define DBJVERSION __DATE__ __TIME__
#pragma message( "--------------> Compiled: " __FILE__ ", Version: " DBJVERSION)
#pragma comment( user, "(c) " STR(__YEAR__) " by dbj@dbj.org | Version: " DBJVERSION ) 
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

