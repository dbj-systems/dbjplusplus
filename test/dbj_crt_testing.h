#pragma once
#include <typeinfo>
#pragma region "dbj stlen and strnlen"
DBJ_TEST_SPACE_OPEN(_dbj_crt_testing_space_)
	/*
	dbj crt caters for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t
	*/
	template< typename T, size_t N>
	inline void strlen_strnlen_test
	(
		const T(&prompt)[N]
	)
	{
		using ::dbj::console::print;
		print("\n\nTesting array of type ", typeid(T).name(), " and of length ", N, "\t");
		print("\n\t", DBJ_NV(dbj::countof(prompt)));
		// native char arrays are using dbj.org "zero time" versions     
		print("\n\t", DBJ_NV(dbj::str::strlen(prompt)));
		print("\n\t", DBJ_NV(dbj::str::strnlen(prompt, dbj::BUFSIZ_)));

		// testing for the T * support 
		auto pointer_tester = [&](auto cptr) 
		{
			using ::dbj::console::print;
			// cptr become a pointer due to the standard decay
			using pointer_to_array = decltype(cptr);

			print("\n\nTesting the support for the ", typeid(pointer_to_array).name(), " pointer to the same array\n");
			// using UCRT strlen
			print("\n\t", DBJ_NV(dbj::str::strlen(cptr)));
			// using UCRT strnlen note: std has no strnlen ...
			print("\n\t", DBJ_NV(dbj::str::strnlen(cptr, dbj::BUFSIZ_)));

			::dbj::str::strnlen(cptr, dbj::BUFSIZ_);
		};

		pointer_tester(prompt);
	}

	DBJ_TEST_UNIT(dbj_strnlen) 
	{
		using ::dbj::console::print;

		char	 promptA[] = "0123456789";
		wchar_t  promptW[] = L"0123456789";
		char16_t prompt16[] = u"0123456789";
		char32_t prompt32[] = U"0123456789";

		strlen_strnlen_test(promptA);
		strlen_strnlen_test(promptW);
		strlen_strnlen_test(prompt16);
		strlen_strnlen_test(prompt32);

		_ASSERTE(dbj::str::strnlen(promptA, BUFSIZ) == 10);
		_ASSERTE(dbj::str::strnlen(promptW, BUFSIZ) == 10);
		_ASSERTE(dbj::str::strnlen(prompt16, BUFSIZ) == 10);
		_ASSERTE(dbj::str::strnlen(prompt32, BUFSIZ) == 10);

	}
	DBJ_TEST_SPACE_CLOSE

#pragma endregion "eof dbj stlen and strnlen"