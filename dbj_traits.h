#pragma once
#ifndef _WIN32
#include <cxxabi.h>
#endif

#include <type_traits>
#include <typeinfo>
#include <array>
#include <vector>
// license is at eof
#pragma region enable_if helpers

	namespace dbj {
		template < typename T >
		const std::string name() noexcept
		{
#ifdef _WIN32
			return { typeid(T).name() };
#else // __linux__
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
				// else if(error == -3)
				return { "bad argument" };

#endif // __linux__
		} // name()
	} // dbj

#define DBJ_TYPENAME(T) dbj::name<decltype(T)>().c_str()  

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
#pragma endregion enable_if helpers

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

		// template<typename T> constexpr inline bool is_range_v = is_range<T>::value;
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


} // eof dbj
#pragma endregion
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
