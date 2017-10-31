#pragma once
/*
  Various tests

  NOTE: generaly it is preffered to have tests in top level anonymous namespaces
        this does not introduce globals but also helps to spot and resolve various namespacing issues

		If this file (and it will) grows long then use region pragmas
*/
#ifdef DBJ_TESTING_EXISTS
#pragma region "dbj win32"
namespace {
	
	using namespace  dbj::win::con;
	typedef typename dbj::win::con::CMD CMD;
	using namespace  dbj::win32::sysinfo;

	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {
		try {
			printex(
				"\n\t\t", DBJ_NV(computer_name()),
				"\n\t\t", DBJ_NV(user_name()),
				"\n\t\t", DBJ_NV(system_directory()),
				"\n\t\t", DBJ_NV(windows_directory())
			);
		}
		catch (...) {
			print( dbj::Exception("Uknown Exception?") );
		}
	};

	inline void test_line(HDC hDC, int sx, int sy, int ex, int ey, Gdiplus::ARGB colr, Gdiplus::REAL wdth)
	{
		/*
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		auto calculated_color = Gdiplus::Color::Color(clr);
		Gdiplus::Pen p(calculated_color, w);
		p.SetEndCap(Gdiplus::LineCap::LineCapRound);
		g.DrawLine(&p, sx, sy, ex, ey);
		*/
		dbj::win32::LINE liner_(hDC, colr, wdth);
		liner_(sx, sy, ex, ey);
	}

	DBJ_TEST_CASE(dbj::nicer_filename("dbj win32 gdi+ line")) {

		HWND this_window = ::GetActiveWindow();
		DBJ_ASSERT(this_window);

		HDC this_hdc = GetDC( this_window );
		DBJ_ASSERT(this_hdc);

		SIZE this_size;
		DBJ_ASSERT(0 != ::GetViewportExtEx(this_hdc, &this_size));

		// Declare and initialize color component values as BYTE types.
		BYTE a = 0xFF;
		BYTE r = 0x80;
		BYTE g = 0x80;
		BYTE b = 0xFF;

		// Create an ARGB value from the four component values.
		ARGB argb = Color::MakeARGB(a, r, g, b);

		test_line(this_hdc, 0, 0, this_size.cx, this_size.cy, argb, 20);

	}
}
#pragma endregion

#endif // DBJ_TESTING_EXISTS
