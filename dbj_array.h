#pragma once

#include <vector>
#include <array>
#include <ctime>

namespace dbj {
	namespace {
		using namespace std;

		// http://en.cppreference.com/w/cpp/experimental/to_array
		namespace {
			template <class T, size_t N, size_t... I>
			/*constexpr*/ inline array<remove_cv_t<T>, N>
				to_array_impl(T(&a)[N], index_sequence<I...>)
			{
				return { { a[I]... } };
			}
		}
	}

	/*
	Transform native array into std::array at compile time
	*/
	template <class T, std::size_t N>
	inline constexpr array<remove_cv_t<T>, N> native_to_std_array(T(&a)[N])
	{
		return to_array_impl(a, make_index_sequence<N>{});
	}
}

/// <summary>
/// NARF == Native ARray reFerence
/// </summary>
namespace dbj::narf {

	//---------------------------------------------------------------
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

	//---------------------------------------------------------------
	// ARF core
	//---------------------------------------------------------------

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
		auto B = std::begin(arf.get());
		auto E = std::end(arf.get());
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

		auto random = [](int max_val, int min_val = 1) -> int {
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
	/// <summary>
	/// test unit
	/// </summary>
	/// <param name="argc">count of command line args</param>
	/// <param name="sargv">the command line args strings</param>
	inline void unit(int argc, wchar_t * sargv[])
	{
		using std::array;

		// the arf type is here
		// std::reference_wrapper<int[10]> 
		auto arf = dbj::narf::make(array<int, 10>{});
		// argument to make()
		// does not exist here any more
		// arf contains the copy 
		// of the native array  

		// few more creation/print examples
		{
			default_print(
				dbj::narf::make({ 0,1,2,3,4,5,6,7,8,9 }),
				default_element_output_
			);
			// asci string literals
			default_print(
				dbj::narf::make({ "Abra","Ka","Dabra" }),
				[](size_t j, const char * element) { printf(" %zd:\"%s\"", j, element); }
			);
			// native char array ref
			default_print(
				dbj::narf::make("Abra Ka Dabra"),
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
		// so that internal array elements 
		// can be updated
		for (auto & element : arf.get()) {
			element = random(255);
		};

		default_print(
			arf,
			default_element_output_,
			"\nAfter updating in the for loop"
		);

	}

	//-----------------------------------------------------
	typedef const int(&i3ref)[3];

	// template<typename T, size_t N>
	inline auto arfer = [](
		/*const auto(&nar)[]*/
		i3ref if_
		)
		constexpr->i3ref
	{
		i3ref local_{ 1,2,3 };
		return local_;
	};

}


namespace dbj {

	// http://cpptruths.blogspot.rs/2011/10/multi-dimensional-arrays-in-c11.html
	template <class T, size_t ROW, size_t COL>
		using Matrix = std::array<std::array<T, COL>, ROW>;
	// usage: Matrix<float, 3, 4> mat;
	
	template <class T, size_t ROW, size_t COL>
	using NativeMatrix = T[ROW][COL];
	// usage: NativeMatrix<float, 3, 4> mat;

	namespace arr {
		/*
		return array reference to the C array inside std::array
		usage:
				decltype(auto) iar =
					internal_array_reference(
							std::array<int,3>{1,2,3}
				) ;
		*/
		template<typename T, size_t N,
			typename ARR = std::array<T, N>, /* std::array */
			typename ART = T[N],    /* C array */
			typename ARF = ART & ,  /* reference to it */
			typename ARP = ART * >  /* pointer   to it */
				constexpr inline	ARF
			reference(const std::array<T, N> & arr)
		{
			return *(ARP) const_cast<typename ARR::pointer>(arr.data());
		}

		/*
		native ARray Helper

		(c) 2018 by dbj.org
		*/
		template< typename T, size_t N >
		struct ARH
		{
			// vector type
			typedef std::vector<T> ARV;
			// std::array type
			typedef std::array<T, N> ARR;
			// inbuilt ARray type
			typedef T ART[N];
			// reference to ART
			typedef ART& ARF;
			// pointer to ART
			typedef ART* ARP;

			/*
			return pointer to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARP
				to_arp( ARR arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/*
			return reference to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARF
				to_arf( ARR arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}

			/*		native	array to vector			*/
			static constexpr ARV
				to_vector(const ARF arr_)
			{
				return ARV{ arr_, arr_ + N };
			}

			/*		"C"	array to std array			*/
			static constexpr ARR 
				to_std_array(const ARF arr_)
			{
				ARR retval_{};
				std::copy(arr_, arr_ + N, retval_.begin() );
				return retval_;
			}
		}; // ARH


#ifdef DBJ_TESTING_EXISTS

			DBJ_TEST_UNIT(": dbj array handler ") 
			{
			{
				// the "C" way
				char arr_of_chars[]{ 'A','B','C' };
				char(&ref_to_arr_of_chars)[3] = arr_of_chars;
			}
			{   // manual C++ old school
				std::array<char, 3> three_chars{ 'A','B','C' };
				const char(&uar)[3] = *(char(*)[3])three_chars.data();
			}
			// the modern C++ dbj way
			using A16 = ARH<int, 3>;
			A16::ARR arr { 1,2,3 };
			A16::ARP arp = A16::to_arp(arr);
			A16::ARF arf = A16::to_arf(arr);

			// prove that the type is right
			auto rdr0 = A16::to_vector(arf);

			/*
			testing the internal_array_reference

			decltype(auto) bellow reveals the underlying type
			namely it transform int* to int(&)[]
			that is reference to c array inside std::array
			*/
			decltype(auto) arf2 = A16::to_arf(arr);
			decltype(auto) rdr1 = A16::to_vector(arf2);

			decltype(auto) arf3 = arf2;
			auto rdr2 = A16::to_vector(arf3);
		}
#endif // DBJ_TESTING
	} // arr
} // dbj

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

