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

// basically forget about init lists passing
// https://stackoverflow.com/questions/20059061/having-trouble-passing-multiple-initializer-lists-to-variadic-function-template/47159747#47159747
// yes I know macro + variadic = horific
// but in here it seems as a sort of a solution
#define DBJ_IL(T,...) (std::initializer_list<T>{__VA_ARGS__})

namespace dbj {
	namespace util {

		// std::equal has many overloads
// it is less error prone to have it here
// in a single form
// and use this one as we exactly need
		template<class InputIt1, class InputIt2>
		constexpr bool equal_(InputIt1 first1, InputIt1 last1, InputIt2 first2)
		{
			for (; first1 != last1; ++first1, ++first2) {
				if (!(*first1 == *first2)) {
					return false;
				}
			}
			return true;
		}

		/*
		text line of chars, usage
		constexpr auto line_ = c_line();
		constexpr auto line_2 = c_line<60>('=');
		*/
		template<size_t size = 80>
		constexpr inline std::string_view  char_line
		(
			const char filler = '-'
		)
		{
			static std::string_view the_line_(
				[&]
			{
				static std::array< char, size + 1 > array_{};
				array_.fill(filler);
				array_[size] = '\0';
				return array_.data();
			}()
				);
			return the_line_;
		};

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


		//-----------------------------------------------------------------------------
		/*

		why? to have "void" as function return type
		why? for std::cout << T situations
		how to use:

		void f1() {}
		const char * f2() { return __func__ ; }

		std::cout << std::endl <<
		std::get<0>( possible_void_call(f1) )
		<< std::endl ;

		std::cout << std::endl <<
		std::get<0>( possible_void_call(f2) )
		<< std::endl ;
		*/
		template<typename F, typename  ...Args >
		inline auto
			possible_void_call(F fun, Args ... args)
		{
			if constexpr (std::is_void<
				std::invoke_result_t<F, Args...>
			>::value)
			{
				return std::make_tuple("void");
			}
			else {
				return std::make_tuple(fun(args...));
			}
		}

		//-----------------------------------------------------------------------------
		// rac == Range and Container
		// I prefer it to std::array
		template<
			typename T, std::size_t N
		>
			struct rac final
		{
			using type = T;
			using data_ref = std::reference_wrapper<T[N]>;
			T * begin() const { return value; }
			T * end() const { return value + N; }
			size_t size() const { return N; }
			data_ref data() const { return data_ref{ value }; }
			// yes data is public
			// if you need a foot gun
			// help yourself
			mutable T value[N]{};
		};

		inline auto random = [](int max_val, int min_val = 1) -> int {
			static auto initor = []() {
				std::srand((unsigned)std::time(nullptr)); return 0;
			}();
			return min_val + std::rand() / ((RAND_MAX + 1u) / max_val);
		};

		inline auto dbj_count = [](auto && range) constexpr->size_t
		{
			static_assert(::dbj::is_range_v< decltype(range) >)
				return std::distance(std::begin(range), std::end(range));
		};

		/*
		this is good idea, generate tuple with a Func
		since types do not matter func retval
		can be of any type
		and we can do a lot of things inside a func
		*/
		template <typename F, size_t... Is>
		inline auto gen_tuple_impl(F func, std::index_sequence<Is...>) {
			return std::make_tuple(func(Is)...);
		}

		template <size_t N, typename F>
		inline auto gen_tuple(F func) {
			return gen_tuple_impl(func, std::make_index_sequence<N>{});
		}
		// dbj added -- for std sequences
		template<typename T, size_t N>
		inline auto seq_tup(const T & sequence) {
			return gen_tuple<N>(
				[&](size_t idx) { return sequence[idx]; }
			);
		};

		// dbj added -- for native arrays
		template<typename T, size_t N>
		inline auto seq_tup(T(&arr)[N]) {
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
		inline auto applicator = [](auto  callback, auto... args) -> void {
			// todo: use std::is_callable on the callback 
			if constexpr (sizeof...(args) > 0) {
				// since initializer lists guarantee sequencing, this can be used to
				// call a function on each element of a pack, in order:
				// also with the help of a comma operator
				// which will populate 'dummy' with 0's
				char dummy[sizeof...(args)] = { (callback(args), 0)... };
			}
		};
#endif

		/*
		find an item in anything that has begin and end iterators
		*/
		inline auto find = [](auto sequence, auto item) constexpr -> bool {
			return std::find(std::begin(sequence), std::end(sequence), item) != std::end(sequence);
		};


		/*
		bellow works for all the std:: string types and string view types
		but it also works for char pointers and wchar_t pointers
		and it is all inside the single lambda
		which is fast because redundant code is removed at compile time
		*/
		template<
			typename C
		>
			inline auto starts_with(
				std::basic_string_view<C> val_,
				std::basic_string_view<C> mat_
			)
		{
			return 0 == val_.compare(0, mat_.size(), mat_);
		}
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
		inline std::vector<Type> remove_duplicates(const Type(&arr_)[N]) {
			return remove_duplicates(std::vector<Type>{arr_, arr_ + N});
		}

		// The memset function call could be optimized 
		// away by the compiler 
		// if the array object used, is no futher accessed.
		// other than this bellow  
		// one can use the more complex
		// std way: http://en.cppreference.com/w/c/string/byte/memset
		extern "C"
			inline void
			secure_zero(void *s, size_t n)
		{
#ifdef _DBJ_MT_
			dbj::sync::lock_unlock __dbj_auto_lock__;
#endif // _DBJ_MT_
			volatile char *p = (char *)s;
			while (n--) *p++ = 0;
		}

	} // util
} // dbj


/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"