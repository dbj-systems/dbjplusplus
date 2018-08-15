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
		typename std::enable_if_t< dbj::is_std_char_v<T>, int > = 0)
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
	inline size_t strlen(const char16_t * cp) { return std::char_traits<char16_t>::length(cp); }
	inline size_t strlen(const char32_t * cp) { return std::char_traits<char32_t>::length(cp); }

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
static_assert(my_string.size() == 13);
static_assert(my_string[4] == 'o');
constexpr str_const my_other_string = my_string;
static_assert(my_string == my_other_string);
constexpr str_const world(my_string, 7, 5);
static_assert(world == "world");
constexpr char x = world[5]; // Does not compile because index is out of range!

also presented here:
https://en.cppreference.com/w/cpp/language/constexpr
*/

namespace dbj::str {

	// constexpr string
	// dbj: big note! this class does not won anything, just points to
	class str_const final 
	{ 
		const char* const p_{ nullptr };
		const std::size_t sz_{ 0 };
	public:

		template<std::size_t N>
		constexpr str_const(const char(&a)[N]) : // ctor
			p_(a), sz_(N - 1) {
		}

		template<std::size_t N>
		constexpr str_const(const char(&a)[N], std::size_t from, std::size_t to) : // ctor
			p_(a + from), sz_(from + to)
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
	}; // str_const

	// https://en.cppreference.com/w/cpp/language/constexpr
	// C++11 constexpr functions had to put everything in a single return statement
	// (C++14 doesn't have that requirement)
	constexpr std::size_t countlower(str_const s, std::size_t n = 0,
		std::size_t c = 0)
	{
		return n == s.size() ? c :
			'a' <= s[n] && s[n] <= 'z' ? countlower(s, n + 1, c + 1) :
			countlower(s, n + 1, c);
	}


	constexpr std::size_t small_string_optimal_size{ 255 };

/*
Make a string optimized for small sizes
that is up to arbitrary value of 255
this makes std::basic_string not to do heap alloc/de-alloc
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
constexpr inline 
// alow only std char types to be used
std::enable_if_t< dbj::is_std_char_v<CT>, string_type  >
optimal
(
	size_type SMALL_SIZE = small_string_optimal_size ,
	char_type init_char_ 
		= static_cast<char_type>('?')
)
{
	return string_type(	
		SMALL_SIZE,	
		init_char_
	);
}

/*-------------------------------------------------------------*/
template <
		typename CT,
		typename string_view_type = basic_string_view<CT>,
		typename string_type
		   = std::basic_string< std::decay_t<CT> >
	>
		inline 
	// alow only char and wchar_t 
	std::enable_if_t< dbj::is_char_v<CT> || dbj::is_wchar_v<CT>, string_type  >
	lowerize(
			const CT * from_ , const CT * last_
		)
	{
		_ASSERTE(from_ != nullptr);
		_ASSERTE(last_ != nullptr);
		_ASSERTE(last_ != from_);

		// safe and simple: copy to string (first!)
		string_type retval( from_, last_ );

		auto loc = std::locale("");
		// facet of user's preferred locale
		const std::ctype<CT >	& facet_ = std::use_facet<std::ctype<CT>>(loc);

		// this can fail for some locales
//		if constexpr(dbj::is_char_v<CT>) {
			facet_.tolower(retval.data(), retval.data() + retval.size());
//		}
//		else {
//			facet_.towlower(from_, last_);
//		}
		// assumption is eos is properly placed?
		return retval ;
	}
	/*-------------------------------------------------------------*/
	template <
		typename CT,
		typename string_view_type = basic_string_view<CT>,
		typename string_type = std::basic_string< CT >
	>
		constexpr inline 
		// alow only char and wchar_t
		std::enable_if_t< dbj::is_char_v<CT> || dbj::is_wchar_v<CT>, string_type  >
		lowerize(
			std::basic_string_view<CT> view_
		) 
	{
		_ASSERTE( ! view_.empty() );
		return lowerize<CT>( view_.data(), (view_.data() + view_.size()));
	}
	/*-------------------------------------------------------------*/
	template <
		typename CT, std::size_t N,
		typename string_type 
		  = std::basic_string< std::decay_t<CT> >
	>
		constexpr inline 
		// only char wchar_t
		std::enable_if_t< dbj::is_char_v<CT> || dbj::is_wchar_v<CT>, string_type  >
		lowerize(
			const CT (&view_)[N]
		)
	{
		return lowerize( view_ , (view_ + N) );
	}


	/// <summary>
	/// ui compare means locale friendly compare
	/// apparently collate id can not be compiled/linked for 
	/// char16_t and char32_t
	/// </summary>
	template<
		typename CT,
		typename std::enable_if_t< dbj::is_char_v<CT> || dbj::is_wchar_v<CT>, int > = 0
	>
	inline int ui_string_compare
	(
		const CT * p1, const CT  * p2,  bool ignore_case = true
	)
	{
		_ASSERTE(p1 != nullptr);
		_ASSERTE(p2 != nullptr);
		std::basic_string<CT> s1{ p1 }, s2{ p2 };

		if (ignore_case) {
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
	template < typename CT , typename std::enable_if_t< dbj::is_std_char_v<CT>, int> = 0 >
	inline  bool  is_view_prefix_to_view (
		 std::basic_string_view<CT> lhs, std::basic_string_view<CT> rhs
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

	// hard to believe but yes, all done at compile time
	template < typename CT, 
		typename string_type = std::basic_string<CT>,
		typename string_view_type = std::basic_string_view<CT>,
		typename std::enable_if_t< dbj::is_std_char_v<CT>, int> = 0 >
	inline  bool  is_prefix(
		CT const * lhs, CT const * rhs
	)
	{
		_ASSERTE(lhs != nullptr );
		_ASSERTE(rhs != nullptr );
		string_type ls(lhs), rs(rhs);
		return is_view_prefix_to_view(
			string_view_type{ ls.data(), ls.size() },
			string_view_type{ rs.data(), rs.size() }
		);
	}

	class __declspec(novtable) backslash final 
	{
	public:
		template< typename CT, typename std::enable_if_t< dbj::is_std_char_v<CT>, int> = 0 >
		size_t add ( std::basic_string<CT> & path_ , CT slash_ )
		{
			if (path_.back() != slash_) {
				path_[path_.size() + 1] = slash_;
			}
			return path_.size();
		}

		auto operator () ( std::string & path_)
		{
			return this->add( path_, '\\' );
		}
	};


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

namespace dbj::str {
	template <class T>
	using expr_type = typename std::remove_cv_t<std::remove_reference_t<T>>;

	/*
	remove all instances of a substring found in a string
	call with string view literals for the easiest usage experience
	work for all std char/string/stin_view types
	*/
	template <
		typename CT,
		typename string_type = std::basic_string<CT>,
		typename size_type = typename string_type::size_type
	>
	inline 
		std::enable_if_t< dbj::is_std_char_v<CT>, string_type >
		remove_all_subs(
			std::basic_string_view<CT> input_,
			std::basic_string_view<CT> pattern
		) 
	{
		string_type rezult{ input_.data() };
		size_type n = pattern.length();
		for (size_type j = rezult.find(pattern);
			j != string::npos;
			j = rezult.find(pattern)
			) 
		{
			rezult.erase(j, n);
		}
		return rezult;
	}

	template <
		typename CT, 
		typename string_type = std::basic_string<CT> ,
		typename size_type = typename string_type::size_type
	>
	inline 
		std::enable_if_t< dbj::is_std_char_v<CT>, string_type >
	remove_first_sub(
		std::basic_string_view<CT> s, std::basic_string_view<CT> pattern
	) {
		string_type rezult{ s.data() };
		size_type j = rezult.find(pattern.data());
		if (j != string_type::npos) {
			size_type n = pattern.length();
			rezult.erase(j, n);
		}
		return rezult;
	}


	// https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c

	template<typename C>
	inline
		std::enable_if_t< dbj::is_std_char_v<C>, std::basic_string<C> >
		replace_inplace
	(
		std::basic_string_view<C>  subject,
		std::basic_string_view<C>  search,
		std::basic_string_view<C>  replace
	)
	{
		std::basic_string<C> input{ (C *)subject.data() };
		size_t pos = 0;
		while ((pos = input.find(search, pos)) != std::string::npos) {
			input.replace(pos, search.length(), replace);
			pos += replace.length();
		}
		return input;
	}

	template<typename C>
	inline
		std::enable_if_t< dbj::is_std_char_v<C>, std::basic_string<C> >
		replace_inplace
		(
			C const * subject,
			C const * search,
			C const * replace
		) {
		return std::basic_string<C>{
			replace_inplace(
				std::basic_string_view<C>{ subject},
				std::basic_string_view<C>{ search },
				std::basic_string_view<C>{ replace })
		};
	    }
} // dbj::str
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

