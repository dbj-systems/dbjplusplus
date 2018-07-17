#pragma once


DBJ_TEST_SPACE_OPEN( dbj_util )

DBJ_TEST_UNIT(": dbj util unit range container test ") 
{
	using RCI10 = dbj::util::rac<int, 10>;
	RCI10 rci10{};

	int j{};
	for (auto && elem : rci10) {
		elem = j++;
	}

	auto & dta1 = rci10.value ;
	DBJ_TEST_ATOM( rci10.data() );
}

	DBJ_TEST_UNIT(": dbj util math float_to_integer test ") {

		using namespace dbj::math::float_to_integer;

		auto test = [](float val_) {
			dbj::log::print("\n\nInput:\t");
			DBJ_TEST_ATOM(val_);
			DBJ_TEST_ATOM(nearest(val_));
			DBJ_TEST_ATOM(drop_fractional_part(val_));
			DBJ_TEST_ATOM(next_highest(val_));
			DBJ_TEST_ATOM(integer(val_));
		};

		test(123.00f);
		test(123.500f);
		test(123.4901f);
		test(123.501f);
	}

	DBJ_TEST_UNIT(": dbj dbj_util_test ") {

		int intarr[]{ 1,1,2,2,3,4,5,6,6,6,7,8,9,9,0,0 };
		auto DBJ_UNUSED(ret1)
			= DBJ_TEST_ATOM ( dbj::util::remove_duplicates(intarr) );
		std::string as2[16]{
			"abra", "ka", "dabra", "babra",
			"abra", "ka", "dabra", "babra",
			"abra", "ka", "dabra", "babra",
			"abra", "ka", "dabra", "babra",
		};

		auto ad = DBJ_TEST_ATOM( dbj::util::remove_duplicates(as2) );
		char carr[] { 'c','a','b','c','c','c','d', 0x0 };
		auto DBJ_UNUSED(rez) 
			 = DBJ_TEST_ATOM( dbj::util::remove_duplicates(carr) );
		auto see_mee_here = carr;
		DBJ_TEST_ATOM( dbj::util::starts_with("abra ka dabra", "abra") );
		DBJ_TEST_ATOM( dbj::util::starts_with(L"abra ka dabra", L"abra") );
	};


	DBJ_TEST_SPACE_CLOSE
