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

// if the argument of the DBJ_ARR_LEN macro is a pointer, 
// code won't compile 
#define DBJ_ARR_LEN(str) (::dbj::array_size_helper(str)) 


/* avoid macros as much as possible */
#ifdef NOMINMAX
inline const auto & MIN = [](const auto & a, const auto & b) 
  constexpr -> bool { return (a < b ? a : b); };
inline const auto & MAX = [](const auto & a, const auto & b) 
  constexpr -> bool { return (a > b ? a : b); };
#else
#error Be sure you have #define NOMINMAX placed before including windows.h !
#endif


// NOTE: do not have a space after a macro name and before the '(' !!
#ifndef DBJ_STRINGIFY	
#define DBJ_STRINGIFY(s) #s
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
	using wstring_vector = std::vector<std::wstring>;
	using string_vector = std::vector<std::string>;

	using ::std::string;
	using ::std::wstring;


	constexpr inline auto LINE    (){ return "--------------------------------------------------------------------------------"sv; };
	constexpr inline auto COMPANY (){ return "DBJ.Systems Ltd."sv; };
	constexpr inline auto YEAR    (){ return std::string_view{ (__DATE__ + 7) }; };

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

// Good decades old VERIFY macro
#define DBJ_VERIFY_(x, file, line ) if (false == (x) ) ::dbj::nano::terror( #x ", failed", file, line )
#define DBJ_VERIFY(x) DBJ_VERIFY_(x,__FILE__,__LINE__)


	/* 512 happpens to be the POSIX BUFSIZ */
#ifndef BUFSIZ
#define BUFSIZ 512
#endif

	constexpr inline size_t BUFSIZ_ = BUFSIZ * 2 * 4; // 4096

#pragma warning( push )
#pragma warning( disable: 4190 )

	inline dbj::string itos(long l_) {
		std::array<char, 64> str{ {0} };

		[[maybe_unused]] auto [p, ec] 
			= std::to_chars(str.data(), str.data() + str.size(), l_);
		DBJ_VANISH(p);
		_ASSERTE(ec != std::errc::value_too_large);
			return { str.data() };
	}

	template <typename T, size_t N>
	constexpr inline size_t
		array_size_helper(const T(&)[N]) { return N; }


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

	standard c++ will "do" the bellow for any T, that 
	is applicable to required operators
	*/
	 inline auto  sign = [] (const auto & v) constexpr -> int {
		return (v > 0) - (v < 0); // -1, 0, or +1
	 };


	template < typename T, size_t N > 
	  inline constexpr size_t 
		countof(T const (&array_)[N]) { 
		  DBJ_VANISH(array_);
		  return N; 
	  }
} // dbj

/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"