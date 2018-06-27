#pragma once

DBJ_TEST_SPACE_OPEN(string_util_tests )

DBJ_TEST_UNIT(" dbj str tokenizer") {

	DBJ_TEST_ATOM(dbj::str::tokenize("prefix mif fix fenix"));
	DBJ_TEST_ATOM(dbj::str::tokenize(L"prefix mif fix fenix"));
}

DBJ_TEST_UNIT(" dbj str optimal") {

	// capacity and size of os1 is 255
	// it will not do any heap alloc / de-alloc
	// for size < 255
	DBJ_TEST_ATOM(dbj::str::optimal<char>());
	DBJ_TEST_ATOM(dbj::str::optimal<wchar_t>(1024));
	DBJ_TEST_ATOM(dbj::str::optimal<char16_t>(512, u'='));
	DBJ_TEST_ATOM(dbj::str::optimal<char32_t>(128, U'+'));
}

DBJ_TEST_UNIT(" testing dbj::str::is_std_string_v<>")
{
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::string>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::wstring>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::u16string>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<std::u32string>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<char *>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<const char *>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<char(&)[]>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<char(*)[]>);
	DBJ_TEST_ATOM(dbj::str::is_std_string_v<char[]>);
}

DBJ_TEST_UNIT(" testing dbj::str::is_std_char_v<>")
{
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<wchar_t>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char16_t>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char32_t>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char *>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<const char *>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char & >);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<char &&>);
	DBJ_TEST_ATOM(dbj::str::is_std_char_v<const wchar_t >);
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
