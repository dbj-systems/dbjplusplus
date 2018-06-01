#pragma once
#ifdef DBJ_TESTING_EXISTS

#include <ctime>
#include "../dbj_array.h"

/// <summary>
/// for more stringent test we 
/// test from outside of the
/// namespace that we are testing
/// </summary>
namespace dbj_arf_testing {

	namespace dbj_testing_space  {

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
		inline void native_arr_consumer(T(&arr_ref)[N]) {
			for (auto & element : arr_ref) {
				// element = arr_ref[0];
			}
		}
	}

	namespace dbj_testing_space {
		inline auto few_creation_examples() {

			auto narf_0 = DBJ_TEST_ATOM(dbj::narf::make({ 0,1,2,3,4,5,6,7,8,9 }));
			// default_print(	narf_0,	default_element_output_	);

			static auto narf_1 = DBJ_TEST_ATOM(dbj::narf::make({ "native","array","of", "narrow","string","literals" }));
			// default_print( narf_1,[](size_t j, const char * element) { printf(" %zd:\"%s\"", j, element); }	);

			auto narf_2 = DBJ_TEST_ATOM(dbj::narf::make("native char array"));
			// default_print( narf_2 ,	[](size_t j, const char element) { printf(" %zd:'%c'", j, element); });

			auto buffer = narf_1;
			//char word_[]{"A"};
			//dbj::narf::apply(buffer, [&](auto idx, auto & element ) {
			// no can do --> element = word_;
			//word_[0] += 1;
			//});

			return buffer;
		}

		// this is how we get the reference
		// to the contained native array
		template < typename narf_type >
		auto
			different_ways_to_obtain_reference(
				// arg by value
				narf_type & arf
			) {
			auto & native_arr_ref = arf.get();
			decltype(auto) not_elegant = dbj::narf::data(arf);
			auto & standard = dbj::narf::data(arf);
			// change the n-arr contents
			// watch the retval in debugger to check 
			// the survival
			dbj::narf::apply(arf, [](auto idx, auto element) {
				char word[]{ (char)random(90, 64) };
				element = word;
			});
			// return by value
			return arf;
		}

		template < typename narf_type >
		auto calling_native_array_consumer(
			// arg by value
			narf_type arf
		) {
			// example of calling a function 
			// that requires
			// native array as argument
			native_arr_consumer(arf.get());
			// same as
			native_arr_consumer(dbj::narf::data(arf));
			// same as
			using value_type = typename narf_type::type;
			native_arr_consumer((value_type &)arf);

			return arf;
		}
	}

	DBJ_TEST_UNIT(": native dbj array (narf) handler ")
	{
		using namespace dbj_testing_space;
		auto arf_0 = DBJ_TEST_ATOM(few_creation_examples());
		auto arf_1 = DBJ_TEST_ATOM(different_ways_to_obtain_reference(arf_0));
		auto arf_2 = DBJ_TEST_ATOM(calling_native_array_consumer(arf_1));
		// at this point all the arf's 
		// are referencing the same native array
		auto[bg, ed] = DBJ_TEST_ATOM(dbj::narf::range(arf_0));

		// default get() is the pointer
		// due to array type decay 
		auto narrptr = arf_2.get();

		DBJ_TEST_ATOM(dbj::narf::size(arf_2));
	}
	
			DBJ_TEST_UNIT(": dbj array handler ARH ") 
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
			using A16 =  dbj::arr::ARH<int, 3>;
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

}
#endif