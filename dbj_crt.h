#pragma once

// avoid min/max macros 
#define NOMINMAX
#include <sysinfoapi.h>
#include <strsafe.h>

#include <cwctype>
#include <cctype>


#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <string_view>

namespace dbj {

	// the most used 4 types
	using wstring = std::wstring;
	using wstring_vector = std::vector<std::wstring>;

	using string = std::string;
	using string_vector = std::vector<std::string>;


	// std::equal has many overloads
	// it is less error prone to have it here
	// and use this one as we exactly need
	template<class InputIt1, class InputIt2>
	bool equal_(InputIt1 first1, InputIt1 last1, InputIt2 first2)
	{
		for (; first1 != last1; ++first1, ++first2) {
			if (!(*first1 == *first2)) {
				return false;
			}
		}
		return true;
	}

	/// <summary>
	/// http://en.cppreference.com/w/cpp/types/alignment_of
	/// please make sure alignment is adjusted as 
	/// necessary
	/// after calling these functions
	/// </summary>
	namespace heap {

		template<typename T>
		inline T * heap_alloc(size_t size_ = 0) {
			T * rez_ = new T;
			_ASSERTE(rez_ != nullptr);
			return rez_;
		}

		template<typename T>
		inline bool heap_free(T * ptr_) {
			_ASSERTE(ptr_);
			delete ptr_;
			ptr_ = nullptr;
			return true;
		}

#ifdef DBJ_WIN

		template<typename T>
		inline T * heap_alloc_win(size_t size_ = 0) {
			T * rez_ = (T*)CoTaskMemAlloc(sizeof(T));
			_ASSERTE(rez_ != nullptr);
			return rez_;
		}

		template<typename T>
		inline bool heap_free_win(T * ptr_) {
			_ASSERTE(ptr_);
			CoTaskMemFree((LPVOID)ptr_);
			ptr_ = nullptr;
			return true;
		}

#endif // DBJ_WIN

	}

	/*
	-------------------------------------------------------------------------
	dbj++ exception
	*/
	struct Exception : protected std::runtime_error {
	public:
		typedef std::runtime_error _Mybase;

		Exception(const std::string & _Message)
			: _Mybase(_Message.c_str())
		{	// construct from message string
		}

		Exception(const std::wstring & _WMessage)
			: _Mybase(
				std::string(_WMessage.begin(), _WMessage.end() ).data()
			)
		{	// construct from message unicode std string
		}


		Exception(const char *_Message)
			: _Mybase(_Message)
		{	// construct from message string
		}

		// virtual char const* what() const
		operator std::wstring () const {
			std::string s_(this->what());
			return std::wstring(s_.begin(), s_.end());
		}
	};
}

// DBJ namespace is different from dbj (lower case) namespace 
namespace DBJ {

	/* 512 happpens to be the BUFSIZ */
	constexpr size_t BUFSIZ_ = 512 * 2;

	// DBJ::TRACE exist in release builds too

	// this requires all the  args 
	// that are strings to be wide strings 
	// so I am doubtfull it will work ... easily
	template <typename ... Args>
	static void TRACE(wchar_t const * const message, Args ... args) noexcept
	{
		wchar_t buffer[DBJ::BUFSIZ_]{};
		_ASSERTE(-1 != _snwprintf_s(buffer, _countof(buffer), _countof(buffer), message, (args) ...));
		::OutputDebugStringW(buffer);
	}
	template <typename ... Args>
	static void TRACE(const char * const message, Args ... args) noexcept
	{
		char buffer[DBJ::BUFSIZ_]{};
		_ASSERTE(-1 != _snprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, (args) ...));
		::OutputDebugStringA(buffer);
	}
} // eof DBJ 

#pragma region DBJ debug things
#ifdef _DEBUG
#define DBJ_ASSERT _ASSERTE
#define DBJ_VERIFY(result, expression) DBJ_ASSERT(result == expression)
#else
// release code dissapears some things, not all
// be carefull with DBJ::TRACE as it might _ASSERTE in release builds
// 
#define DBJ_ASSERT 
// release code for DBJ_VERIFY_ stays but with no checks
#define DBJ_VERIFY(result, expression) (void)(expression)
#endif
#pragma endregion 

/*
2017-10-18	DBJ created
DBJ CRT (DBJ C++ Run Time) is inside top level dbj namespace
*/
namespace dbj {

	/*
	Core algo from http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign

	standard c++ will "do" the bellow for "anyt" type that 
	is applicable to required operators
	*/
	 inline auto  sign = [] (const auto & v) constexpr -> int {
		return (v > 0) - (v < 0); // -1, 0, or +1
	 };

	template< size_t N>
	__forceinline dbj::wstring wide(const char(&charar)[N])
	{
		return dbj::wstring(std::begin(charar), std::end(charar));
	}

	__forceinline dbj::wstring wide(const char * charP)
	{
		std::string_view cv(charP);
		return dbj::wstring(cv.begin(), cv.end());
	}

	/* if and when needed add
	template<size_t N> 	__forceinline std::wstring narrow(const wchar_t(&charar)[N]);
	__forceinline std::wstring narrow(const wchar_t * charP);
	*/

	/* avoid macros as much as possible */

	inline auto MIN = [](auto a, auto b) { return (((a) < (b)) ? (a) : (b)); };
	inline auto MAX = [](auto a, auto b) { return (((a) > (b)) ? (a) : (b)); };

	template < typename T, size_t N > 
	  inline constexpr size_t 
		countof(T const (&array)[N]) { return N; }

	/*
	in here we cater for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t
	*/
/*
Inspired by: https://opensource.apple.com/source/bash/bash-80/bash/lib/sh/strnlen.c
DBJ_INLINE size_t strnlen(const CHAR_T *s, size_t maxlen)
{
	const CHAR_T *e = {};
	size_t n = {};

	for (e = s, n = 0; *e && n < maxlen; e++, n++)
		;
	return n;
}
*/

/*
(c) 2017, 2018 by dbj.org
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
	Pointer (to character arrays) support
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

/* 
DBJ C++RT utilities
*/
namespace dbj {


	/*
	Tame the full path filenames  in __FILE__
	Inspired with:	https://stackoverflow.com/questions/8487986/file-macro-shows-full-path/8488201#8488201
	Usage:
	#error dbj::nicer_filename(__FILE__) " has a problem."
	TODO: is wide version necessary?
	*/
	static __forceinline 
		constexpr auto nicer_filename(const char * filename) {
		return (strrchr(filename, '\\') ? strrchr(filename, '\\') + 1 : filename);
	}


	/*
	Schurr_cpp11_tools_for_class_authors.pdf

	constexpr str_const my_string = "Hello, world!";
	static_assert(my_string.size() == 13, "");
	static_assert(my_string[4] == 'o', "");
	constexpr str_const my_other_string = my_string;
	static_assert(my_string == my_other_string, "");
	constexpr str_const world(my_string, 7, 5);
	static_assert(world == "world", "");
	//  constexpr char x = world[5]; // Does not compile because index is out of range!
	*/
	class str_const final { // constexpr string
		const char* const p_{ nullptr } ;
		const std::size_t sz_{ 0 };
	public:
		
		template<std::size_t N>
		constexpr str_const(const char(&a)[N]) : // ctor
			p_(a), sz_(N - 1) {		}

		constexpr char operator[](std::size_t n) const 
		{ 
			return n < sz_ ? this->p_[n] : throw std::out_of_range("");
		}
		
		constexpr std::size_t size() const noexcept { return this->sz_; }
	};

	/*
	text line of chars, usage
	constexpr c_line<80, '-'> L80;
	*/
	template <unsigned Size, char filler = ' '>
	class c_line final {
		mutable char array_[Size+1] = { filler };
	public:
		constexpr c_line() {
			int b = 0;
			while (b != (Size+1)) {
				array_[b] = filler; b++;
			}
			array_[Size] = '\x0';
		}
		constexpr operator const char * () const noexcept { return array_;  }
	};
} // dbj

 /* standard suffix for every other header here */

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

#if 0
// dbj: discarded as stunt
//www.highprogrammer.com/alan/windev/visualstudio.html
// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define Expander(x) (x)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") :DBJ Reminder: "
#endif