#pragma once
// license is at eof
#pragma region enable_if helpers
namespace dbj {
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
	namespace {
		/*DT  stands for Decay Type
		we usualy decay the types here before using them
		see http://en.cppreference.com/w/cpp/types/decay
		*/
		template< typename T>
		using DT = std::decay_t<T>; // since C++14
									/*EIF stands for enable if*/
		template< bool pred >
		using EIF = typename std::enable_if_t< pred, int >;
	}
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
	using char_star = decltype("XYZ");
	using wchar_star = decltype(L"XYZ");

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
	namespace {
		/*
		are two types equal, for two values provided ?
		*/
		auto equal_types = [](auto & a, auto & b) constexpr -> bool
		{
			return std::is_same_v< std::decay_t<decltype(a)>, std::decay_t<decltype(b)> >;
			/*
			following does not catch pointers
			return std::is_same_v< dbj::remove_cvref_t<decltype(a)>, dbj::remove_cvref_t<decltype(b)> >;
			*/
		};
	}
	/* see dbj_util tests for usage example */
} // dbj
#pragma endregion 

#pragma region is for containers
  /*
  dbj vector to touple optimization of  https://stackoverflow.com/posts/28411055/
  */
namespace dbj {

	using namespace std;

	namespace {

		template<typename T> struct is_std_array : public false_type {};

		/* this is not catching anything but a **value** of type vector<t> */
		template<typename T, size_t N>
		struct is_std_array< array<T, N> > : public true_type {};

		/*-----------------------------------------------------------*/

		template<typename T> struct is_vector : public false_type {};

		/* this is not catching anything but a **value** of type vector<t> */
		template<typename T, typename A>
		struct is_vector< vector<T, A> > : public true_type {};
	}
	/*
	To use the above one would need to write this (for example) :
	teamplate<typename T> void some_function ( T & v ) {
	static_assert( is_vector< std::decay_t< decltype(v)> >::value );
	}
	this is not very elegant or usefull , so ...
	*/

	/* template struct */
	template< typename T>
	struct IS_VECTOR {
		static const bool value = is_vector< std::decay_t< T > >::value;
	};

	/* variable template */
	template< typename T>
	inline constexpr bool IS_VECTOR_V = is_vector< std::decay_t< T > >::value;

	/* template alias */
	template< typename T>
	using IS_VECTOR_T = typename is_vector< std::decay_t< T > >::type;

	namespace {
		/*
		one uses this in runtime situations
		*/
		auto is_vector_v = [](const auto & v) -> boolean {
			return is_vector< std::decay_t< decltype(v) > >::value;
		};
	}

} // eof dbj
#pragma endregion

#ifdef DBJ_TESTING_EXISTS
/* dbj type traits and enable if helpers */
#ifndef DBJ_NV

#ifndef DBJ_STRINGIFY
#define DBJ_STRINGIFY(s) # s
#endif

#ifndef DBJ_EXPAND
#define DBJ_EXPAND(s) DBJ_STRINGIFY(s)
#endif
/*
use print or printex to show the symbol and its value, for example:
printex ( DBJ_NV( typeid(whatever).name ), DBJ_NV( typeid(xyz).name ) ) ;

WARNING: this is primitive, use with caution
TODO: if symbol contains comma this is not going to work
*/
#define DBJ_NV_DELIMITER " , "
#define DBJ_NV( symbol) DBJ_EXPAND(symbol)  DBJ_NV_DELIMITER , symbol 
#endif

namespace dbj {
namespace {

	template<typename T, typename dbj::require_integral<T> = 0>
	DBJ_INLINE auto Object(T&& t) { return std::variant<T>(t); }

	template<typename T, typename dbj::require_floating<T> = 0>
	DBJ_INLINE auto Object(T&& t) { return std::variant<T>(t); }

	/*usage*/
	template <typename F>
	inline void dbj_traits_tests ( F & print ) {

		assert(true == dbj::is_floating<decltype(42.0f)>());
		assert(true == dbj::is_integral<decltype(42u)>());
		assert(true == dbj::is_object<decltype(42u)>());

		auto o1 = (Object(42   ));
		auto o2 = (Object(42.0f));
	}
} // namespace
} // dbj
#endif // DBJ_TESTING_EXISTS

/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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
