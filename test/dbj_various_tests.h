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
	
	// using namespace  dbj::win::con;
	typedef typename dbj::win::con::CMD CMD;
	using namespace  dbj::win32::sysinfo;

	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {
		try {
			dbj::printex(
				"\n\t\t", DBJ_NV(computer_name()),
				"\n\t\t", DBJ_NV(user_name()),
				"\n\t\t", DBJ_NV(system_directory()),
				"\n\t\t", DBJ_NV(windows_directory())
			);
		}
		catch (...) {
			dbj::print( dbj::Exception("Uknown Exception?") );
		}
	};
	/*
	inspired by
	https://support.microsoft.com/en-us/help/124103/how-to-obtain-a-console-window-handle-hwnd
	*/
	inline HWND get_console_hwnd(void)
	{
		constexpr unsigned MY_BUFSIZE = 1024; // Buffer size for console window titles.
		HWND hwndFound;         // This is what is returned to the caller.
		char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated WindowTitle.
		char pszOldWindowTitle[MY_BUFSIZE]; // Contains original WindowTitle.

		// Fetch current window title.
		GetConsoleTitleA(pszOldWindowTitle, MY_BUFSIZE);
		// Format a "unique" NewWindowTitle.
		wsprintfA(pszNewWindowTitle, "%d/%d", GetTickCount(), GetCurrentProcessId());
		// Change current window title.
		SetConsoleTitleA(pszNewWindowTitle);
		// Ensure window title has been updated.
		Sleep(40);
		// Look for NewWindowTitle.
		hwndFound = FindWindowA(NULL, pszNewWindowTitle);
		// Restore original window title.
		SetConsoleTitleA(pszOldWindowTitle);

		return(hwndFound);
	}

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

		// Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;

		try {
			HWND this_window = get_console_hwnd();
			DBJ_ASSERT(this_window);
			// Initialize GDI+.
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			Gdiplus::Graphics grfx(this_window);

			HDC this_hdc = grfx.GetHDC();
			DBJ_ASSERT(this_hdc);

			// Set the clipping region.
			grfx.SetClip(Rect(100, 100, 200, 100));

			Gdiplus::Rect rect;
			grfx.GetVisibleClipBounds(&rect);

			// Declare and initialize color #8080FF
			BYTE a = 0xFF, r = 0x80, g = 0x80, b = 0xFF;

			// Create an ARGB value from the four component values.
			ARGB argb = Color::MakeARGB(a, r, g, b);

			test_line(this_hdc,
				rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(),
				argb, 20);
		} catch (...) {
			dbj::print(dbj::Exception("Exception in " __FUNCSIG__));
		}
		::GdiplusShutdown(gdiplusToken);
	}
}
#pragma endregion

#endif // DBJ_TESTING_EXISTS
