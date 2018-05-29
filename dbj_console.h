#pragma once
// windows.h required here
#pragma region "Console Interfaces"
#include "dbj_crt.h"
#include "dbj_win32.h"
/*
#include "dbj_console_painter.h"
*/
namespace dbj {
	namespace win {
		namespace con {
			/* interface to the wide char console */
			__interface IConsole {
				/* what code page is used */
				const unsigned code_page() const;
				/* out is based on HANDLE and std::wstring */
				HANDLE handle() const;
				void out(const std::wstring & wp_) const ;
			};
		}
	}
}
#pragma endregion "Console Interfaces"

namespace dbj {
namespace win {
namespace con {
#pragma region "fonts"
	/*
	Apparently "Terminal" is the magical font name that "always works" 
	but gives raster fonts
	Otherwise I am yet to find a font name which does not work
	provided it is installed on the system
	so use safe font names

	https://stackoverflow.com/a/33672503/5560811

	*/

	constexpr static const char * const SafeFontNames [] {
		"Arial", "Calibri", "Cambria", "Cambria Math", "Comic Sans MS", "Courier New",
		"Ebrima", "Gadugi", "Georgia",
		/* "Javanese Text Regular Fallback font for Javanese script", "Leelawadee UI", */
		"Lucida Console", 
		/*
		"Malgun Gothic", "Microsoft Himalaya", "Microsoft JhengHei",
		"Microsoft JhengHei UI", "Microsoft New Tai Lue", "Microsoft PhagsPa",
		"Microsoft Tai Le", "Microsoft YaHei", "Microsoft YaHei UI",
		"Microsoft Yi Baiti", "Mongolian Baiti", "MV Boli", "Myanmar Text",
		"Nirmala UI", 
		*/
		"Segoe MDL2 Assets", "Segoe Print", "Segoe UI", "Segoe UI Emoji",
		"Segoe UI Historic", "Segoe UI Symbol", "SimSun", "Times New Roman",
		"Trebuchet MS", "Verdana", "Webdings", "Wingdings", "Yu Gothic",
		"Yu Gothic UI"
	};

	inline bool setfont(const wchar_t * font_name = L"Lucida Console", short height_ = 20) {
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
#pragma endregion 

	typedef enum class CODE : UINT { page_1252 = 1252, page_65001 = 65001 } CODE_PAGE ;

#pragma region "WideOut"
// namespace {
	/*
	Windows "native" unicode is UTF-16
	Be warned than proper implementation of UTF-8 related code page did not happen
	before W7 and perhaps it is full on W10
	See :	http://www.dostips.com/forum/viewtopic.php?t=5357
	Bellow is not FILE * but HANDLE based output.
	It also uses #define CP_UTF8 65001, as defined in winnls.h
	This two are perhaps why this almost always works.

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd374122(v=vs.85).aspx

	Even if you get your program to write UTF16 correctly to the console,
	Note that the Windows console isn't UTF16 friendly and may just show garbage.
	*/
	struct __declspec(novtable)	WideOut final
		: public dbj::win::con::IConsole
	{
		mutable		HANDLE output_handle_;
		mutable		UINT   previous_code_page_;
		mutable		UINT	code_page_{};
	public:
		WideOut(CODE CODEPAGE_ = CODE::page_1252 ) noexcept
			: code_page_((UINT)CODEPAGE_)
			, output_handle_ (::GetStdHandle(STD_OUTPUT_HANDLE))
			, previous_code_page_ (::GetConsoleOutputCP())
		{
			//this->output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
			_ASSERTE(INVALID_HANDLE_VALUE != this->output_handle_);
			// previous_code_page_ = ::GetConsoleOutputCP();
			_ASSERTE(0 != ::SetConsoleOutputCP(code_page_));
			/*			TODO: GetLastError()			*/
		}

		WideOut(const WideOut & other) {
			output_handle_ = other.output_handle_;
			previous_code_page_ = other.previous_code_page_;
			code_page_ = other.code_page_;
		}

		WideOut & operator = (const WideOut & other ) {
			output_handle_		=  other.output_handle_		;
			previous_code_page_	=  other.previous_code_page_;
			code_page_			=  other.code_page_			;
			return *this;
		}

		~WideOut()
		{
			_ASSERTE(0 != ::SetConsoleOutputCP(previous_code_page_));
			// TODO: should we "relase" this->output_handle_ ?
			/*			TODO: GetLastError()  		*/
		}

		/* what code page is used */
		const unsigned code_page() const { return this->code_page_; }
		/* out is based on HANDLE and std::wstring */
		HANDLE handle() const { return this->output_handle_; }

		/* the default one */
		inline void out(const std::wstring & wp_) const 
		{
			const HANDLE & output_h_ = this->output_handle_;
			_ASSERTE(0 != ::WriteConsoleW(output_h_, wp_.data(),
				static_cast<DWORD>(wp_.size()),	NULL, NULL));
		}

		inline void out(const std::string & ns_) const
		{
			const HANDLE & output_h_ = this->output_handle_;
			_ASSERTE(0 != ::WriteConsoleA(output_h_, ns_.data(),
				static_cast<DWORD>(ns_.size()), NULL, NULL));
		}

		inline void out(const std::u16string  & u16_) const
		{
			this->out(std::wstring{ u16_.begin(), u16_.end() });
		}

		inline void out(const std::u32string  & u32_) const
		{
			this->out(std::wstring{ u32_.begin(), u32_.end() });
		}

	}; // WideOut

	/*
	here we hide the single application wide console instance
	this is single app wide instance
	*/
	// namespace {
#if !defined(_CONSOLE)
#pragma message ( "#############################################################" )
#pragma message ( DBJ_CONCAT( "File: ", __FILE__) )
#pragma message ( DBJ_CONCAT( "Line: ",  DBJ_EXPAND(__LINE__)))
#pragma message ("This is probably not a console app?")
#pragma message ( "#############################################################" )
#endif
		inline auto console_ = WideOut( CODE::page_65001 ) ;
		/* we expose the HANDLE to the print-ing because of future requirements
		wanting to use error handle etc ...
		*/
		inline HANDLE  HANDLE_{ console_.handle() };
	// }

// } //nspace

// we are here in dbj::win::con
// we need to have only a single instance 
inline auto switch_console (dbj::win::con::CODE code_) {
	console_ = dbj::win::con::WideOut(code_);
	return console_;
};

#pragma endregion 
#pragma region "print-ing implementation"
/*
  print-ing implementation+interface is whole inside dbj::win::con::anonymous_name_space
  that makes it acessible to clients. E.g. :

  using dbj::win::con::print ;  print("Hi!");

  besides parts which are not because they are in :

  dbj::win::con
*/
// namespace {
	/*
		console.out(...) is the only method to output to a console

		this is the special out that does not use the console output class
		but painter commander

		Thus we achieved a decoupling of console and painter
	*/
	template<
		typename N, 
		typename = std::enable_if_t<
			std::is_same_v<N, painter_command> 
		> 
	>
	inline void out( N cmd_ ) {
		painter_commander().execute(cmd_);
	}

	inline void paint(painter_command cmd_) {
		painter_commander().execute(cmd_);
	}

	/*
	by using enable_if we make sure this template instances are made
	only for types we want
	*/
	template<typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value > >
	inline void out(const N & number_) {
		// static_assert( std::is_arithmetic<N>::value, "type N is not a number");
		console_.out(std::to_wstring(number_));
	}

	/* here are the out() overloads for intrinsic types */
	inline void out(const std::wstring & ws_) {
		console_.out(ws_);
	}

	inline void out(const std::string & s_) {
		console_.out(s_);
	}

	inline void out(const std::u16string  & s_) {
		console_.out(s_);
	}

	inline void out(const std::u32string  & s_) {
		console_.out(s_);
	}
	inline void out(const std::string_view & sv_) {
		dbj::win::con::out(
			std::string(sv_.data())
		);
	}

	inline void out(const std::wstring_view & sv_) {
		dbj::win::con::out(
			std::wstring(sv_.data())
		);
	}
	/*	implement for these when required
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
	using wstring_view = basic_string_view<wchar_t>; */


	/*
	above is lovely but that will catch more than we hoped for
	for example bool-eans too
	*/
	inline void out(bool val_) {
		console_.out(val_ ? "true" : "false" );
	}


	template<size_t N>
	inline void out(const char (& car_)[N]) {
		console_.out(
			std::string( car_, car_ + N)
		);
	}

	template<size_t N>
	inline void out(const wchar_t(&wp_)[N]) {
		console_.out(std::wstring(wp_, wp_ + N ));
	}

	inline void out(const char * cp) {
		console_.out(std::string(cp));
	}

	inline void out(const wchar_t * cp) {
		console_.out(std::wstring(cp));
	}

	inline void out(const char16_t * cp) {
		console_.out(std::u16string(cp));
	}

	inline void out(const char32_t * cp) {
		console_.out(std::u32string(cp));
	}

	inline void out(const wchar_t wp_) {
		console_.out(std::wstring(1, wp_));
	}

	inline void out(const char c_) {
		char str[] = { c_ };
		console_.out(std::wstring(std::begin(str), std::end(str)));
	}

	inline void out(const char16_t wp_) {
		console_.out(std::u16string{ 1, wp_ } );
	}

	inline void out(const char32_t wp_) {
		console_.out(std::u32string{ 1, wp_ });
	}

	/*
	now we will deliver out() overloads for "compount" types using the ones above
	made for intrinsic types
	------------------------------------------------------------------------
	output the exceptions
	*/

	/* print exception and also color the output red */
	inline void out(const dbj::Exception & x_) {
		paint( painter_command::bright_red );
		// "magic" calls std::wstring casting operator
		// not a good idea?
		console_.out((x_));
		paint( painter_command::text_color_reset );
	}

	inline void out(const std::exception & x_) {
		paint(painter_command::bright_red);
		out( dbj::Exception(x_.what()) );
		paint(painter_command::text_color_reset);
	}

	template<typename T>
	inline void out(const std::variant<T> & x_) {
		out( std::get<0>(x_) );
	}

	template<typename T, typename A	>	
	inline void out(const std::vector<T,A> & v_) {
		out(prefix);
		unsigned c_ = 0;
		auto v_size = v_.size();
		for (auto e : v_ )
		{
			out( e );
			if ( ++c_ < v_size )
				out(delim);
		}
		out(suffix);
	}

	template<typename T, std::size_t S	>
	inline void out(const std::array<T, S> & arr_) {
		print_range(arr_);
	}

	template <class... Args>
	inline void out(const std::tuple<Args...>& tple) {
		std::apply(
			[](auto&&... xs) {
			    dbj::print_varargs(xs...);
		      },
			tple);
	}

	/* output the { ... } aka std::initializer_list<T> */
	template <class... Args>
	inline void out(const std::initializer_list<Args...> & il_) {
		std::apply(
			[](auto&&... xs) {
			dbj::print_varargs(xs...);
		},
			il_);
	}

} // con
} // win

// back to ::dbj nspace

namespace {

	/*
	seaquences, ranges, varargs
	*/

	constexpr char space = ' ', prefix = '{', suffix = '}', delim = ',';

	/* anything that has size, begin and end */
	auto print_range = [](const auto & range) {

		const size_t argsize = range.size() ;
		if ( argsize < 1 ) return;
		unsigned arg_count = 0;

		auto delimited_out = [&](auto && val_) {
			win::con::out(val_);
			if (++arg_count < (argsize - 1)) win::con::out(delim);
		};

		win::con::out(prefix); win::con::out(space);
		for (auto item : range) {
			delimited_out(item);
		}
		win::con::out(suffix);
	};

	/* also called from void out(...) functions for compound types. e.g. void out(tuple&) */
	template<typename... Args >
	inline	void print_varargs( Args... args)
	{
		if constexpr (sizeof...(Args) < 1) return;
		constexpr size_t argsize = sizeof...(Args);
		unsigned arg_count = 0;

		auto delimited_out = [&](auto && val_) {
			win::con::out(val_);
			if (arg_count++ < (argsize - 1) ) win::con::out(delim);
		};

		win::con::out(prefix); win::con::out(space);
		char dummy[sizeof...(Args)] = { (delimited_out(args), 0)... };
		win::con::out(space); win::con::out(suffix);
		(void)dummy;
	}

	} // nspace

/*
forget templates, variadic generic lambda saves you of declaring them 
*/
	// namespace {
		inline auto print = [](auto && first_param, auto && ... params)
		{
			win::con::out(first_param);

			// if there are  more params
			if constexpr (sizeof...(params) > 0) {
				// recurse
				print(params...);
			}
				return print;
		};
	// }

#pragma endregion "eof printer implementation"

} // dbj

namespace dbj {
	namespace console::config {
		/*
		TODO: usable interface for users to define this
		*/
		namespace {
			using namespace dbj::win;
			inline auto configure_ = [](
				) -> bool {
				auto font_name_ = L"Lucida Console";
				auto code_page_ = con::CODE_PAGE::page_1252;
				try {
					auto new_console [[maybe_unused]] = con::switch_console(code_page_);
					con::setfont(font_name_); 
					DBJ::TRACE(L" Console code page set to ", code_page_, " and font to: ", font_name_);
				}
				catch (...) {
					// can happen before main()
					// and user can have no terminators and abort set up
					// so ...
					dbj::wstring message_ = dbj::win32::getLastErrorMessage(__FUNCSIG__);
					DBJ::TRACE(L"Exception %s", message_.data());
					throw dbj::Exception(message_);
				}
				return true;
			};

			inline const auto single_start = configure_();
		}
	}
}

/*
  Copyright 2017 by dbj@dbj.org

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
