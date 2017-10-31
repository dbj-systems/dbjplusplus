#pragma once

#pragma region "console testing"
#ifdef DBJ_TESTING_EXISTS
namespace {
	using namespace dbj::win::con;
	typedef typename dbj::win::con::CMD CMD;

	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {

		WideOut wout{};
		const static std::wstring doubles = L"║═╚";
		const static std::wstring singles = L"│─└";
		/*
		here we use them commands through the print()
		*/
		wout.print(
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%",
			CMD::white, "White", doubles, singles,
			CMD::red, "Red", doubles, singles,
			CMD::green, "Green", doubles, singles,
			CMD::blue, "Blue", doubles, singles,
			CMD::bright_red, "Bright Red", doubles, singles,
			CMD::text_color_reset, "Reset", doubles, singles

		);
	}
}
#endif // DBJ_TESTING_EXISTS
#pragma endregion "console testing"
