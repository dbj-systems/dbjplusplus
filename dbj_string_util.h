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


namespace dbj::str {

	using namespace std;

	// dbj.org 2018-07-03
	// NOTE: pointers are not char's
	// char *. wchar_t * .. are thus not chars	
	// take care of chars and their signed and unsigned forms
	// where 'char' means one of the four std char types

	template<class _Ty>	struct is_char : std::false_type {	};
	template<> struct is_char<char> : std::true_type {	};
	template<> struct is_char<signed char> : std::true_type {	};
	template<> struct is_char<unsigned char> : std::true_type {	};

	template<class _Ty>	struct is_wchar : std::false_type {	};
	template<> struct is_wchar<wchar_t> : std::true_type {	};

	template<class _Ty>	struct is_char16 : std::false_type {	};
	template<> struct is_char16<char16_t> : std::true_type {	};

	template<class _Ty>	struct is_char32 : std::false_type {	};
	template<> struct is_char32<char32_t> : std::true_type {	};

	// and one for all
	template<typename T>
	struct is_std_char :
		std::integral_constant
		<
		bool,
		is_char<T>::value || is_wchar<T>::value ||
		is_char16<T>::value || is_char32<T>::value
		>
	{};

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
						dbj::str::is_std_char_v< T::value_type >,
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

	  // all the types required / implicit instantiations
	using char_range_to_string = inner::meta_converter<std::string   >;
	using wchar_range_to_string = inner::meta_converter<std::wstring  >;
	using u16char_range_to_string = inner::meta_converter<std::u16string>;
	using u32char_range_to_string = inner::meta_converter<std::u32string>;


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

