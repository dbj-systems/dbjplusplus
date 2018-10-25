#pragma once

#include <deque>
#include <list>
#include <forward_list>

DBJ_TEST_SPACE_OPEN(string_util_tests )

// todo: pull in the C solution from dbj clib
#ifdef DBJ_USE_STD_STREAMS
DBJ_TEST_UNIT(dbjstrtokenizer)
{
	DBJ_TEST_ATOM(dbj::str::tokenize("prefix mif fix fenix"));
	DBJ_TEST_ATOM(dbj::str::tokenize(L"prefix mif fix fenix"));
}
#endif

DBJ_TEST_UNIT(dbjstroptimal) {

	// capacity and size of os1 is 255
	// it will not do any heap alloc / de-alloc
	// for size < 255
	DBJ_TEST_ATOM(dbj::str::optimal<char>());
	// increase to 1024
	DBJ_TEST_ATOM(dbj::str::optimal<wchar_t>(1024));
	// increase to 512 and stamp with u'='
	DBJ_TEST_ATOM(dbj::str::optimal<char16_t>(512, u'='));
	// increase to 128 and stamp with U'+'
	DBJ_TEST_ATOM(dbj::str::optimal<char32_t>(128, U'+'));
}

DBJ_TEST_UNIT(testingdbjstris_std_string_v)
{
	DBJ_TEST_ATOM(dbj::is_std_string_v<std::string>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<std::wstring>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<std::u16string>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<std::u32string>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<char *>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<const char *>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<char(&)[]>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<char(*)[]>);
	DBJ_TEST_ATOM(dbj::is_std_string_v<char[]>);
}

DBJ_TEST_UNIT(testingdbjstris_std_char_v)
{
	DBJ_TEST_ATOM(dbj::is_std_char_v<char>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<wchar_t>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<char16_t>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<char32_t>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<char *>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<const char *>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<char & >);
	DBJ_TEST_ATOM(dbj::is_std_char_v<char &&>);
	DBJ_TEST_ATOM(dbj::is_std_char_v<const wchar_t >);
}

DBJ_TEST_UNIT(dbjstringutils) {
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


/// <summary>
///  the string meta converter testing
/// </summary>
template<typename converter_type>
void test_conversion(converter_type && the_converter)
{
	using namespace std::string_literals;
	auto arf = std::ref(L"Hello!");

	// DBJ_TEST_ATOM(the_converter);

	// standard string literals
	DBJ_TEST_ATOM(the_converter("\"the\0\0standard string literal\""s));
	DBJ_TEST_ATOM(the_converter(L"\"the\0\0standard string literal\""s));
	DBJ_TEST_ATOM(the_converter(u"\"the\0\0standard string literal\""s));
	DBJ_TEST_ATOM(the_converter(U"\"the\0\0standard string literal\""s));

	// C string literals
	DBJ_TEST_ATOM(the_converter("\"Abra Ca Dabra Alhambra\""));
	DBJ_TEST_ATOM(the_converter(L"\"Abra Ca Dabra Alhambra\""));
	DBJ_TEST_ATOM(the_converter(u"\"Abra Ca Dabra Alhambra\""));
	DBJ_TEST_ATOM(the_converter(U"\"Abra Ca Dabra Alhambra\""));

	// the four standard string types
	DBJ_TEST_ATOM(the_converter(std::string{ "\"Abra Ca Dabra Alhambra\"" }));
	DBJ_TEST_ATOM(the_converter(std::wstring{ L"\"Abra Ca Dabra Alhambra\"" }));
	DBJ_TEST_ATOM(the_converter(std::u16string{ u"\"Abra Ca Dabra Alhambra\"" }));
	DBJ_TEST_ATOM(the_converter(std::u32string{ U"\"Abra Ca Dabra Alhambra\"" }));

	// now let's try the sequence containers

	// the four standard char types in std::array
	DBJ_TEST_ATOM(the_converter(std::array<char, 6>{  'H', 'e', 'l', 'l', 'o', '!'}));
	DBJ_TEST_ATOM(the_converter(std::array<wchar_t, 6>{ L'H', L'e', L'l', L'l', L'o', L'!'}));
	DBJ_TEST_ATOM(the_converter(std::array<char16_t, 6>{ u'H', u'e', u'l', u'l', u'o', u'!'}));
	DBJ_TEST_ATOM(the_converter(std::array<char32_t, 6>{ U'H', U'e', U'l', U'l', U'o', U'!'}));

	// the four standard char types in std::vector
	DBJ_TEST_ATOM(the_converter(std::vector<char>	{  'H', 'e', 'l', 'l', 'o', '!'}));
	DBJ_TEST_ATOM(the_converter(std::vector<wchar_t>	{ L'H', L'e', L'l', L'l', L'o', L'!'}));
	DBJ_TEST_ATOM(the_converter(std::vector<char16_t>{ u'H', u'e', u'l', u'l', u'o', u'!'}));
	DBJ_TEST_ATOM(the_converter(std::vector<char32_t>{ U'H', U'e', U'l', U'l', U'o', U'!'}));

	// the four standard char types in std::deque
	DBJ_TEST_ATOM(the_converter(std::deque<char>	{  'H', 'e', 'l', 'l', 'o', '!'}));
	DBJ_TEST_ATOM(the_converter(std::deque<wchar_t>	{ L'H', L'e', L'l', L'l', L'o', L'!'}));
	DBJ_TEST_ATOM(the_converter(std::deque<char16_t>{ u'H', u'e', u'l', u'l', u'o', u'!'}));
	DBJ_TEST_ATOM(the_converter(std::deque<char32_t>{ U'H', U'e', U'l', U'l', U'o', U'!'}));

	// the four standard char types in std::forward_list
	DBJ_TEST_ATOM(the_converter(std::forward_list<char>	{  'H', 'e', 'l', 'l', 'o', '!'}));
	DBJ_TEST_ATOM(the_converter(std::forward_list<wchar_t>	{ L'H', L'e', L'l', L'l', L'o', L'!'}));
	DBJ_TEST_ATOM(the_converter(std::forward_list<char16_t>{ u'H', u'e', u'l', u'l', u'o', u'!'}));
	DBJ_TEST_ATOM(the_converter(std::forward_list<char32_t>{ U'H', U'e', U'l', U'l', U'o', U'!'}));

	// we can serve stunt men to
	DBJ_TEST_ATOM(the_converter(static_cast<const char(&)[]>("\"Abra Ca Dabra\"")));
	DBJ_TEST_ATOM(the_converter(static_cast<const wchar_t(&)[]>(L"\"Abra Ca Dabra\"")));
	DBJ_TEST_ATOM(the_converter(static_cast<const char16_t(&)[]>(u"\"Abra Ca Dabra\"")));
	DBJ_TEST_ATOM(the_converter(static_cast<const char32_t(&)[]>(U"\"Abra Ca Dabra\"")));

	// then string views, and so on ...
	// now lets test non char containers
	// rightly so, none of this compiles
	// DBJ_SHOW(the_converter(std::array<int, 6>{  1,2,3,4,5,6 }));
	// DBJ_SHOW(the_converter(std::vector<int>{  1, 2, 3, 4, 5, 6 }));
	// DBJ_SHOW(the_converter(std::basic_string<int>{  1, 2, 3, 4, 5, 6 }));
}

DBJ_TEST_UNIT(metaconvertortest) {

	test_conversion(dbj::range_to_string    );
	test_conversion(dbj::range_to_wstring   );
	test_conversion(dbj::range_to_u16string );
	test_conversion(dbj::range_to_u32string );
}

DBJ_TEST_SPACE_CLOSE
