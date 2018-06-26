#pragma once

#include "dbj_crt.h"

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

namespace dbj::str {

	using namespace std;

	//NOTE: pointers are not char's
	// char *. wchar_t * .. are thus not chars	template< class T>
	template<typename T>
	struct is_std_char : integral_constant<bool,
		is_same<remove_cv_t<T>, char     >::value ||
		is_same<remove_cv_t<T>, wchar_t  >::value ||
		is_same<remove_cv_t<T>, char16_t >::value ||
		is_same<remove_cv_t<T>, char32_t >::value> {};

	template<typename T>
	inline constexpr bool  is_std_char_v = is_std_char<T>::value;


	template< class T>
	struct is_std_string : integral_constant<bool,
		is_same<remove_cv_t<T> , string    >::value ||
		is_same<remove_cv_t<T> , wstring   >::value ||
		is_same<remove_cv_t<T> , u16string >::value ||
		is_same<remove_cv_t<T> , u32string >::value> {};

	template<typename T>
	inline constexpr bool  is_std_string_v = is_std_string<T>::value;

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
	DBJ_CHECK_IF( is_std_char_v<CT> );

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
	DBJ_CHECK_IF(is_std_char_v<CT>,"CT argument is not a standard char type");

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

}
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

