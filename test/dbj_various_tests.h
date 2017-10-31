#pragma once
/*
  Various tests

  NOTE: generaly it is preffered to have tests in top level anonymous namespaces
        this does not introduce globals but also helps to spot and resolve various namespacing issues

		If this file (and it will) grows long use region's 
*/
#ifdef DBJ_TESTING_EXISTS
#pragma region "dbj win32"
namespace {
	using namespace sysinfo;
	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {
		try {
			dbj::io::printex(
				"\n\t\t", DBJ_NV(computer_name()),
				"\n\t\t", DBJ_NV(user_name()),
				"\n\t\t", DBJ_NV(system_directory()),
				"\n\t\t", DBJ_NV(windows_directory())
			);
		}
		catch (...) {
			dbj::io::printex("\nUknown Exception?");
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
		LINE liner_(hDC, colr, wdth);
		liner_(sx, sy, ex, ey);
	}

	DBJ_TEST_CASE(dbj::nicer_filename("dbj win32 gdi+ line")) {
	}
}
#pragma endregion

#endif // DBJ_TESTING_EXISTS
