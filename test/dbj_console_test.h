#pragma once

#pragma region "console testing"
#ifdef DBJ_TESTING_EXISTS
namespace dbj_testing_space  {
	using namespace  dbj::win::con ;

	DBJ_TEST_UNIT(" : dbj console test") {

		const static std::wstring squiglies = L"αß©∂€αß©∂€αß©∂€αß©∂€αß©∂€";
		/*
		here we use them commands 
		*/
		dbj::print(
			"\n", painter_command::white,				"White\t\t",		squiglies,
			"\n", painter_command::red,					"Red\t\t",			squiglies,
			"\n", painter_command::green,				"Green\t\t",		squiglies,
			"\n", painter_command::blue,				"Blue\t\t",			squiglies,
			"\n", painter_command::bright_red,			"Bright Red\t",		squiglies,
			"\n", painter_command::text_color_reset,	"Reset\t\t",		squiglies
		);
	}
}
#endif // DBJ_TESTING_EXISTS
#pragma endregion "console testing"
