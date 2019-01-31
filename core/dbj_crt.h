#pragma once

// #include <sysinfoapi.h>
// #include <strsafe.h>
// #include <algorithm>
// #include <cstddef>
// #include <cwctype>
// #include <cctype>
// #include <string>
// #include <vector>
// #include <map>
// #include <algorithm>

#include <cassert>
#include <string_view>
#include <charconv>
#include <system_error>
#include <array>

#ifndef __clang__
#ifndef _MSC_VER
#error dbj++  requires Visual C++ 
#endif // !_MSC_VER
#if _MSC_VER < 1911
#error dbj++ requires Visual C++ 14.1 or better
#endif
#endif

#if (!defined(UNICODE)) || (! defined(_UNICODE))
#error dbj++ requires UNICODE builds
#endif

#ifdef _SCL_SECURE_NO_WARNINGS
#error dbj++ can not be used with  _SCL_SECURE_NO_WARNINGS defined
#endif

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
works in any space, example here
https://godbolt.org/z/jGC98L
*/
#define DBJ_NOUSE(...) static_assert( (noexcept(__VA_ARGS__),true) );

/* avoid macros as much as possible */
// #ifdef NOMINMAX
inline const auto & MIN = [](const auto & a, const auto & b) 
  constexpr -> bool { return (a < b ? a : b); };
inline const auto & MAX = [](const auto & a, const auto & b) 
  constexpr -> bool { return (a > b ? a : b); };
// #else
// #error Be sure you have #define NOMINMAX placed before including windows.h !
// #endif


// NOTE: do not have a space after a macro name and before the '(' !!
#ifndef DBJ_STRINGIFY	
#define DBJ_STRINGIFY_(s) #s
#define DBJ_STRINGIFY(s) DBJ_STRINGIFY_(s)
#define DBJ_EXPAND(s) DBJ_STRINGIFY(s)
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )

#define DBJ_EXP_(s) #s
#define DBJ_EXP(s) DBJ_EXP_(s)
#endif

// https://www.boost.org/doc/libs/1_35_0/boost/preprocessor/stringize.hpp
#    define BOOST_PP_STRINGIZE(text) BOOST_PP_STRINGIZE_A((text))
#    define BOOST_PP_STRINGIZE_A(arg) BOOST_PP_STRINGIZE_I arg
#    define BOOST_PP_STRINGIZE_I(text) #text
//

#ifndef DBJ_COMPANY	

#ifdef _DEBUG
#define DBJ_BUILD "DBJ*Debug"
#else
#define DBJ_BUILD "DBJ*Release"
#endif 

#define DBJ_COMPANY "DBJ.Systems Ltd."
#define DBJ_YEAR (__DATE__ + 7)
#define DBJ_BUILD_STAMP "(c) " __DATE__ " by " DBJ_COMPANY "| " DBJ_BUILD ": [" __DATE__ "][" __TIME__ "]" 
#endif
#define DBJ_ERR_PROMPT(x) \
__FILE__ "(" DBJ_EXPAND(__LINE__) ") -- " __FUNCSIG__ " -- " x " -- "

// 
#define DBJ_CHECK_IF static_assert

/*
DBJ preffered concept to code standard C++ string constants
is to use string_view

#include <string_view>
using namespace
::std::literals::string_view_literals;

	constexpr inline auto
	   compiletime_string_view_constant
		= "compile time"sv ;

as evident this also works at compile time

static_assert(compiletime_string_view_constant == "compile time" );

*/

namespace dbj {

	using namespace ::std;
	using namespace ::std::string_view_literals;

	// probably the most used types
	using wstring_vector = vector<wstring>;
	using string_vector  = vector<string>;

	constexpr inline auto LINE    (){ return "--------------------------------------------------------------------------------"sv; };
	constexpr inline auto COMPANY (){ return "DBJ.Systems Ltd."sv; };
	constexpr inline auto YEAR    (){ return std::string_view{ (__DATE__ + 7) }; };

	inline wchar_t const * app_base_name() noexcept {
		auto initor = []() {
			std::wstring base_name(__wargv[0]);
			base_name.erase(0,
				1 + base_name.find_last_of(L"\\")
			);
			return base_name;
		};
		static auto basename_ = initor();
		return basename_.c_str();
	}


	namespace nano
	{
		template<typename T, typename F>
		inline T
			transform_to(F str) noexcept
		{
			if constexpr (!is_same_v<T, F>) {
				if (str.empty()) return {};
				return { std::begin(str), std::end(str) };
			}
			else {
				// T and F are the same type
				// thus just move a copy
				return str;
			}
		};
		// no native pointers please
		// use literals
		template<typename T, typename F>
		inline T transform_to(F *) = delete;

		[[noreturn]]
		inline void terror
		(char const * msg_, char const * file_, const unsigned line_)
		{
			assert(msg_ != nullptr);
			assert(file_ != nullptr);
			assert(line_ > 0);
			::fprintf(stderr, "\n\ndbj++ Terminating ERROR:%s\n%s (%d)", msg_, file_, line_);
			exit(EXIT_FAILURE);
		}

	} // nano

// decades old VERIFY macro
#define DBJ_VERIFY_(x, file, line ) if (false == (x) ) ::dbj::nano::terror( #x ", failed", file, line )
#define DBJ_VERIFY(x) DBJ_VERIFY_(x,__FILE__,__LINE__)


	/* 512 happpens to be the POSIX BUFSIZ */
#ifndef BUFSIZ
#define BUFSIZ 512
#endif

	constexpr inline size_t BUFSIZ_ = BUFSIZ * 2 * 4; // 4096

#pragma warning( push )
#pragma warning( disable: 4190 )

	inline dbj::string itos(long l_) 
	{
		std::array<char, 64> str{ {0} };

		[[maybe_unused]] auto [p, ec] 
			= std::to_chars(str.data(), str.data() + str.size(), l_);
		DBJ_NOUSE(p);
		_ASSERTE(ec != std::errc::value_too_large);
			return { str.data() };
	}

	/*
	transform path to filename
	delimiter is '\\'
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
			std::string_view suffix = "")
		{
			_ASSERTE(!file_path.empty());
			return {
				FILENAME(file_path) + "(" + dbj::itos(line_) + ")"
				+ (suffix.empty() ? "" : suffix.data())
			};
		}

	} // extern "C"
#pragma warning( pop )

	template <size_t BUFSIZ_ = dbj::BUFSIZ_, typename ... Args>
	inline std::wstring printf_to_buffer(wchar_t const * const message, Args ... args) noexcept
	{
		wchar_t buffer[BUFSIZ_]{};
		auto DBJ_MAYBE(R) = _snwprintf_s(buffer, BUFSIZ_, _TRUNCATE, message, (args) ...);
		_ASSERTE(-1 != R);
		return {buffer};
	}

	template <size_t BUFSIZ_ = dbj::BUFSIZ_, typename ... Args>
	inline std::string printf_to_buffer(const char * const message, Args ... args) noexcept
	{
		char buffer[BUFSIZ_]{};
		auto DBJ_MAYBE(R) = _snprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, (args) ...);
		_ASSERTE(-1 != R);
		return { buffer };
	}

	// DBJ::TRACE exist in release builds too
	template <typename ... Args>
	inline void TRACE(wchar_t const * const message, Args ... args) noexcept
	{
		::OutputDebugStringW(
			(printf_to_buffer( message, (args)... )).c_str()
		);
	}
	template <typename ... Args>
	inline void TRACE(const char * const message, Args ... args) noexcept
	{
		::OutputDebugStringA(
			(printf_to_buffer(message,(args)...)).c_str()
		);
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
	Core algo from http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign

	standard c++ will compile this, for any T, that 
	is applicable to required operators
	return value is trigraph -1,0,+1
	result will be constexpr on POD types
	*/
	template<typename T>
	 inline constexpr auto  sign ( T const & v) 
	 {
		return (v > 0) - (v < 0); // -1, 0, or +1
	 };

// if the argument of the DBJ_ARR_LEN macro is a pointer, 
// code won't compile 
#define DBJ_ARR_LEN(str) (::dbj::countof(str)) 

	template < typename T, size_t N > 
	  inline constexpr size_t 
		countof(T const (&)[N]) { 
		  return N; 
	  }
} // dbj

/* inclusion of this file defines the kind of a licence used */
#include "../dbj_gpl_license.h"

/* standard suffix for every dbj header */
#pragma comment( user, DBJ_BUILD_STAMP ) 