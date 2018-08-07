#pragma once

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

	

		/// <summary>
		/// some generic utility function 
		/// with argument declared as 
		/// native array reference
		/// </summary>
		template<typename T, std::size_t N >
		inline void native_arr_consumer(T(&arr_ref)[N]) {
			for (auto & element : arr_ref) {
				__noop;
			}
		}
	}

	namespace dbj_testing_space {

		inline decltype(auto)
			few_creation_examples() {

			int iarr[]{ 0,1,2,3,4,5,6,7,8,9 };

			auto narf_0 = DBJ_TEST_ATOM(
				dbj::narf::make(iarr)
			);

			/* this throws an segv on read 
			static decltype(auto) narf_1 = DBJ_TEST_ATOM(
				dbj::narf::make({ "native","array","of", "narrow","string","literals" }));
			*/

			const char * narrs[]{ "native", "array", "of", "narrow", "string", "literals" };
			static auto narf_1 = DBJ_TEST_ATOM(dbj::narf::make(narrs));


			char const_char_array[]{ "native char array" };

			auto narf_2 = DBJ_TEST_ATOM(
					dbj::narf::make(const_char_array));

			return narf_1;
		}

		// this is how we get the reference
		// to the contained native array
		template< typename T>
			auto different_ways_to_obtain_reference( T arf) 
			{
			decltype(auto) not_elegant = dbj::narf::data(arf);
			auto & standard = dbj::narf::data(arf);
			// change the native-arr contents
			// watch the retval in debugger to check 
			// the survival
			dbj::narf::apply(arf, [](auto idx, auto element) {
				char word[]{ (char) dbj::util::random(90, 64) };
				element = word;
			});
			// return by value
			return arf;
		}

		template< typename T>
			auto calling_native_array_consumer
			( T arf ) 
			{
			// example of calling a function 
			// that requires
			// native array as argument
			native_arr_consumer(arf.get());
			// same as
			native_arr_consumer(dbj::narf::data(arf));
			// same as
			// using value_type = typename narf_type::type;
			// native_arr_consumer(arf);

			return arf;
		}
	}

DBJ_TEST_UNIT( native_dbj_array_handler )
{
	using namespace dbj_testing_space;
	auto arf_0 = DBJ_TEST_ATOM(few_creation_examples());
	auto arf_1 = DBJ_TEST_ATOM(different_ways_to_obtain_reference(
		arf_0
	));
	auto arf_2 = DBJ_TEST_ATOM(calling_native_array_consumer(arf_1));
	// at this point all the arf's 
	// are referencing the same native array
	auto[bg, ed] = (dbj::narf::range(arf_0));

	// default get() is the pointer
	// due to array type decay 
	auto narrptr = arf_2.get();

	auto sze = DBJ_TEST_ATOM(dbj::narf::size(arf_2));
}
	
DBJ_TEST_UNIT( dbj_array_handler_ARH ) 
{
	{
		// the "C" way
		char arr_of_chars[]{ 'A','B','C' };
		char(&ref_to_arr_of_chars)[3] = arr_of_chars;
	}
	{   // manual C++ old school
		std::array<char, 3> three_chars{ 'A','B','C' };
		const char(&uar)[3] =
			(*(char(*)[3])three_chars.data());
	}
	// the modern C++ dbj way
	using A16 =  dbj::arr::ARH<int, 3>;
	A16::ARR arr { 1,2,3 };
	A16::ARP arp = DBJ_TEST_ATOM( A16::to_arp(arr) );
	A16::ARF arf = A16::to_arf(arr) ;

	// prove that the type is right
	auto DBJ_UNUSED( rdr0 ) = 
		DBJ_TEST_ATOM ( A16::to_vector(arf)  );

	/*
	testing the internal_array_reference

	decltype(auto) bellow reveals the underlying type
	namely it transform int* to int(&)[]
	that is reference to c array inside std::array
	*/
	A16::ARF arf2 = A16::to_arf(arr) ;
	A16::ARV rdr1[[maybe_unused]] = DBJ_TEST_ATOM( A16::to_vector( arf2) );

	//auto arf3 = DBJ_TEST_ATOM( 
		//arf2
	//);
	// A16::ARV rdr2 [[maybe_unused]] = ( A16::to_vector(arf3));
}

DBJ_TEST_UNIT(a_bit_more_arh_narf_dancing) {


	// narf to temporary no can do 
	// --> auto narf_ = dbj::narf::make({ "A B C NARF" });
	// so
	char charr[]{ "A B C NARF" };
	auto narf_ =  DBJ_TEST_ATOM( dbj::narf::make(charr) );
	decltype(auto) narf_arf_ = DBJ_TEST_ATOM( dbj::narf::data(narf_) );

	using CARH = dbj::arr::ARH<char, 255>;

	//CARH::ARR narf_to_arh
	//	= CARH::to_std_array( narf_arf_ );

	CARH::ARR std_arr{ "CHAR ARR" };

	CARH::ARF native_arr_reference
		= CARH::to_arf(std_arr);

	CARH::ARR std_aray
		= DBJ_TEST_ATOM(CARH::to_std_array(native_arr_reference));

	CARH::ARF literal_to_native_arr_reference
		= CARH::to_arf(std_arr);
}
}
