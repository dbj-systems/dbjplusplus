#pragma once

#include "dbj_string_util.h"

DBJ_TEST_SPACE_OPEN(dbj_string_util)

DBJ_TEST_UNIT(dbj_string_util_replace_inplace) {

	using namespace std::literals;

	const  std::string
		new_string
		= DBJ_TEST_ATOM(dbj::str::replace_inplace("abra ka dabra"sv, "ka"sv, "malaga"sv));

	const  std::wstring
		new_wstring
		= DBJ_TEST_ATOM(dbj::str::replace_inplace(L"abra ka dabra"sv, L"ka"sv, L"malaga"sv));

	const  std::u16string
		new_u16string
		= DBJ_TEST_ATOM(dbj::str::replace_inplace(u"abra ka dabra"sv, u"ka"sv, u"malaga"sv));

	const  std::u32string
		new_u32string
		= DBJ_TEST_ATOM(dbj::str::replace_inplace(U"abra ka dabra"sv, U"ka"sv, U"malaga"sv));
}

DBJ_TEST_UNIT(dbj_string_util_remove_first_sub) {

	using namespace std::literals;

	const  std::string
		new_string
		=  DBJ_TEST_ATOM( dbj::str::remove_first_sub("abra ka dabra"sv, "ka"sv) );

	const  std::wstring
		new_wstring
		= DBJ_TEST_ATOM( dbj::str::remove_first_sub(L"abra ka dabra"sv, L"ka"sv) );

	const  std::u16string
		new_u16string
		= DBJ_TEST_ATOM( dbj::str::remove_first_sub(u"abra ka dabra"sv, u"ka"sv) );

	const  std::u32string
		new_u32string
		= DBJ_TEST_ATOM( dbj::str::remove_first_sub(U"abra ka dabra"sv, U"ka"sv) );
}

DBJ_TEST_UNIT(dbj_string_util_remove_all_subs) {

	using namespace std::literals;

	const  std::string
		new_string
		= DBJ_TEST_ATOM(dbj::str::remove_all_subs("abra ka dabra"sv, "ab"sv));

	const  std::wstring
		new_wstring
		= DBJ_TEST_ATOM(dbj::str::remove_all_subs(L"abra ka dabra"sv, L"ab"sv));

	const  std::u16string
		new_u16string
		= DBJ_TEST_ATOM(dbj::str::remove_all_subs(u"abra ka dabra"sv, u"ab"sv));

	const  std::u32string
		new_u32string
		= DBJ_TEST_ATOM(dbj::str::remove_all_subs(U"abra ka dabra"sv, U"ab"sv));
}

DBJ_TEST_UNIT(dbj_string_util_lowerize) {

	using namespace std::literals;

	// optimal defualt length is
	auto ssos = DBJ_TEST_ATOM( dbj::str::small_string_optimal_size ) ;
	auto optimal_str = DBJ_TEST_ATOM( dbj::str::optimal<char>() );
	optimal_str = "ABRA KA DABRA";

	const  std::string
		new_string
		= DBJ_TEST_ATOM(dbj::str::lowerize(optimal_str.data(), optimal_str.data() + optimal_str.size()));

	const  std::wstring
		new_wstring
		= DBJ_TEST_ATOM(dbj::str::lowerize(L"ABRA KA DABRA"sv));
}

DBJ_TEST_UNIT(dbj_string_util_ui_string_compare) {

	using namespace std::literals;

	auto test = []( auto s1, auto s2 ) -> bool  {
		return 
			(0 == dbj::str::ui_string_compare(s1.data(), s2.data(),true ))
			+
			(0 == dbj::str::ui_string_compare(s1.data(), s2.data(),false));
	};

    auto r1 = DBJ_TEST_ATOM(test( "ABRA"sv,  "ABRA"sv));
    auto r2 = DBJ_TEST_ATOM(test(L"AbrA"sv, L"ABRA"sv));
    //auto r3 = DBJ_TEST_ATOM(test(u"ABRA"sv, u"AbrA"sv));
    // auto r4 = DBJ_TEST_ATOM(test(U"abra"sv, U"abra"sv));
}


DBJ_TEST_UNIT(dbj_string_util_ui_is_prefix) {

	using namespace std::literals;

	auto test = [](auto s1, auto s2) -> bool {
		return
			dbj::str::is_view_prefix_to_view(s1, s2);
	};

	auto r1 = DBJ_TEST_ATOM(test("ABRA"sv, "ABRA KA DABRA"sv));
	auto r2 = DBJ_TEST_ATOM(test(L"AbrA"sv, L"ABRA ka dabra"sv));
	auto r3 = DBJ_TEST_ATOM(test(u"ABRA"sv, u"AbrA ka DABRA"sv));
	auto r4 = DBJ_TEST_ATOM(test(U"abra"sv, U"abra ka dabra"sv));
}

DBJ_TEST_SPACE_CLOSE