#pragma once

DBJ_TEST_SPACE_OPEN(kv_storage_test)


DBJ_TEST_UNIT(" dbj k/v storage test ") {

	dbj::sync::lock_unlock padlock_;

	static INT DBJ_UNUSED(anchor) { 1 };
	auto rez1 = dbj::str::is_prefix(L"abra", L"abra ka dabra");
	auto rez2 = dbj::str::is_prefix(L"abra ka dabra", L"abra");
}

DBJ_TEST_SPACE_CLOSE