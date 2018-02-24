#pragma once
// dbj++ 2018
// license at eof

#include <type_traits>
#include <vector>
#include <algorithm>
#include <set>
#include <iterator>
#include <functional>

#include "dbj_traits.h"

namespace dbj {

/*
create tuple from any range type
that can be iterated over with
begin and end iterators
(c) dbj.org Feb 2018
note: POC code
*/
auto && range_to_tuple = [](const auto & v)
{
	// static_assert(IS_VECTOR_V< decltype(v) >);

	// Vector Element Type 
	using VET = typename decay<decltype(v)>::type;
	// Tuple Type
	using TT = tuple< typename VET::value_type >;

	TT rett0 = tie(v[0]);

	for (auto element_ : v) {
		// this cast is a kludge
		// relace with compiler agnostic solution
		rett0 = (TT&&)(
			tuple_cat(
				rett0, tie(element_)
			)
			);
	}
	return rett0;
};



	namespace xprmntl {

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

	} // namespace 

	namespace util {

		template<typename F, typename... Targs>
		inline
			void applicator(F callback, Targs... args)
		{
			// since initializer lists guarantee sequencing, this can be used to
			// call a function on each element of a pack, in order:
			char dummy[sizeof...(Targs)] = { (callback(args), 0)... };
		}

	} // util
} // dbj


namespace dbj {
	namespace util {

		/*
		array to vector
		*/
		template<typename Type, size_t N, typename outype = std::vector<Type> >
		inline constexpr outype array_to_vector(const Type(&arr_)[N])
		{
			return outype{ arr_, arr_ + N };
		}

		namespace {
			/*
			find an item in anything that has begin and end iterators
			*/
			auto find = [](auto sequence, auto item) constexpr -> bool {
				return std::find(std::begin(sequence), std::end(sequence), item) != std::end(sequence);
			};


			/*
			bellow works for all the std:: string types and string view types
			but it also works for char pointers and wchar_t pointers
			and it is all inside the single lambda
			which is fast because redundant code is removed at compile time
			*/
			auto starts_with = [](auto val_, auto mat_) {

				static_assert(eqt(val_, mat_),
					"dbj::does_start [error] arguments not of the same type"
					);

				if  constexpr(eqt(val_, char_star{})) {
					// #pragma message ("val type is char *")
					return starts_with(std::string(val_), std::string(mat_));
				}
				else if  constexpr(eqt(val_, wchar_star{})) {
					// #pragma message ("val type is wchar_t *")
					return starts_with(std::wstring(val_), std::wstring(mat_));
				}
				else {
					return 0 == val_.compare(0, mat_.size(), mat_);
				}
			};
		}
#if 0
		/*
		classical overloading solution
		*/
		template<typename T = char>
		inline
			bool starts_with(const std::basic_string<T> & value, const std::basic_string<T> & match)
		{
			return 0 == value.compare(0, match.size(), match);
		}

		template<size_t N, typename C = char >
		inline
			bool starts_with(const C(&value_)[N], const C(&match)[N])
		{
			return starts_with<C>(std::basic_string<C>{value_}, std::basic_string<C>{match});
		}

		template< typename  C = char >
		inline
			bool starts_with(const C * value_, const C * match)
		{
			return starts_with<C>(std::basic_string<C>{value_}, std::basic_string<C>{match});
		}
#endif // 0

		/*
		------------------------------------------------------------------------------------
		*/
		template <typename Type>
		inline std::vector<Type> remove_duplicates(const std::vector<Type> & vec, const bool sort = false) {

			if (sort) {
				/*
				this is apparently also faster for very large data sets
				*/
				const std::set<Type> s(vec.begin(), vec.end());
				return std::vector<Type>{ s.begin(), s.end() };
			}
			else {
				std::vector<Type> unique_chunks{};
				for (auto x : vec) {
					if (!dbj::util::find(unique_chunks, x)) {
						unique_chunks.push_back(x);
					}
				}
				return unique_chunks;
			}
		}

		template <typename Type, size_t N >
		inline std::vector<Type> remove_duplicates(const Type(&arr_)[N], const bool sort = false) {
			return remove_duplicates(std::vector<Type>{arr_, arr_ + N}, sort);
		}

		/*
		------------------------------------------------------------------------------------
		*/
		inline auto string_pad(std::string s_, char padchar = ' ', size_t maxlen = 12) {
			return s_.insert(0, maxlen - s_.length(), padchar);
		};

		inline auto string_pad(int number_) {
			return string_pad(std::to_string(number_));
		};

	} // util
} // dbj
#ifdef DBJ_TESTING_EXISTS
	template <typename F>
	inline void dbj_util_test( F & print ) {
		{

			int intarr[]{ 1,1,2,2,3,4,5,6,6,6,7,8,9,9,0,0 };
			auto ret1 = dbj::util::remove_duplicates(intarr);
			std::string as2[16]{
				"abra", "ka", "dabra", "babra",
				"abra", "ka", "dabra", "babra",
				"abra", "ka", "dabra", "babra",
				"abra", "ka", "dabra", "babra",
			};

			auto ad = dbj::util::remove_duplicates(as2);
			char carr[] = { 'c','a','b','c','c','c','d', 0x0 };
			dbj::util::remove_duplicates(carr);
			auto see_mee_here = carr;
		}
		{
			auto doesit1 = dbj::util::starts_with("abra ka dabra", "abra");
			auto doesit2 = dbj::util::starts_with(L"abra ka dabra", L"abra");
		}
	}
#endif 
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

  __interface msvc keyword
  explained here: https://docs.microsoft.com/en-us/cpp/cpp/interface
  */
