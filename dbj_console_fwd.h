#pragma once

#ifndef  UNICODE
#error __FILE__  requires unicode
#endif // ! UNICODE

#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif
#define STRICT
#define NOSERVICE
// avoid min/max macros 
#define NOMINMAX
#include <windows.h>
#endif


#if !defined(_CONSOLE)
#pragma message ( "#############################################################" )
#pragma message ( DBJ_CONCAT( "File: ", __FILE__) )
#pragma message ( DBJ_CONCAT( "Line: ",  DBJ_EXPAND(__LINE__)))
#pragma message ("This is probably not a console app?")
#pragma message ( "#############################################################" )
#endif


#include <string_view>
#include "dbj_crt.h"
#include "dbj_traits.h"
#include "dbj_win32.h"
#include "dbj_string_util.h"


namespace dbj::console {



	constexpr inline char    space = ' ', prefix = '{', suffix = '}', delim = ',' , nl = '\n' ;
	constexpr inline wchar_t wspace = L' ', wprefix = L'{', wsuffix = L'}', wdelim = L',', wnl = L'\n';

	constexpr inline const char    
		* space_str{ " " }, *prefix_str{ "{" }, *suffix_str{ "}" }, *delim_str{ "," }, * nl_str{ "\n" };
	constexpr inline const wchar_t 
		* wspace_str{ L" " }, *wprefix_str{ L"{" }, *wsuffix_str{ L"}" }, *wdelim_str{ L"," }, * wnl_str{ L"\n" };


#pragma region "fonts"
	/*
	Apparently "Terminal" is the magical font name that "always works"
	but gives raster fonts
	Otherwise I am yet to find a font name which does not work
	provided it is installed on the system
	so use safe font names

	https://stackoverflow.com/a/33672503/5560811

	*/
	constexpr static const wchar_t * const SafeFontNames[]{
		L"Lucida Console",
		L"Arial", L"Calibri", L"Cambria", L"Cambria Math", L"Comic Sans MS", L"Courier New",
		L"Ebrima", L"Gadugi", L"Georgia",
		/* "Javanese Text Regular Fallback font for Javanese script", "Leelawadee UI", */
		/*
		"Malgun Gothic", "Microsoft Himalaya", "Microsoft JhengHei",
		"Microsoft JhengHei UI", "Microsoft New Tai Lue", "Microsoft PhagsPa",
		"Microsoft Tai Le", "Microsoft YaHei", "Microsoft YaHei UI",
		"Microsoft Yi Baiti", "Mongolian Baiti", "MV Boli", "Myanmar Text",
		"Nirmala UI",
		*/
		L"Segoe MDL2 Assets", L"Segoe Print", L"Segoe UI", L"Segoe UI Emoji",
		L"Segoe UI Historic", L"Segoe UI Symbol", L"SimSun", L"Times New Roman",
		L"Trebuchet MS", L"Verdana", L"Webdings", L"Wingdings", L"Yu Gothic",
		L"Yu Gothic UI"
	};
	//
	// choice of console font is critical
	// on my machine this font produces 
	// almost all chars from various 
	// non english languages
	// on your system that might be different
	// check what gives results 
	// by using console properties dialogue
	// while non-displayable chars are shown
	// on the console
	//
	constexpr inline const wchar_t * const extended_chars_good_font{ L"SimSun-ExtB" };
	constexpr inline const wchar_t * const default_font{ SafeFontNames[0] };
	inline bool set_font(const wchar_t * font_name = default_font, short height_ = 20) {
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof cfi;
		cfi.nFont = 0;
		cfi.dwFontSize.X = 0;
		cfi.dwFontSize.Y = height_;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		//
		::wcscpy_s(cfi.FaceName, LF_FACESIZE, font_name);
		return ::SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	}

	inline bool set_extended_chars_font(
		const wchar_t * font_name = extended_chars_good_font, short height_ = 20
	)
	{
		auto retval = set_font(font_name, height_);
		DBJ_VERIFY(retval);
		return retval;
	}

	inline CONSOLE_FONT_INFOEX get_current_font_(HANDLE console_handle_)
	{
		static HANDLE handle_ = console_handle_; // console_engine_.handle();
		static CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof cfi;
		cfi.nFont = 0;
		cfi.dwFontSize.X = 0;
		cfi.dwFontSize.Y = 0;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;
		auto DBJ_MAYBE(wmemset_rez) = std::wmemset(cfi.FaceName, '?', LF_FACESIZE);

		BOOL rez = ::GetCurrentConsoleFontEx(
			handle_,
			FALSE,
			&cfi
		);

		if (rez == 0) {
			auto lems = dbj::win32::getLastErrorMessage(
				__FILE__ "(" DBJ_EXPAND(__LINE__) ") dbj console get_font_name() failed -- "
			);
			throw dbj::exception(lems);
		}
		return cfi;
	}
#pragma endregion

	/* 
	interface to the console 
	for the specific purpose of out-put
	where out() is based on HANDLE and wide chars 
	native to windows and std::wstring 
	*/
	struct IConsole {
		/* what code page is in use */
		virtual const unsigned code_page() const = 0;

		virtual HANDLE handle() const = 0 ;
		// const non-ref argument
		virtual void out( std::wstring_view wp_) const  = 0;
		// fast low level
		// from --> to, must be a sequence
		virtual void out( const wchar_t * from,  const wchar_t * to) const = 0;

		// http://www.gotw.ca/publications/mill18.htm#Notes
		virtual ~IConsole() {}
	};

/*
printer is console single user
*/
	class Printer final {

		mutable IConsole * console_{};

		Printer(IConsole * another_console_)
			: console_(another_console_)
		{
			_ASSERTE(another_console_);
		}

	public:
		// to be implemented where IConsole implementation is visible
		friend inline const Printer & printer_instance();

		IConsole const & cons() const noexcept 
		{ 
			_ASSERTE(this->console_ != nullptr); return (*this->console_); 
		}

		Printer() = default;
		Printer(const Printer &) = delete;
		Printer & operator = (const Printer &) = delete;
		Printer(Printer &&) = default;
		Printer & operator = (Printer &&) = default;
		~Printer() { this->console_ = nullptr; }

		void char_to_console(const char * char_ptr) const noexcept {
			_ASSERTE(char_ptr);
			if (char_ptr[0] == (char)0) { // probably an error
				return;
			}
			const std::wstring wstr_{ dbj::range_to_wstring(char_ptr) };
			cons().out(wstr_.data(), wstr_.data() + wstr_.size());
		}


		void wchar_to_console(const wchar_t  * char_ptr) const noexcept {
			_ASSERTE(char_ptr);
			if (char_ptr[0] == (wchar_t)0) { // probably an error
				return;
			}
			const std::wstring wstr_{ char_ptr };
			cons().out(wstr_.data(), wstr_.data() + wstr_.size());
		}

		template <typename ... Args>
		void printf(wchar_t const * const message, Args ... args) const noexcept
		{
			static constexpr size_t buff_siz_ = DBJ::BUFSIZ_;
			wchar_t buffer[buff_siz_]{};
			auto R = _snwprintf_s(buffer, _countof(buffer), _countof(buffer), message, (args) ...);
			_ASSERTE(-1 != R);
			wchar_to_console(buffer);
		}

		template <typename ... Args>
		void printf(const char * const message, Args ... args) const noexcept
		{
			static constexpr size_t buff_siz_ = DBJ::BUFSIZ_;
			char buffer[buff_siz_]{};
			auto DBJ_MAYBE(R) = _snprintf_s(buffer, _countof(buffer), _countof(buffer), message, (args) ...);
			_ASSERTE(-1 != R);
			char_to_console(buffer);
		}

	}; // Printer



	typedef enum class CODE : UINT {
		page_1252 = 1252,   // western european windows
		page_65001 = 65001, // utf8
		page_1200 = 1200,  // utf16?
		page_1201 = 1201   // utf16 big endian?
	} CODE_PAGE;

	constexpr inline const CODE_PAGE default_code_page = CODE::page_65001;

	struct WideOut;

} // namespace dbj::console 