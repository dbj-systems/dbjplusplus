#pragma once
// license is at eof
/*
LINUX type name demangling has to ne done like this

#include <cxxabi.h>

template < typemame T> 
std::string demangle () {
// delete malloc'd memory
struct free_ {
void operator()(void* p) const { std::free(p); }
};
// custom smart pointer for c-style strings allocated with std::malloc
using ptr_type = std::unique_ptr<char, free_>;

// special function to de-mangle names
int error{};
ptr_type name{ abi::__cxa_demangle(typeid(T).name(), 0, 0, &error) };

if (!error)        return { name.get() };
if (error == -1)   return { "memory allocation failed" };
if (error == -2)   return { "not a valid mangled name" };
// else if(error == -3) or otherwise
return { "bad argument" };
}

*/

#include "dbj_crt.h"

#include <type_traits>
#include <typeinfo>
#include <array>
#include <vector>
#include <string>

namespace dbj {
	namespace tt {

	constexpr inline const  char space[]{ " " };
	constexpr inline const  char line[]{ "------------------------------------------------------------" };

		// we hold the result
	template < typename T >
	constexpr inline const char * name_() noexcept
	{
		static const char * type_name_[]{ typeid(T).name() };
		return type_name_[0];
	} // name()
	

	template<typename T>
	struct actual final {
		using unqualified_type = std::remove_cv_t< T >;
		using not_ptr_type = std::remove_pointer_t< unqualified_type > ;
	};


		template< typename T>
		struct instrument final
		{
			template< typename TT>
			struct descriptor final
			{
				using type = TT;
				constexpr   static inline const bool	is_pointer 
					= std::is_pointer_v<TT>;
				constexpr 	static inline const bool	is_array 
					= std::is_array_v<TT>;
				constexpr 	static inline const size_t  number_of_dimension 
					= std::rank_v<TT>;
				constexpr 	static inline const size_t  first_extent 
					= std::extent_v<TT>;
			};

			using def_type 
				= descriptor<T>;
			using actual_type 
				= descriptor<std::remove_cv_t< std::remove_pointer_t<T> >>;
			using under_type 
				= descriptor<typename std::remove_all_extents<T>::type>;

			template<typename T>
			static std::string to_string(void) noexcept
			{
				return
					std::string{ "\ndefault type" } +
					std::string{ T::descriptor_to_string<T::def_type>() } +
					std::string{ "\nactual type" } +
					std::string{ T::descriptor_to_string<T::actual_type>() } +
					std::string{ "\nunderlying type" } +
					std::string{ T::descriptor_to_string<T::under_type>() };
			}

			template< typename T >
			static std::string descriptor_to_string() noexcept
			{
				return DBJ::printf_to_buffer(
					"\n%-20s"
					"\n%-20s : %s / %s"
					"\n%-20s : %s -- %zu"
					"\n%-20s : %s -- %zu",
					name_<T::type>(),
					space, (T::is_pointer ? "Pointer" : "NOT Pointer"), (T::is_array ? "Array" : "NOT Array"),
					space, "dimensions, if array", T::number_of_dimension,
					space, "dimension[0] size, if array", T::first_extent
				);
			}
		};

	} // tt
} // dbj 

#ifndef DBJ_TYPENAME
#define DBJ_TYPENAME(T) (dbj::tt::name_<T>()) 
#define DBJ_VALTYPENAME(V) (dbj::tt:name_<decltype(V)>()) 
#else
#error  DBJ_TYPENAME already defined?
#endif // !DBJ_TYPENAME

#pragma region char and string related traits
namespace dbj {

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
		is_char< std::remove_cv_t<T> >::value || is_wchar<std::remove_cv_t<T>>::value ||
		is_char16<std::remove_cv_t<T>>::value || is_char32<std::remove_cv_t<T>>::value
		>
	{};

	template<typename T>
	inline constexpr bool  is_std_char_v = is_std_char<T>::value;

	// is T, a standard string
	template< class T>
	struct is_std_string : integral_constant<bool,
		is_same<remove_cv_t<T>, string    >::value ||
		is_same<remove_cv_t<T>, wstring   >::value ||
		is_same<remove_cv_t<T>, u16string >::value ||
		is_same<remove_cv_t<T>, u32string >::value> {};

	template<typename T>
	inline constexpr bool  is_std_string_v = is_std_string<T>::value;

} // dbj
#pragma endregion

#pragma region enable_if helpers

	/*
	Templates are zealous eaters of types
	thus overloading and templates do not mix easily
	iso C++ comitee inveneted std::enable_if<>
	Which in turn makes you either "be somewhere else"
	or scramble for false cover of macros
	Here we do not use macros. we use modern C++ to deliver
	helpers when using templates woth or without overloading

	std::enable_if<> tamed with template aliases
	insipred with https://codereview.stackexchange.com/questions/71946/use-of-macros-to-aid-visual-parsing-of-sfinae-template-metaprogramming
	NOTE: C++17 onwards is required
	
	*/
namespace dbj {
		/*DT  stands for Decay Type
		we usualy decay the types here before using them
		see http://en.cppreference.com/w/cpp/types/decay
		*/
		template< typename T>
		using DT = std::decay_t<T>; // since C++14
									/*EIF stands for enable if*/
		template< bool pred >
		using EIF = typename std::enable_if_t< pred, int >;
	
	/* we define constexpr dbj::is_ function for every std::is_ */
	template< typename T>
	constexpr bool is_object() { return std::is_object_v< DT<T>>; }

	template< typename T>
	constexpr bool is_integral() { return std::is_integral_v< DT<T>>; }

	template< typename T>
	constexpr bool is_floating() { return std::is_floating_point< DT<T> >::value; }

	/* bellow are dbj::require_ templates we use through EIF<> */
	template< typename T>
	using require_integral = EIF< std::is_integral_v< DT<T> > >;

	template< typename T>
	using require_floating = EIF< std::is_floating_point< DT<T> >::value >;

	template< typename T>
	using require_object = EIF< std::is_object_v< DT<T> > >;

}
#pragma endregion 
// enable_if helpers

#pragma region type traits + generic lambdas
namespace dbj {
	using char_star = decltype("X");
	using wchar_star = decltype(L"X");

	namespace {
		/*
		part of C++20, but also implemented here
		http://en.cppreference.com/w/cpp/types/remove_cvref
		*/
		template< class T >
		struct remove_cvref {
			typedef std::remove_cv_t<std::remove_reference_t<T>> type;
		};

		template< class T >
		using remove_cvref_t = typename remove_cvref<T>::type;
	}

	/// <summary>
	/// c++ 17 generic lambdas have issues
	/// with required types of auto arguments
	/// in c++20 this will be fixed with new
	/// lambda arguments template declaration syntax
	/// </summary>
	namespace required
	{
		// does type of the value v matches the template 
		// argument type RQ
		template<typename RQ>
		inline auto  is_type = [](const auto & v_ = 0) constexpr -> bool
		{
			using T = std::decay_t< decltype(v_) >;
			return std::is_same<T, RQ>();
		};

		// here we can extend with helpers we need
		// for example this one
		inline auto is_uint64 = [](const auto & v_ = 0) constexpr -> bool
		{
			return is_type<std::uint64_t>(v_);
		};

	} // required 

	namespace {
		/*
		are two types equal, for two values provided ?
		*/
		inline auto equal_types = [](auto & a, auto & b) constexpr -> bool
		{
			return std::is_same_v< std::decay_t<decltype(a)>, std::decay_t<decltype(b)> >;
/*
if not using decay_t the following does not catch pointers hidden in auto's
return std::is_same_v< dbj::remove_cvref_t<decltype(a)>, dbj::remove_cvref_t<decltype(b)> >;
*/
		};
	}
	/* see dbj_util tests for usage example */
} // dbj
#pragma endregion 

#pragma region for containers
  /*
  dbj vector to touple optimization of  https://stackoverflow.com/posts/28411055/
  */
namespace dbj {

	using namespace std;

	namespace inner {

		template <typename T, typename = void>
		struct is_range final : std::false_type {};

		template <typename T>
		struct is_range<T
			, std::void_t
			  <
			    decltype(std::declval<T>().begin()),
			    decltype(std::declval<T>().end())
			  >
		> final : std::true_type {};

		/*-----------------------------------------------------------*/
		/*
		full is_stl_container solution here: https://goo.gl/8ZQ5Xj
		*/
		template<typename T > struct is_std_array : public false_type {};

		/* this is not catching anything but a type array<t,n> */
		template<typename T, size_t N>
		struct is_std_array< array<T, N> > : public true_type {};

		/*-----------------------------------------------------------*/

		template<typename T> struct is_std_vector : public false_type {};

		/* this is not catching anything but a type vector<t> */
		template<typename T, typename A>
		struct is_std_vector< vector<T, A> > : public true_type {};
	}

	// type aliases and value aliases
	template<typename T>
	constexpr inline bool is_range_v = inner::is_range<T>::value;

	template<typename T>
	using is_range_t = typename inner::is_range<T>::type;

	template< typename T >
	using is_std_array_t = typename inner::is_std_array< std::decay_t<T> >::type;

	template< typename T >
	constexpr inline bool is_std_array_v = inner::is_std_array< std::decay_t<T> >::value;

	template< typename T>
	using is_std_vector_t = typename inner::is_std_vector< std::decay_t<T> >::type;

	template< typename T>
	constexpr inline bool is_std_vector_v = inner::is_std_vector< std::decay_t<T> >::value;

	//template <typename T>
	//using is_bool_t = std::is_same<typename std::decay_t<T>, bool>::type ;

	template <typename T>
	constexpr inline bool is_bool_v = std::is_same_v<typename std::decay_t<T>, bool>;

} // eof dbj
#pragma endregion
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
