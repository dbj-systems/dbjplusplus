#pragma once
#pragma region "dbj win32"
namespace dbj_testing_space  {
	
	// using namespace  dbj::win::con;
	// typedef typename dbj::console::painter_command CMD;
	using namespace  dbj::win32::sysinfo;

	DBJ_TEST_UNIT(" : dbj various tests ") {
		try {
			dbj::console::print(
				"\n\t\t", DBJ_NV(computer_name()),
				"\n\t\t", DBJ_NV(user_name()),
				"\n\t\t", DBJ_NV(system_directory()),
				"\n\t\t", DBJ_NV(windows_directory())
			);
		}
		catch (...) {
			dbj::console::print( dbj::Exception("Uknown Exception?") );
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
		// as per advice on WARNING C28159 
		wsprintfA(pszNewWindowTitle, "%d/%d", (int)GetTickCount64(), (int)GetCurrentProcessId());
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

	DBJ_TEST_UNIT(" : dbj win32 gdi+ line" ) {

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

			Gdiplus::Rect rect{};
			grfx.GetVisibleClipBounds(&rect);

			if ( (rect.Width > 1) && (rect.Height > 1)) {

				// Declare and initialize color #8080FF
				BYTE a = 0xFF, r = 0x80, g = 0x80, b = 0xFF;

				// Create an ARGB value from the four component values.
				ARGB argb = Color::MakeARGB(a, r, g, b);

				test_line(this_hdc,
					rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom(),
					argb, 20);
			}
		} catch (...) {
			dbj::console::print(dbj::Exception("Exception in " __FUNCSIG__));
		}
		::GdiplusShutdown(gdiplusToken);
	}
}
#pragma endregion

