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

#pragma region not in use

#if 0
namespace internal {
	constexpr char space = ' ', prefix = '{', suffix = '}', delim = ',';

	/*
	anything that has begin and end
	NOTE: that includes references to native arrays
	*/
	inline auto print_range = [](const auto & range) {

		// not requiring  range.size();
		// thus can do native arrays
		std::size_t argsize =
			static_cast<std::size_t>(
				std::distance(
					std::begin(range), std::end(range)
				)
				);

		if (argsize < 1) return;

		std::size_t arg_count{ 0 };

		auto delimited_out = [&](auto && val_) {
			out__(val_);
			if ((arg_count++) < (argsize - 1)) out__(delim);
		};

		out__(prefix); out__(space);
		for (auto item : range) {
			delimited_out(item);
		}
		out__(space); out__(suffix);
	};

	/* also called from void out__(...) functions for compound types. e.g. void out__(tuple&) */
	// template<typename... Args >
	inline	auto print_varargs = [](
		auto && first,
		auto && ... args
		)
	{
		constexpr std::size_t pack_size = sizeof...(args);

		std::size_t arg_count = 0;

		auto delimited_out = [&](auto && val_)
		{
			out__(val_);
			if (arg_count < pack_size) {
				out__(delim);
			}
			arg_count += 1;
		};

		out__(prefix); out__(space);

		delimited_out(first);

		if constexpr (pack_size > 0) {
			(delimited_out(args), ...);
		}

		out__(space); out__(suffix);
	};

} // internal nspace
#endif
  /*

  console.out__(...) is the only method to output to a console

  this is the special out__ that does not use the console output class
  but painter commander

  Thus we achieved a decoupling of console and painter
	  template<
		  typename PC ,
		  typename std::enable_if_t<
		  std::is_same_v< std::decay_t<PC>, painter_command>
		  , int> = 0
	  >
	  inline void out__	( const PC & cmd_ )
	  {
		  painter_commander().execute(cmd_);
	  }
	  */
  /*
	  template<
		  typename N ,
		  typename std::enable_if_t<std::is_arithmetic_v<std::decay_t<N>>, int > = 0
	  >
	  inline void out__	( const N & number_	)
	  {
		  // static_assert( std::is_arithmetic<N>::value, "type N is not a number");
		  console_.out__(std::to_wstring(number_));
	  }

	  template< typename B,
		  typename std::enable_if_t< dbj::is_bool_v<B>, int > = 0 >
	  inline void out__
	  ( const B & val_ )
	  {
		  console_.out__((true == val_ ? L"true" : L"false"));
	  }
	  */
	  /*
	   output the standard string
	   enable only if it is made out__
	   of standard chars
	  template< typename T >
	  inline void out__ ( const std::basic_string<T> & s_ ) {

		  static_assert(dbj::str::is_std_char_v<T>);

		  if (!s_.empty())
			  console_.out__(s_);
	  }
	  */

	  /*
	  output the standard string view
	  enable only if it is made out__
	  of standard chars

	  template<typename T	>
	  inline void out__ ( std::basic_string_view<T>  s_ )
	  {
		  static_assert( dbj::str::is_std_char_v<T> );

		  if (!s_.empty())
			  console_.out__(s_);
	  }

	  inline void out__(const std::string & s_) {
		  if (!s_.empty())
			  console_.out__(s_);
	  }

	  inline void out__(const std::u16string  & s_) {
		  if (!s_.empty())
			  console_.out__(s_);
	  }

	  inline void out__(const std::u32string  & s_) {
		  if (!s_.empty())
			  console_.out__(s_);
	  }
  */

  /*
	  inline void out__(const std::string_view sv_) {
		  if (sv_.empty()) return;
		  console_.out__(sv_);
	  }

	  inline void out__(const std::wstring_view sv_) {
		  if (sv_.empty()) return;
		  console_.out__(sv_);
	  }

	  inline void out__(const std::u16string_view sv_) {
		  if (sv_.empty()) return;
		  console_.out__(sv_);
	  }

	  inline void out__(const std::u32string_view sv_) {
		  if (sv_.empty()) return;
		  console_.out__(sv_);
	  }

	  implement for these when required

  template<typename T, size_t N>
  inline void out__(const T(*arp_)[N]) {
	  using arf_type = T(&)[N];
	  arf_type arf = (arf_type)arp_;
	  internal::print_range(arf);
  }


  template<size_t N>
  inline void out__(const char(&car_)[N]) {
	  console_.out__(
		  std::string_view(car_, car_ + N)
	  );
  }

  template<size_t N>
  inline void out__(const wchar_t(&wp_)[N]) {
	  console_.out__(std::wstring(wp_, wp_ + N));
  }

  inline void out__(const char * cp) {
	  _ASSERTE(cp != nullptr);
	  console_.out__(std::string(cp));
  }

  inline void out__(const wchar_t * cp) {
	  _ASSERTE(cp != nullptr);
	  console_.out__(std::wstring(cp));
  }

  inline void out__(const char16_t * cp) {
	  _ASSERTE(cp != nullptr);
	  console_.out__(std::u16string(cp));
  }

  inline void out__(const char32_t * cp) {
	  _ASSERTE(cp != nullptr);
	  console_.out__(std::u32string(cp));
  }

  inline void out__(const wchar_t wp_) {
	  console_.out__(std::wstring(1, wp_));
  }

  inline void out__(const char c_) {
	  char str[] = { c_ , '\0' };
	  console_.out__(std::wstring(std::begin(str), std::end(str)));
  }

  inline void out__(const char16_t wp_) {
	  console_.out__(std::u16string{ 1, wp_ });
  }

  inline void out__(const char32_t wp_) {
	  console_.out__(std::u32string{ 1, wp_ });
  }
  */
  /*
  now we will deliver out__() overloads for "compound" types using the ones above
  made for intrinsic types
  ------------------------------------------------------------------------
  output the exceptions

	  output array of T
	  template <typename T, size_t N >
		  inline void out__( const T (&carr) [N], typename std::enable_if_t<
			  std::is_array_v< std::remove_cv_t<T>[N] >
			  , int > = 0 )
	  {
		  static_assert(N > 1);
		  internal::print_range((T(&)[N])carr);
	  }

	  output array of T *
	  template <typename T, size_t N,
		  typename actual_type = std::remove_cv_t< std::remove_pointer_t<T> >
	  >
		  inline void out__(const T(*carr)[N], typename std::enable_if_t<
			  std::is_array_v< std::remove_cv_t<T>[N] >
			  , int > = 0 )
	  {
		  static_assert(N > 1);
		  internal::print_range((T(&)[N])carr);
	  }

	  output std char type
	  template < typename T >
		  inline void out__(const T & chr,
			  typename std::enable_if_t< dbj::is_std_char_v<T>
			  , int > = 0)
	  {
		  using actual_type = std::remove_cv_t< T >;
		  actual_type car[]{chr};
			  console_.out__(chr);
	  }
	  output pointer to std char type
	  template < typename T,
		  typename actual_type = std::remove_cv_t< std::remove_pointer_t<T> >,
		  typename std::enable_if_t<
		  dbj::is_std_char_v<actual_type>
		  , int > = 0
	  >
	  inline void out__(const T * ptr)
	  {
		  _ASSERTE(ptr != nullptr);
		  using actual_type = std::remove_cv_t< std::remove_pointer_t<T> >;
			  console_.out__(std::basic_string<T>{ptr});
	  }

	  */
	  /*
		  inline auto print = [] ( const auto & first_param, auto && ... params)
		  {
			   out(first_param);

			  // if there are  more params
			  if constexpr (sizeof...(params) > 0) {
				  // recurse
				  print(params...);
			  }
			  return print;
		  };
	  */
#pragma endregion