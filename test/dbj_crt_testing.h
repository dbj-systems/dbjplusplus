#pragma once
#pragma region "dbj stlen and strnlen"
#ifdef DBJ_TESTING_EXISTS
namespace {
	using dbj::io::print;
	using dbj::io::printex;

	/*
	dbj crt caters for char, wchar_t, char16_t, char32_t
	for details please see https://docs.microsoft.com/en-us/cpp/cpp/char-wchar-t-char16-t-char32-t
	*/
	template< typename T, size_t N>
	DBJ_INLINE void strlen_strnlen_test(
		const T(&prompt)[N])
	{
		printex("\n\nTesting array of type ", typeid(T).name(), " and of length ", N, "\t");
		printex("\n\t", DBJ_NV(dbj::countof(prompt)));
		// char type arrays are using dbj.org "zero time" versions     
		printex("\n\t", DBJ_NV(dbj::strlen(prompt)));
		printex("\n\t", DBJ_NV(dbj::strnlen(prompt, BUFSIZ)));

		// testing for the T * support 
		auto pointer_tester = [](auto cptr) {
			// cptr becomes pointer due to the standard decay of C++ array to pointer
			using pointer_to_array = decltype(cptr);

			printex("\n\nTesting the support for the ", typeid(pointer_to_array).name(), " pointer to the same array\n");
			// using UCRT strlen
			printex("\n\t", DBJ_NV(dbj::strlen(cptr)));
			// using UCRT strnlen note: std has no strnlen ...
			printex("\n\t", DBJ_NV(dbj::strnlen(cptr, BUFSIZ)));
		};

		pointer_tester(prompt);
	}

	DBJ_TEST_CASE("dbj crt") {
		print("\n(c) % by dbj.org, MSVC version: %", YEAR, _MSC_FULL_VER);

		char	 promptA[] = "0123456789";
		wchar_t  promptW[] = L"0123456789";
		char16_t prompt16[] = u"0123456789";
		char32_t prompt32[] = U"0123456789";

		strlen_strnlen_test(promptA);
		strlen_strnlen_test(promptW);
		strlen_strnlen_test(prompt16);
		strlen_strnlen_test(prompt32);

		assert(dbj::strnlen(promptA, BUFSIZ) == 10);
		assert(dbj::strnlen(promptW, BUFSIZ) == 10);
		assert(dbj::strnlen(prompt16, BUFSIZ) == 10);
		assert(dbj::strnlen(prompt32, BUFSIZ) == 10);

	}
}
#endif
#pragma endregion "eof dbj stlen and strnlen"