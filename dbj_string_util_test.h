#pragma once

#include "dbj_string_util.h"

DBJ_TEST_SPACE_OPEN(dbj_string_util)

DBJ_TEST_UNIT(dbj_string_util_replace_inplace) {

	using namespace std::literals;

	const  std::string
		new_string
		= dbj::str::replace_inplace("abra ka dabra"sv, "ka"sv, "malaga"sv);

	const  std::wstring
		new_wstring
		= dbj::str::replace_inplace(L"abra ka dabra"sv, L"ka"sv, L"malaga"sv);

	const  std::u16string
		new_u16string
		= dbj::str::replace_inplace(u"abra ka dabra"sv, u"ka"sv, u"malaga"sv);

	const  std::u32string
		new_u32string
		= dbj::str::replace_inplace(U"abra ka dabra"sv, U"ka"sv, U"malaga"sv);
}

DBJ_TEST_SPACE_CLOSE