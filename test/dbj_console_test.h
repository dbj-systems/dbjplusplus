#pragma once

#pragma region "console testing"
#ifdef DBJ_TESTING_EXISTS
namespace {
	using namespace  dbj::win::con ;

	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {

		const static std::wstring squiglies = L"αß©∂€αß©∂€αß©∂€αß©∂€αß©∂€";
		/*
		here we use them commands through the printex()
		*/
		dbj::printex(
			"\n", CMD::white,				"White\t\t",		squiglies, 
			"\n", CMD::red,					"Red\t\t",			squiglies,
			"\n", CMD::green,				"Green\t\t",		squiglies,
			"\n", CMD::blue,				"Blue\t\t",			squiglies,
			"\n", CMD::bright_red,			"Bright Red\t",		squiglies,
			"\n", CMD::text_color_reset,	"Reset\t\t",		squiglies
		);
	}
}
#endif // DBJ_TESTING_EXISTS
#pragma endregion "console testing"
