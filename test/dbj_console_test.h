#pragma once

DBJ_TEST_SPACE_OPEN(console_)

	using namespace  dbj::console ;

	DBJ_TEST_UNIT(" : dbj console test ONE" ) {

		constexpr wchar_t  doubles[]{ L"║═╚" };
		constexpr wchar_t  singles[]{ L"│─└" };
		constexpr wchar_t  squiglies[]{ L"αß©∂€αß©∂€αß©∂€αß©∂€αß©∂€" };

		auto run_them_through_colours = [&]( auto && text_ ) {
			/*
			here we use them commands
			*/
			dbj::log::print( "\n",
				"\n", painter_command::white, "White\t\t", text_,
				"\n", painter_command::red, "Red\t\t", text_,
				"\n", painter_command::green, "Green\t\t", text_,
				"\n", painter_command::blue, "Blue\t\t", text_,
				"\n", painter_command::bright_red, "Bright Red\t", text_,
				"\n", painter_command::text_color_reset, "Reset\t\t", text_
			);
		};
			run_them_through_colours(doubles);
			run_them_through_colours(singles);
			run_them_through_colours(squiglies);
	}
#if 0
	DBJ_TEST_UNIT(" : dbj wcout test TWO")
	{
/*
http://stackoverflow.com/questions/2492077/output-unicode-strings-in-windows-console-app
*/
		_setmode(_fileno(stdout), _O_U16TEXT);
		std::wcout << L"Testing unicode -- English -- Ελληνικά -- Español." << std::endl
			<< doubles << std::endl
			<< singles << std::endl;

		/*
		apparently must do this
		also make sure NOT to mix cout and wcout in the same executable
		*/
		std::wcout << std::flush;
		fflush(stdout);
		_setmode(_fileno(stdout), _O_TEXT);
	}
#endif

DBJ_TEST_SPACE_CLOSE
