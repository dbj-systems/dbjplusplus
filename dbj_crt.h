#pragma once
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

namespace dbj {

	namespace {
		/*
		Core algo from http://graphics.stanford.edu/~seander/bithacks.html#CopyIntegerSign
		*/
		inline auto sign = [](const auto & v) constexpr -> int {
			return (v > 0) - (v < 0); // -1, 0, or +1
		};
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

#pragma region DBJ debug things
#ifdef _DEBUG
#define DBJ_ASSERT _ASSERTE
#define DBJ_VERIFY DBJ_ASSERT
#define DBJ_VERIFY_(result, expression) DBJ_ASSERT(result == expression)

// DBJ namespace is different from dbj namespace name
namespace DBJ {

	/* 512 happpens to be the BUFSIZ */
	constexpr size_t BUFSIZ_ = 512 * 2;

	// this requires all the string args to be unicode 
	// so I am doubtfull it will work ... easily
	template <typename ... Args>
	inline void TRACE(wchar_t const * const message, Args ... args) noexcept
	{
		wchar_t buffer[DBJ::BUFSIZ_]{};
		assert(-1 != _snwprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, (args) ...));
		::OutputDebugStringW(buffer);
	}
	template <typename ... Args>
	inline void TRACE(const char * const message, Args ... args) noexcept
	{
		char buffer[DBJ::BUFSIZ_]{};
		assert(-1 != _snprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, (args) ...));
		::OutputDebugStringA(buffer);
	}
}
#else
// release code dissapears
#define DBJ::TRACE 	__noop
#define DBJ_ASSERT __noop
// release code stays + with no checks
#define DBJ_VERIFY(expression) (void)(expression)
#define DBJ_VERIFY_(result, expression) (void)(expression)
#endif
#pragma endregion 
/*
2017-10-18	DBJ created

  DBJ CRT (C++ Run Time) is inside top level dbj namespace
*/
namespace dbj {

	template< size_t N>
	__forceinline std::wstring wide(const char(&charar)[N])
	{
		return std::wstring(std::begin(charar), std::end(charar));
	}

	__forceinline std::wstring wide(const char * charP)
	{
		std::string_view cv(charP);
		return std::wstring(cv.begin(), cv.end());
	}



/*
	template<class F, class... Pack>
	constexpr __forceinline auto
		call
		(F&& fun, Pack&&... args) {
		infoBuf.clear();
		if (0 == std::invoke(fun, (args)...))
			throw dbj::Exception(typeid(F).name());
		return (infoBuf);
	}
*/
	/* avoid macros as much as possible */

	static auto MIN = [](auto a, auto b) { return (((a) < (b)) ? (a) : (b)); };
	static auto MAX = [](auto a, auto b) { return (((a) > (b)) ? (a) : (b)); };
	template < typename T, size_t N > 
	inline	constexpr 
		size_t 
		countof(T const (&array)[N]) { return N; }
	/*
	in here we cater for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t
	*/
#if 0
	/*
	Inspired by: https://opensource.apple.com/source/bash/bash-80/bash/lib/sh/strnlen.c
	*/
		DBJ_INLINE size_t strnlen(const CHAR_T *s, size_t maxlen)
		{
			const CHAR_T *e = {};
			size_t n = {};

			for (e = s, n = 0; *e && n < maxlen; e++, n++)
				;
			return n;
		}
#endif

	/*
	(c) 2017 by dbj.org
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

	namespace {
		using namespace std;

		// http://en.cppreference.com/w/cpp/experimental/to_array
		namespace {
			template <class T, size_t N, size_t... I>
			/*constexpr*/ inline array<remove_cv_t<T>, N>
				to_array_impl(T(&a)[N], index_sequence<I...>)
			{
				return { { a[I]... } };
			}
		}
	}

	/*
	Transform "C array" into std::array
	at compile time
	make sure you use it on native array not a pointer
	*/
	template <class T, std::size_t N>
	inline constexpr array<remove_cv_t<T>, N> to_array(T(&a)[N])
	{
		return to_array_impl(a, make_index_sequence<N>{});
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
		constexpr char operator[](std::size_t n) const { // []
			return n < sz_ ? this->p_[n] : throw std::out_of_range("");
		}
		constexpr std::size_t size() const noexcept { return this->sz_; }
	};

	/*
	dbj "invention": compile time line of chars, usage
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
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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