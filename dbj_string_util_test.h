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

DBJ_TEST_SPACE_CLOSE