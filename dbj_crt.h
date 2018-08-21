#pragma once

// avoid min/max macros 
#define NOMINMAX
#include <sysinfoapi.h>
#include <strsafe.h>

#include <algorithm>
#include <cstddef>
#include <cwctype>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <string_view>

// NOTE: do not have a space after a macro name and before the '(' !!
#ifndef DBJ_STRINGIFY	
#define DBJ_STRINGIFY(s) # s
#define DBJ_EXPAND(s) DBJ_STRINGIFY(s)
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )
#endif

#ifndef DBJ_COMPANY	
#define DBJ_COMPANY "DBJ.Systems Ltd."
#define DBJ_YEAR ( __DATE__ + 7 ) 
#define DBJ_BUILD_STAMP "(c) " __DATE__ " by " DBJ_COMPANY "| Version: [" __DATE__ "][" __TIME__ "]" 
#endif
// 
#define DBJ_CHECK_IF static_assert

#pragma region DBJ debug things
#ifdef _DEBUG
#define DBJ_ASSERT _ASSERTE
#define DBJ_VERIFY(result, expression) DBJ_ASSERT((result) == (expression))
#else
// release code dissapears some things, not all
// be carefull with DBJ::TRACE as it might _ASSERTE in release builds
// 
#define DBJ_ASSERT __noop
// release code for DBJ_VERIFY_ stays but with no checks
#define DBJ_VERIFY(result, expression) (void)(expression)
#endif
#pragma endregion 

/*
DBJ preffered concept to code standard C++ string constants
is to use string_view

using namespace std::literals;

namespace dbj::strings {
	constexpr inline auto which_time() {
		return "compile time"sv;
	}
};

this even works at compile time

static_assert(dbj::strings::which_time() == "compile time" );

*/

// DBJ namespace is different from dbj (lower case) namespace 
namespace DBJ {

	using namespace std::literals;

	constexpr inline auto LINE    (){ return "--------------------------------------------------------------------------------"sv; };
	constexpr inline auto COMPANY (){ return "DBJ.Systems Ltd."sv; };
	constexpr inline auto YEAR    (){ return std::string_view{ (__DATE__ + 7) }; };

	/* 512 happpens to be the POSIX BUFSIZ? */
#ifdef BUFSIZ
	constexpr inline const size_t BUFSIZ_ = BUFSIZ * 2 * 4; // 4096
#else
	constexpr inline const size_t BUFSIZ_ = 512 * 2 * 4 ; // 4096
#endif

#pragma warning( push )
#pragma warning( disable: 4190 )
	/*
	transform path to filename
	delimeter is '\\'
	*/
	extern "C" {

		// inline std::string  FILENAME(const std::string & file_path, const char delimiter_ = '\\') {
		inline std::string  FILENAME(std::string_view file_path, const char delimiter_ = '\\') 
		{
			_ASSERTE(!file_path.empty());
			size_t pos = file_path.find_last_of(delimiter_);
			return
				(std::string::npos != pos
			? std::string{ file_path.substr(pos, file_path.size()) }
			: std::string{ file_path }
				);
		}

		/*
		usual usage :
		DBJ::FILELINE( __FILE__, __LINE__, "some text") ;
		*/
		// inline std::string FILELINE(const std::string & file_path,
		inline std::string FILELINE(std::string_view file_path,
			unsigned line_,
			const std::string & suffix = "")
		{
			_ASSERTE(!file_path.empty());
			return {
				FILENAME(file_path) + "(" + std::to_string(line_) + ")"
				+ (suffix.empty() ? "" : suffix)
			};
		}

	} // extern "C"
#pragma warning( pop )

	template <size_t BUFSIZ_ = DBJ::BUFSIZ_, typename ... Args>
	inline std::wstring printf_to_buffer(wchar_t const * const message, Args ... args) noexcept
	{
		wchar_t buffer[BUFSIZ_]{};
		auto R = _snwprintf_s(buffer, _countof(buffer), _countof(buffer), message, (args) ...);
		_ASSERTE(-1 != R);
		return {buffer};
	}

	template <size_t BUFSIZ_ = DBJ::BUFSIZ_, typename ... Args>
	inline std::string printf_to_buffer(const char * const message, Args ... args) noexcept
	{
		char buffer[BUFSIZ_]{};
		auto R = _snprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, (args) ...);
		_ASSERTE(-1 != R);
		return { buffer };
	}

	// DBJ::TRACE exist in release builds too

	// this requires all the  args 
	// that are strings to be wide strings 
	// so I am doubtfull it will work ... easily
	template <typename ... Args>
	inline void TRACE(wchar_t const * const message, Args ... args) noexcept
	{
		::OutputDebugStringW(
			(DBJ::printf_to_buffer
			( message, 
				(args)... )).c_str()
		);
	}
	template <typename ... Args>
	inline void TRACE(const char * const message, Args ... args) noexcept
	{
		::OutputDebugStringA((DBJ::printf_to_buffer
		(message,
			(args)...)).c_str());
	}
} // eof DBJ 

namespace dbj {

	// probably the most used types
	using wstring = std::wstring;
	using wstring_vector = std::vector<std::wstring>;

	using string = std::string;
	using string_vector = std::vector<std::string>;



	// std::equal has many overloads
	// it is less error prone to have it here
	// in a single form
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
		inline T * alloc(size_t size_ = 0) {
			T * rez_ = new T;
			return rez_;
		}

		template<typename T>
		inline bool free(T * ptr_) {
			_ASSERTE(ptr_);
			delete ptr_;
			ptr_ = nullptr;
			return true;
		}

#ifdef DBJ_WIN

		template<typename T>
		inline T * alloc_win(size_t size_ = 0) {
			T * rez_ = (T*)CoTaskMemAlloc(sizeof(T));
			_ASSERTE(rez_ != nullptr);
			return rez_;
		}

		template<typename T>
		inline bool free_win(T * ptr_) {
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
	struct Exception : public std::runtime_error 
	{

	public:
		typedef std::runtime_error _Mybase;

		Exception(std::string_view _Message)
			: _Mybase(_Message.data())
		{	// construct from message string
		}

		Exception( std::wstring_view _WMessage)
			: _Mybase(
				std::string(_WMessage.begin(), _WMessage.end() ).c_str()
			)
		{	// construct from message unicode std string
		}


		Exception(const char *_Message)
			: _Mybase(_Message)
		{	// construct from message string
		}

		// wide what()
		// note: be sure to copy the result
		std::wstring wwhat() const
		{
			std::string s_{ this->what() };
			return { std::begin(s_), std::end(s_) };
		}
	};

	/*
	Core algo from http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign

	standard c++ will "do" the bellow for any T, that 
	is applicable to required operators
	*/
	 inline auto  sign = [] (const auto & v) constexpr -> int {
		return (v > 0) - (v < 0); // -1, 0, or +1
	 };

	/* avoid macros as much as possible */

	inline const auto & MIN = [](const auto & a, const auto & b) constexpr -> bool { return (a < b ? a : b); };
	inline const auto & MAX = [](const auto & a, const auto & b) constexpr -> bool { return (a > b ? a : b); };

	template < typename T, size_t N > 
	  inline constexpr size_t 
		countof(T const (&array)[N]) { return N; }
} // dbj

/*
Copyright 2017, 2018 by dbj@dbj.org

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
