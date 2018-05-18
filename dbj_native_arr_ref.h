#pragma once
#include <vector>
#include <array>
#include <ctime>

/// <summary>
/// NARF == Native ARray reFerence
/// </summary>
namespace dbj::narf {

	//---------------------------------------------------------------
	// NARF core
	//
	// the key abstraction
	// ARF == Array ReFerrence
	// ARF contains instance of native array T[N]
	// ARF can be safely copied/moved arround
	// in essence one uses ARF to carry arround native arrays
	// unlike std::array
	// ARF easily delivers reference to native array T[N]&
	// example: for() can be easily applied to T[N]&
	//  for (auto & e : arf.get() ) {}
	//---------------------------------------------------------------
	template <typename T, std::size_t N>
	using wrapper = std::reference_wrapper<T[N]>;

	template<typename T, size_t N>
	constexpr auto begin(const wrapper<T, N> & wrp_) {
		return std::begin(wrp_.get());
	}

	template<typename T, size_t N>
	constexpr auto end(const wrapper<T, N> & wrp_) {
		return std::end(wrp_.get());
	}


	/// <summary>
	/// returns std::reference_wrapper copy
	/// that contains reference to native array 
	/// this can be also used with init list
	/// <code>
	///  auto ari = 
	///  native_arr_ref({ 0,1,2,3,4,5,6,7,8,9 });
	///  auto arc = 
	///  native_arr_ref({ "Char array" });
	/// </code>
	/// </summary>
	template<typename T, std::size_t N >
	constexpr auto
		make(const T(&native_arr)[N])
		-> wrapper<T, N>
	{
		using nativarref = T(&)[N];
		return std::ref(
			const_cast<nativarref>(native_arr)
		);
	}

	/// <summary>
	/// returns std::reference_wrapper copy
	/// that contains reference to native array 
	/// in turn contained inside the std::array
	/// argument
	/// </summary>
	template<typename T, std::size_t N >
	constexpr auto
		make(const std::array<T, N> & std_arr)
		-> wrapper<T, N>
	{
		using nativarref = T(&)[N];

		return std::ref(
			(nativarref)*(std_arr.data())
		);
	}
	/// <summary>
	/// the ARF size is the
	/// size of n-array it holds
	/// </summary>
	template<typename T, std::size_t N >
	constexpr std::size_t size(wrapper<T, N> & narw_) {
		return N; //  array_size(narw_.get());
	}

	/// <summary>
	/// default for_each function
	/// note: REF is about references
	/// thus the changes will stay in 
	/// the containing native array after
	/// we leave this function
	/// 
	/// please make sure to read
	/// https://en.cppreference.com/w/cpp/algorithm/for_each
	/// to understand the std::for_each 
	/// requirements and behaviour
	/// </summary>
	template< typename T, size_t N, typename FUN >
	constexpr auto for_each(const wrapper<T, N> & arf, const FUN & fun_) {
		const auto & B = begin(arf);
		const auto & E = end(arf);
		return std::for_each(B, E, fun_);
	}

	/// <summary>
	/// generic apply function
	/// callback signature:
	/// <code>
	/// [] ( size_t idx_ , const auto & element ) {}
	/// </code>
	/// </summary>
	template< typename T, size_t N, typename CBK >
	constexpr auto apply(
		const wrapper<T, N> &  arf,
		CBK output_function
	) {
		size_t j = 0;
		auto outfun_wrap = [&](const auto & element_) {
			output_function(j++, element_);
		};

		return dbj::narf::for_each(arf, outfun_wrap);
	}

} // dbj::narf

#ifdef DBJ_TESTING_EXISTS

  /// <summary>
  /// for more stringent test we 
  /// test from outside of the
  /// namespace that we are testing
  /// </summary>
namespace dbj_arf_testing {

	namespace {

		inline auto default_element_output_ =
			[&](size_t j, const auto & element,
				const char * fmt_str = " %zd:%d ")
		{ printf(fmt_str, j++, element); };

		/// <summary>
		/// default arf print function
		/// </summary>
		template< typename T, size_t N, typename FUN >
		inline auto default_print(
			const dbj::narf::wrapper<T, N> & arf,
			FUN outfun_,
			const char * prefix = "\nArray",
			const char * suffix = ""
		) {
			printf("%s { ", prefix);
			dbj::narf::apply(arf, outfun_);
			printf("} %s", suffix);
		}

		inline auto random = [](int max_val, int min_val = 1) -> int {
			static auto initor = []() {
				std::srand((unsigned)std::time(nullptr)); return 0;
			}();
			return min_val + std::rand() / ((RAND_MAX + 1u) / max_val);
		};

		/// <summary>
		/// some generic utility function 
		/// with argument declared as 
		/// native array reference
		/// </summary>
		template<typename T, std::size_t N >
		inline void native_arr_argument(T(&arr_ref)[N]) {
			for (auto & element : arr_ref) {
				element = random(N);
			}
		}
	}


	DBJ_TEST_UNIT(": native dbj array (narf) handler ")
	{

		using std::array;

		// the arf type is here
		// std::reference_wrapper<int[10]> 
		auto arf = dbj::narf::make(array<int, 10>{});
		// argument to make()
		// does not exist here any more
		// arf contains the copy 
		// of the native array  

		// few creation/print examples
		{
			default_print(
				dbj::narf::make({ 0,1,2,3,4,5,6,7,8,9 }),
				default_element_output_
			);

			default_print(
				dbj::narf::make({ "native","array","of", "asci","string","literals" }),
				[](size_t j, const char * element) { printf(" %zd:\"%s\"", j, element); }
			);

			default_print(
				dbj::narf::make("native char array"),
				[](size_t j, const char element) { printf(" %zd:'%c'", j, element); }
			);
		}

		// this is how we get the reference
		// to the contained native array
		// int[10]&
		auto & native_arr_ref = arf.get();
		printf("\nThe typeid name of the contained arrays reference is %s", typeid(native_arr_ref).name());

		// and this is the pointer
		// due to array type decay 
		// the type is here: int *
		auto native_arr_ptr = arf.get();
		printf("\nThe typeid name of the contained arrays pointer is %s", typeid(native_arr_ptr).name());

		printf("\nThe size of the contained array is %u", (unsigned)dbj::narf::size(arf));

		// example of calling a function 
		// that requires
		// native array as argument
		native_arr_argument(arf.get());

		default_print(
			arf,
			default_element_output_,
			"\nAfter calling native_arr_argument"
		);

		// the for loop usage
		// notice the auto & element declaration 
		// so that internal array 
		// elements are updated
		for (auto & element : arf.get()) {
			element = random(255);
		};

		default_print(
			arf,
			default_element_output_,
			"\nAfter updating in the for loop"
		);

	}
}
#endif

/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
/*
Copyright 2017-2018 by dbj@dbj.org

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


