#pragma once
// dbj++ 2018
// license at eof
/*
#include <type_traits>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
#include <ratio>
*/
#include "dbj_traits.h"
#include <set>

namespace dbj {
	 
	namespace math {

		namespace float_to_integer {
			inline auto nearest = [](float value_) constexpr -> int { 
				return (int)((int)(value_ + 0.5)); 
			};
			inline auto drop_fractional_part = [](float value_) constexpr -> int {
				return (int)((int)value_); 
			};
			inline auto next_highest = [](float value_) constexpr -> int  {
				return (int)((int)(value_ + 0.9)); 
			};
			inline auto  integer = [](float value_) constexpr -> int {
				return (int)value_; 
			};
		};
	}



	namespace util {
		namespace {

			inline auto dbj_count = [](auto && range) constexpr->size_t
			{
				return std::distance(std::begin(range), std::end(range));
			};

			/*
			this is good idea, generate tuple with a Func
			since types do not matter func retval
			can be of any type
			and we can do a lot of things inside a func
			*/
			template <typename F, size_t... Is>
			auto gen_tuple_impl(F func, std::index_sequence<Is...>) {
				return std::make_tuple(func(Is)...);
			}

			template <size_t N, typename F>
			auto gen_tuple(F func) {
				return gen_tuple_impl(func, std::make_index_sequence<N>{});
			}
			// dbj added -- for std sequences
			template<typename T, size_t N>
			auto seq_tup(const T & sequence) {
				return gen_tuple<N>(
					[&](size_t idx) { return sequence[idx]; }
				);
			};

			// dbj added -- for native arrays
			template<typename T, size_t N>
			auto seq_tup(T(&arr)[N]) {
				return gen_tuple<N>(
					[&](size_t idx) { return arr[idx]; }
				);
			}

#if templated_classical_applicator
			/* non recursive applicator */
			template<typename F, typename... Targs>
			inline
				void applicator(F callback, Targs... args)
			{
				// since initializer lists guarantee sequencing, this can be used to
				// call a function on each element of a pack, in order:
				char dummy[sizeof...(Targs)] = { (callback(args), 0)... };
			}
#else
			/*applicator generic lambda*/
			auto applicator = [](auto  callback, auto... args) -> void {
				// todo: use std::is_callable on the callback 
				if constexpr(sizeof...(args) > 0) {
					// since initializer lists guarantee sequencing, this can be used to
					// call a function on each element of a pack, in order:
					// also with the help of a comma operator
					// which will populate 'dummy' with 0's
					char dummy[sizeof...(args)] = { (callback(args), 0)... };
				}
			};
#endif

			/*
			compile time array to vector
			*/
			template<typename Type, size_t N, typename outype = std::vector<Type> >
			inline constexpr outype array_to_vector(const Type(&arr_)[N])
			{
				return outype{ arr_, arr_ + N };
			}

			template<size_t N>
			auto array_to_vector_lambda = [&](const auto(&arr_)[N]) constexpr->std::vector<decltype(auto)>
			{
				return std::vector<decltype(auto)>{ arr_, arr_ + N };
			};



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

				static_assert(equal_types  (val_, mat_),
					"dbj::does_start [error] arguments not of the same type"
					);

				if  constexpr(equal_types(val_, char_star{})) {
					// #pragma message ("val type is char *")
					return starts_with(std::string(val_), std::string(mat_));
				}
				else if  constexpr(equal_types(val_, wchar_star{})) {
					// #pragma message ("val type is wchar_t *")
					return starts_with(std::wstring(val_), std::wstring(mat_));
				}
				else {
					return 0 == val_.compare(0, mat_.size(), mat_);
				}
			};
#if classical_overloading_solution
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
			template <typename Type, bool sort = false >
			inline std::vector<Type> remove_duplicates(const std::vector<Type> & vec) 
			{
				if constexpr (sort) {
					/*
					this is apparently also faster for very large data sets
					*/
					const std::set<Type> s(vec.begin(), vec.end());
					return std::vector<Type>{ s.begin(), s.end() };
				} else {
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
			inline std::vector<Type> remove_duplicates(const Type(&arr_)[N]) {
				return remove_duplicates(std::vector<Type>{arr_, arr_ + N});
			}

			/*
			Actually a left pad where maxlen is 12
			*/
			inline auto string_pad(std::string s_, char padchar = ' ', size_t maxlen = 12) {
				return s_.insert(0, maxlen - s_.length(), padchar);
			};

			inline auto string_pad(int number_, char padchar = ' ', size_t maxlen = 12) {
				return string_pad(std::to_string(number_), padchar, maxlen );
			};


			// create text line of 80 chars '-' by default
			// once called can not be changed ;)
			template< size_t N = 80, typename ARF = char(&)[N], typename ARR = char[N] >
			constexpr auto line(char fill_char = '-') -> ARF
			{
				auto set = [&]() -> ARF {
					static ARR arr;
					auto rz = std::memset(arr, fill_char, N);
					return arr;
				};

				// allocate array on stack once
				static ARF arr_ = set();
				return arr_;
			}
		} // ns
	} // util
} // dbj



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
