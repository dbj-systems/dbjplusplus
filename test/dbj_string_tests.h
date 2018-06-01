#pragma once
#ifdef DBJ_TESTING_EXISTS
namespace dbj_testing_space  {
	DBJ_TEST_UNIT(" dbj string comparisons ") {
		auto rez =
			dbj_ordinal_string_compareW(L"A", L"A", true);
		auto zer =
			dbj::str::ui_string_compare(L"abra babra dabra", L"ABRA babra DABRA", true);
	}
}
#endif