#pragma once
#include "dbj_console_fwd.h"
#include <io.h>
#include <fcntl.h>

namespace dbj::console {

#pragma region WideOut
	/*
	The "core of the trick"

	SetConsoleOutputCP(CP_UTF8);
	_setmode(_fileno(stdout), _O_U8TEXT);
	wprintf(L"UTF-8 display: %s\r\n", uname);   //uname is a wchar_t*

	Windows "native" unicode is UTF-16
	Be warned than proper implementation of UTF-8 related code page did not happen
	before W7 and just *perhaps* it is full on W10
	See :	http://www.dostips.com/forum/viewtopic.php?t=5357
	Bellow is not FILE * but HANDLE based output.
	It also uses #define CP_UTF8 65001, as defined in winnls.h
	This two are perhaps why this almost always works.

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd374122(v=vs.85).aspx

	Even if you get your program to write UTF16 correctly to the console,
	Note that the Windows console isn't Unicode friendly and may just show garbage.
	*/
	struct WideOut final : public IConsole
	{
		mutable		HANDLE output_handle_;
		mutable		UINT   previous_code_page_;
		mutable		UINT	code_page_{};

	public:

		static constexpr CODE DEFAULT_CODEPAGE_ = CODE::page_65001 ;

		WideOut(CODE CODEPAGE_ = DEFAULT_CODEPAGE_) noexcept
			: code_page_((UINT)CODEPAGE_)
			, output_handle_(::GetStdHandle(STD_OUTPUT_HANDLE))
			, previous_code_page_(::GetConsoleOutputCP())
		{
			//this->output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
			_ASSERTE(INVALID_HANDLE_VALUE != this->output_handle_);
			// previous_code_page_ = ::GetConsoleOutputCP();
			_ASSERTE(0 != ::SetConsoleOutputCP(code_page_));
			/*			TODO: GetLastError()			*/
			// apparently for a good measure one has to do this too ...
			::SetConsoleCP(code_page_);
			// we do NOT use file handlers but
			// this is REALLY important to do
			_setmode(_fileno(stdout), _O_U8TEXT);
			// after this guess the right font and you are ok ;)
		}
		// no copying
		WideOut(const WideOut & other) = delete;
		WideOut & operator = (const WideOut & other) = delete;
		// we need this one so we can pass the singleton instance out
		WideOut(WideOut && other) = default;
		WideOut & operator = (WideOut && other) = default;

		~WideOut()
		{
			auto DBJ_MAYBE(rezult) = ::SetConsoleOutputCP(previous_code_page_);
			// apparently for a good measure one has to do this too ...
			::SetConsoleCP(previous_code_page_);
			_ASSERTE(0 != rezult);
		}

		/* what code page is used */
		const unsigned code_page() const noexcept override { return this->code_page_; }
		/* out__ is based on HANDLE and std::wstring */
		HANDLE handle() const override  { 

			if (output_handle_ == INVALID_HANDLE_VALUE) {
				auto lems = dbj::win32::getLastErrorMessage(
					DBJ_ERR_PROMPT("INVALID_HANDLE_VALUE?")
				);
				throw dbj::exception(lems);
			}
#ifdef _DEBUG
			DWORD lpMode{};
			if (0 == GetConsoleMode(output_handle_, &lpMode)) {
				auto lems = dbj::win32::getLastErrorMessage(
					DBJ_ERR_PROMPT("INVALID_HANDLE?")
				);
				throw dbj::exception(lems);
			}
#endif
				return this->output_handle_;
		}
    	// from --> to, must be a sequence
		// this is the *fastest* method
		void out( const wchar_t * from,  const wchar_t * to) const override
		{
#ifndef _DEBUG
			static
#endif // !_DEBUG
				const HANDLE output_h_ = 
#if 1
				this->handle();
#else
			::GetStdHandle(STD_OUTPUT_HANDLE);
			_ASSERTE(output_h_ != INVALID_HANDLE_VALUE);
#endif

			_ASSERTE(from != nullptr);
			_ASSERTE(to != nullptr);
			_ASSERTE(from != to);

			std::size_t size = std::distance(from, to);
			_ASSERTE( size > 0 );
#if 1
			// this is *crucial*
			// otherwise ::WriteConsoleW will fail
			// in a debug builds on unpredicatble
			// and rare ocasions
			// effectively set the last error to 0
			(void)dbj::win32::last_error();
#endif
			auto retval = ::WriteConsoleW
			(
				output_h_,
				from,
				static_cast<DWORD>(size), NULL, NULL
			);
			if (retval != 0) {
				int last_win32_err = dbj::win32::last_error();
				if (last_win32_err > 0) {
					auto lems = dbj::win32::getLastErrorMessage(
						__FILE__ "(" DBJ_EXPAND(__LINE__) ") -- "
						, last_win32_err
					);
#ifndef _DEBUG
					throw dbj::exception(lems);
#else
					dbj::TRACE("\n%s\n", lems.c_str());
#endif
				}
			}
		}

		/* as dictated by the interface implemented */
		inline void out(const std::wstring_view wp_) const override
		{
			out(wp_.data(), wp_.data() + wp_.size());
		}

		/*
		here we hide the single application wide 
		IConsole implementation instance
		*/
		static WideOut & instance( 
			CODE_PAGE const & code_page = default_code_page
		)
		{
			static WideOut single_instance
				= [&]() -> WideOut {
				// this is anonymous lambda called only once
				return { code_page };
			}(); // call immediately
			return single_instance;
		};

	}; // WideOut


	inline WideOut & console_engine_ = WideOut::instance();

	inline std::wstring get_font_name()
	{
		HANDLE handle_ = console_engine_.handle();
		CONSOLE_FONT_INFOEX cfi = get_current_font_(handle_);
		return { cfi.FaceName };
	}

	/* 
	do we need this?
	inline HANDLE  HANDLE_{ console_.handle() };
	*/

	/* this is Printer's friend*/
	inline const Printer & printer_instance() 
	{
		static Printer single_instance
			= [&]() -> Printer {
			// this is anonymous lambda called only once
			return { &console_engine_ };
		}(); // call immediately
		return single_instance;
	}

#pragma endregion 

	inline void paint(const painter_command & cmd_) {
		painter_commander().execute(cmd_);
	}

	namespace config {

		/*
		TODO: usable interface for users to define this
		*/
		inline const bool & instance()
		{
			static auto configure_once_ = []() -> bool
			{
				auto font_name_ = L"Lucida Console";
				auto code_page_ = ::dbj::console::WideOut::instance().code_page();
				try {
					// TODO: switch code page on a single running instance
					// auto new_console[[maybe_unused]] = con::switch_console(code_page_);

					::dbj::console::set_font(font_name_);
					DBJ::TRACE(L"\nConsole code page set to %d and font to: %s\n"
						, code_page_, font_name_
					);

					// and now the really crazy and important measure 
					// for Windows console
					::system("chcp 65001");
					DBJ::TRACE(L"\nConsole chcp 65001 done\n");

				}
				catch (...) {
					// can happen before main()
					// and user can have no terminators set up
					// so ...
					dbj::wstring message_ = dbj::win32::getLastErrorMessage(__FUNCSIG__);
					DBJ::TRACE(L"\nException %s", message_.data());
					throw dbj::exception(message_);
				}
				return true;
			}();
			return configure_once_;
		} // instance()

		inline const bool & single_start = instance();

	} // config

} // dbj::console

/*
  Copyright 2017,2018 by dbj@dbj.org

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http ://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  */
