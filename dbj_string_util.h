#pragma once

#include "dbj_crt.h"
#include "dbj_traits.h"

/*
#include <string>
#include <map>
#include <vector>
#include <iterator>
*/
// #include <type_traits>
#include <locale>
#include <sstream> // wstringstream
#include <memory>  // allocator
#include <string>
#include <optional>

#if 0
/*
the oher three" string types to std::ostream
*/
inline std::ostream & operator << (std::ostream & os, std::wstring && ws_)
{
	return os << std::string{ ws_.begin(), ws_.end() };
}

inline std::ostream & operator << (std::ostream & os, std::u16string && ws_)
{
	return os << std::string{ ws_.begin(), ws_.end() };
}

inline  std::ostream & operator << (std::ostream & os, std::u32string && ws_)
{
	return os << std::string{ ws_.begin(), ws_.end() };
}
#endif

namespace dbj {


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

	Inspired by: https://opensource.apple.com/source/bash/bash-80/bash/lib/sh/strnlen.c
	DBJ_INLINE size_t strnlen(const CHAR_T *s, size_t maxlen)
	{
	const CHAR_T *e = {};
	size_t n = {};

	for (e = s, n = 0; *e && n < maxlen; e++, n++)
	;
	return n;
	}

	in here we cater for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t

	*/
	template<typename T, size_t N>
	inline size_t strnlen(
		const T(&carr)[N],
		const size_t & maxlen,
		typename
		std::enable_if_t< dbj::is_std_char_v<T>, int > = 0)
	{
		return dbj::MIN(N, maxlen) - 1;
	}
	/*
	strlen for C++ native char array reference
	*/
	template<typename T, size_t N>
	inline size_t strlen(
		const T(&carr)[N],
		typename
		std::enable_if_t< dbj::is_std_char_v<T>, int > = 0
		)
	{
		return N - 1;
	}

	/*
	Pointer (to character arrays) support
	note: iosfwd include file contains char_traits we need

	2018 JUL 19  We would not repeat here what UCRT already has done
	namely char and wchar_t versions
	std lib defines strlen for char * and wchr_t *
	*/
	//  inline size_t strlen(const char *    cp) { return std::strlen(cp); }
	//  inline size_t strlen(const wchar_t * cp) { return std::wcslen(cp);  }
	inline size_t strlen(const char16_t * cp) { return std::char_traits<char16_t>::length(cp); }
	inline size_t strlen(const char32_t * cp) { return std::char_traits<char32_t>::length(cp); }
	/*
	 inline size_t strnlen(const char * cp, const size_t & maxlen) {
	size_t cpl = std::char_traits<char>::length(cp);
	return (cpl > maxlen ? maxlen : cpl);
	}
	 inline size_t strnlen(const wchar_t * cp, const size_t & maxlen) {
	size_t cpl = std::char_traits<wchar_t>::length(cp);
	return (cpl > maxlen ? maxlen : cpl);
	}
	*/
	inline size_t strnlen(const char16_t * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<char16_t>::length(cp);
		return (cpl > maxlen ? maxlen : cpl);
	}
	inline size_t strnlen(const char32_t * cp, const size_t & maxlen) {
		size_t cpl = std::char_traits<char32_t>::length(cp);
		return (cpl > maxlen ? maxlen : cpl);
	}

} // dbj

/*
TODO: this is very old stuff of questionalbe value, has to be checked as a such

Schurr_cpp11_tools_for_class_authors.pdf

constexpr str_const my_string = "Hello, world!";
static_assert(my_string.size() == 13, "");
static_assert(my_string[4] == 'o', "");
constexpr str_const my_other_string = my_string;
static_assert(my_string == my_other_string, "");
constexpr str_const world(my_string, 7, 5);
static_assert(world == "world", "");
constexpr char x = world[5]; // Does not compile because index is out of range!
*/

namespace dbj {

	class str_const final { // constexpr string
		const char* const p_{ nullptr };
		const std::size_t sz_{ 0 };
	public:

		template<std::size_t N>
		constexpr str_const(const char(&a)[N]) : // ctor
			p_(a), sz_(N - 1) {
		}

		template<std::size_t N>
		constexpr str_const(const char(&a)[N], std::size_t from, std::size_t to) : // ctor
			p_(a + from), sz_(a + from + to)
		{
			static_assert(to <= N);
			static_assert(from <  to);
		}

		// dbj added
		constexpr const char * data() const { return this->p_; }

		constexpr char operator[](const std::size_t & n) const
		{
			return (n <= sz_ ? this->p_[n] : throw std::out_of_range(__func__));
		}

		constexpr std::size_t size() const noexcept { return this->sz_; }

		// dbj added
		constexpr bool friend operator == (const str_const & left, const str_const & right)
		{
			if (left.size() != right.size())
				return false;
			std::size_t index_ = 0, max_index_ = left.size();
			while (index_ < max_index_) {
				if (left[index_] != right[index_]) {
					return false;
				}
				index_ += 1;
			}
			return true;
		}
	};

} // dbj

namespace dbj::str {



	constexpr std::size_t small_string_optimal_size{ 255 };

/*
Make a string optimized for small sizes
that is up to 255
this makes string not to do heap alloc/de-alloc
for strings up to 255 in length
Discussion (for example):
http://www.modernescpp.com/index.php/component/jaggyblog/c-17-avoid-copying-with-std-string-view
*/
template < 
typename CT,
typename string_type = std::basic_string< CT > ,
typename char_type = typename string_type::value_type,
typename size_type = typename string_type::size_type
>
constexpr inline string_type optimal
(
	size_type SMALL_SIZE = small_string_optimal_size ,
	char_type init_char_ 
		= static_cast<char_type>(0)
)
{
	DBJ_CHECK_IF( dbj::is_std_char_v<CT> );

	return string_type(	
		SMALL_SIZE,	
		init_char_
	);
}

/*-------------------------------------------------------------*/
template <
		typename CT,
		typename string_type 
		   = std::basic_string< std::decay_t<CT> >
	>
		constexpr inline string_type
		lowerize(
			CT * from_ , CT * last_
		)
	{
	DBJ_CHECK_IF(dbj::is_std_char_v<CT>,"CT argument is not a standard char type");

		string_type retval{ from_, last_ };
		auto rez = std::for_each(
			retval.begin(), 
			retval.begin() + retval.size(),
			[](CT & element) { element = std::tolower(element); }
		);
		return retval;
	}
	/*-------------------------------------------------------------*/
	template <
		typename CT,
		typename string_view_type = basic_string_view<CT>,
		typename string_type = std::basic_string< CT >
	>
		constexpr inline string_type 
		lowerize(
			string_view_type view_ 
		) 
	{
		return lowerize<CT>( view_.data(), view_.data() + view_.size());
	}
	/*-------------------------------------------------------------*/
	template <
		typename CT, std::size_t N,
		typename string_type 
		  = std::basic_string< std::decay_t<CT> >
	>
		constexpr inline string_type
		lowerize(
			CT (&view_)[N]
		)
	{
		return lowerize( view_ , view_ + N ); 
	}


	/// <summary>
	/// ui compare means locale friendly compare
	/// </summary>
	template<typename CT>
	inline int ui_string_compare(
		const CT * p1, const CT * p2, bool ignore_case
	)
	{
		_ASSERTE(p1 != nullptr);
		_ASSERTE(p2 != nullptr);
		std::basic_string<CT> s1{ p1 }, s2{ p2 };

		if (false == ignore_case) {
			s1 = lowerize<CT>(s1);
			s2 = lowerize<CT>(s2);
		}
		// the "C" locale is default 
		std::locale loc{};
		// the collate type 
		using collate_type = std::collate<CT>;
		// get collate facet:
		const auto & coll = std::use_facet< collate_type >( loc );
		//
		return coll.compare(p1, p1 + s1.size(), p2, p2 + s2.size());
	}


	/// <summary>
	/// is Lhs prefix to Rhs
	/// L must be shorter than R
	/// </summary>
	inline  bool  is_prefix(
		const std::wstring_view lhs, const std::wstring_view rhs
	)
	{
		_ASSERTE(lhs.size() > 0);
		_ASSERTE(rhs.size() > 0);

		// "predefined" can not be a prefix to "pre" 
		// opposite is true
		if (lhs.size() > rhs.size()) return false;

		return dbj::equal_(
			lhs.begin(),
			lhs.end(),
			rhs.begin());
	}

	extern "C" inline auto
		add_backslash(wstring & path_)
	{
		constexpr auto char_backslash{ L'\\' };
		if (path_.back() != char_backslash) {
			path_.append(wstring{ char_backslash });
		}
		return path_.size();
	}


	inline dbj::wstring_vector 
		tokenize (const wchar_t * szText, wchar_t token = L' ')
	{
		dbj::wstring_vector words{};
		std::wstringstream ss;
		ss.str(szText);
		std::wstring item;
		while (getline(ss, item, token))
		{
			if (item.size() != 0)
				words.push_back(item);
		}

		return words;
	}
	// narrow version
	inline dbj::string_vector
		tokenize(const char * szText, char token = ' ')
	{
		dbj::string_vector words{};
		std::stringstream ss;
		ss.str(szText);
		std::string item;
		while (getline(ss, item, token))
		{
			if (item.size() != 0)
				words.push_back(item);
		}

		return words;
	}

	namespace inner {
		/// <summary>
		/// Try to convert any range made of standard char types
		/// return type should be one of std strings
		/// range is anything that has begin() and end(), and 
		/// value_type typedef as per std containers model
		/// </summary>
		template < typename return_type >
		struct meta_converter final
		{
			template<typename T>
			return_type operator () (T arg)
			{
				if constexpr (dbj::is_range_v<T>) {
					static_assert (
						// arg must have this typedef
						dbj::is_std_char_v< T::value_type >,
						"can not transform ranges not made out of standard char types"
						);
					return { arg.begin(), arg.end() };
				}
				else {
					using actual_type
						= std::remove_cv_t< std::remove_pointer_t<T> >;
					return this->operator()(
						std::basic_string<actual_type>{ arg }
					);
				}
			}
		}; // meta_converter

		   // explicit instantiations
		template struct meta_converter<std::string   >;
		template struct meta_converter<std::wstring  >;
		template struct meta_converter<std::u16string>;
		template struct meta_converter<std::u32string>;

		template < typename T >
		std::ostream & operator << (std::ostream & os, const meta_converter<T> & ws_)
		{
			return os << "\nMeta Converter, return type: " << typeid(T).name() << "\n\n";
		}

	} // inner

} // dbj::str

namespace dbj {
	// all the meta converter INTANCES required / implicit instantiations
	inline dbj::str::inner::meta_converter<std::string   > range_to_string{};
	inline dbj::str::inner::meta_converter<std::wstring  > range_to_wstring{};
	inline dbj::str::inner::meta_converter<std::u16string> range_to_u16string{};
	inline dbj::str::inner::meta_converter<std::u32string> range_to_u32string{};
}


#pragma region deprected stuff due to meta converter introduction
#if 0
namespace dbj {

	template< size_t N>
	__forceinline dbj::wstring wide(const char(&charar)[N])
	{
		return { std::begin(charar), std::end(charar) };
	}

	__forceinline dbj::wstring wide(const char * charP)
	{
		std::string_view cv(charP);
		return { cv.begin(), cv.end() };
	}

	template< size_t N>
	__forceinline dbj::string narrow(const wchar_t(&charar)[N])
	{
		return { std::begin(charar), std::end(charar) };
	}

	__forceinline dbj::string narrow(const wchar_t * charP)
	{
		std::wstring_view cv(charP);
		return { cv.begin(), cv.end() };
	}
}
#endif
#pragma endregion
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

