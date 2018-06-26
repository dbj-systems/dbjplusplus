#pragma once

DBJ_TEST_SPACE_OPEN(string_util_tests )

DBJ_TEST_UNIT(" dbj str optimal") {

	// capacity and size of os1 is 255
	// it will not do any heap alloc / de-alloc
	// for size < 255
	auto os1 = DBJ_TEST_ATOM(dbj::str::optimal<char>());
	auto os2 = DBJ_TEST_ATOM(dbj::str::optimal<wchar_t>(1024));
	auto os3 = DBJ_TEST_ATOM(dbj::str::optimal<char16_t>(512, u'='));
	auto os4 = DBJ_TEST_ATOM(dbj::str::optimal<char32_t>(128, U'+'));
}

DBJ_TEST_UNIT(" testing dbj::str::is_std_string_v<>")
{
	auto rez_0 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::string>);
	auto rez_1 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::wstring>);
	auto rez_2 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::u16string>);
	auto rez_3 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::u32string>);
	auto rez_4 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<char *>);
	auto rez_5 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<const char *>);
	auto rez_6 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<char(&)[]>);
	auto rez_7 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<char(*)[]>);
	auto rez_8 = DBJ_TEST_ATOM(dbj::str::is_std_string_v<char[]>);
}

DBJ_TEST_UNIT(" testing dbj::str::is_std_char_v<>")
{
	auto rez_0 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char>);
	auto rez_1 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<wchar_t>);
	auto rez_2 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char16_t>);
	auto rez_3 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char32_t>);
	auto rez_4 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char *>);
	auto rez_5 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<const char *>);
	auto rez_6 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char & >);
	auto rez_7 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<char &&>);
	auto rez_8 = DBJ_TEST_ATOM(dbj::str::is_std_char_v<const wchar_t >);
}

DBJ_TEST_UNIT(" dbj string utils ") {
	//auto rez =
		DBJ_TEST_ATOM(
			0 == dbj_ordinal_string_compareW(L"A", L"A", true));
	//auto zer =
		DBJ_TEST_ATOM(
			dbj::str::ui_string_compare(L"abra babra dabra", L"ABRA babra DABRA", true));

	//auto rez_2 = 
		DBJ_TEST_ATOM(
		dbj::str::is_prefix(L"PREFIX", L"PREFIX MIX FIX"));

	//auto rez_3 = 
		DBJ_TEST_ATOM(
		dbj::str::is_prefix(L"MINIX", L"PREFIX MIX FIX"));

	
	//this will not compile, because one
	//can not lowerize string literals
	//auto rez_4 = DBJ_TEST_ATOM(dbj::str::lowerize(L"PREFIX MIX FIX" ));
	

	wchar_t wide_charr_arr[]{ L"PREFIX MIX FIX" };
	//auto rez_4 = 
	   DBJ_TEST_ATOM(dbj::str::lowerize(wide_charr_arr));

}

DBJ_TEST_SPACE_CLOSE(string_util_tests)
