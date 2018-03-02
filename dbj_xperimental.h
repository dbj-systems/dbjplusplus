#pragma once
/*
__interface msvc keyword
explained here: https://docs.microsoft.com/en-us/cpp/cpp/interface
*/
namespace dbj {
#pragma region template to inherit lambdas
	// dbj::x is eXperimental stuff
	namespace x {
		/*
		http://cpptruths.blogspot.rs/2018/02/inheritance-vs-stdvariant-based.html
		*/
		   template <class... Ts>
		   struct overloaded : Ts... {
			   /*
			   generate function call operator for every lambda inherited
			   this works because lambdas are impemented as functors
			   */
			   using Ts::operator()...;
			   /*
			   again this also works because lambdas are functors
			   and they have ctors that receive lambda instance
			   so here we construct this struct with variable
			   number of lambdas:
			    
				 auto enchilada_ = overloaded {
				     [](bool && ){ return "bool was sent"; }, 
					 [](float &&){ return "float was sent";}
				 } ;
				     enchilada_(true) ; // calls first lambda stored
					 enchilada_("does not compile"); // no lambda inside to receive const char *
			   */
			   explicit overloaded(Ts... ts) : Ts(ts)... {}
		   };

		   // C++17 user defined deduction guide for the above
		   // removes the need to give template arguments when 
		   // constructing overloaded
#ifndef _MSVC_LANG
		   template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
#else
#pragma message "--------> Be advised MSVC C++ stil does not deliver user defined deduction guides"
#endif
	} // x
#pragma endregion

	// dbj::x is eXperimental stuff
	namespace x {
		/*
		create tuple from any range type
		that can be iterated over with
		begin and end iterators
		an operator '[]' too
		(c) dbj.org Feb 2018
		note: POC code

		DISCUSSION:

		using namespace std;
		auto buzz = make_tuple( "foo", "bar", "baz" );
		auto bark = make_tuple( "moo", "meow", "arf" );
		auto my_cat_tuple = std::tuple_cat(buzz, std::move(bark));

		my_cat_tuple is size 6

		the key issue is how to achieve this as run time implementation
		for example in a loop ...
		*/

#define RANGE_TO_TUPLE_KLUDGE (1==1)

		auto && range_to_tuple = [](const auto & v)
		{
			auto rett0 = make_tuple(v[0]);
			using TT = decltype(rett0);

			for (auto element_ : v) {
#if RANGE_TO_TUPLE_KLUDGE
				// this cast is a kludge
				// replace with compiler agnostic solution
				auto rettemp = (
					tuple_cat(
						rett0,
						move(TT(element_))
					)
					);
				rett0 = rettemp;
#else
				TT inter{ element_ };
				rett0 = tuple_cat(rett0, move(inter));
#endif
			}
			return rett0;
		};


		/*
		this works on T only if T ctor has 
		number of args equal to the tuple number of types
		*/
		namespace detail {
			template <class T, class Tuple, size_t... I>
			constexpr T make_from_tuple_impl(Tuple&& t, index_sequence<I...>)
			{
				return T(get<I>(forward<Tuple>(t))...);
			}
		} // namespace detail

		template <class T, class Tuple>
		constexpr T make_from_tuple(Tuple&& t, T * = 0)
		{
			return detail::make_from_tuple_impl<T>(forward<Tuple>(t),
				make_index_sequence<tuple_size_v<decay_t<Tuple>>>{});
		}

	} // x
} // dbj

#ifdef DBJ_TESTING_EXISTS
template <typename F>
inline void dbj_util_test(F & print) {
/* keep test with the code it is testing so that devs can see it not try to discover it*/
}
#endif 
/* standard suffix for every dbj++ header */
#pragma comment( user, __FILE__ "(c) 2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 

/*
Copyright 2018 by dbj@dbj.org

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

