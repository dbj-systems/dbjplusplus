#pragma once

// windows.h required here
#if !defined(_CONSOLE)

#pragma message ( "#############################################################" )
#pragma message ( DBJ_CONCAT( "File: ", __FILE__) )
#pragma message ( DBJ_CONCAT( "Line: ",  DBJ_EXPAND(__LINE__)))
#pragma message ("This is probably not a console app?")
#pragma message ( "#############################################################" )
#endif

#pragma region "Console Interfaces"
#include "dbj_crt.h"
#include "dbj_win32.h"
/*
#include "dbj_console_painter.h"
*/
namespace dbj::win::con {

	/* interface to the wide char console */
	__interface IConsole {
		/* what code page is used */
		const unsigned code_page() const;
		/* out is based on HANDLE and std::wstring */
		HANDLE handle() const;
		// const non-ref argument
		void out(const std::wstring_view wp_) const;

		// fast low level
		// from --> to, must be a sequence
		void out(const wchar_t * from, const wchar_t * to) const;
	};
#pragma endregion "Console Interfaces"

#pragma region "fonts"
	/*
	Apparently "Terminal" is the magical font name that "always works"
	but gives raster fonts
	Otherwise I am yet to find a font name which does not work
	provided it is installed on the system
	so use safe font names

	https://stackoverflow.com/a/33672503/5560811

	*/

	constexpr static const char * const SafeFontNames[]{
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

	typedef enum class CODE : UINT {
		page_1252 = 1252,   // western european windows
		page_65001 = 65001, // utf8
		page_1200 = 1200,  // utf16?
		page_1201 = 1201   // utf16 big endian?
	} CODE_PAGE;

#pragma region "WideOut"
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

		static constexpr CODE DEFAULT_CODEPAGE_ = CODE::page_1201;

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
		}
		// copying
		WideOut(const WideOut & other) = delete;
		/*
		{
			output_handle_ = other.output_handle_;
			previous_code_page_ = other.previous_code_page_;
			code_page_ = other.code_page_;
		}
		*/

		WideOut & operator = (const WideOut & other) = delete;
		/*
		{
			output_handle_ = other.output_handle_;
			previous_code_page_ = other.previous_code_page_;
			code_page_ = other.code_page_;
			return *this;
		}
		*/
		// moving
		WideOut(WideOut && other) = default;
		WideOut & operator = (WideOut && other) = default;


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

		// this is the fast method
		void out(const wchar_t * from) const
		{
			static const HANDLE & output_h_ = this->output_handle_;
			_ASSERTE(from != nullptr);
			std::wstring wide{ from };
			auto retval = ::WriteConsoleW
			(
				output_h_,
				wide.data(),
				static_cast<DWORD>(wide.size()), NULL, NULL
			);
			_ASSERTE(retval != 0);
		}

		// from --> to, must be a sequence
		// this is the *fastest* method
		void out(const wchar_t * from, const wchar_t * to) const
		{
			static const HANDLE & output_h_ = this->output_handle_;

			_ASSERTE(from != nullptr);
			_ASSERTE(to != nullptr);
			_ASSERTE(from != to);

			std::size_t size = std::distance(from, to);

			auto retval = ::WriteConsoleW
			(
				output_h_,
				from,
				static_cast<DWORD>(size), NULL, NULL
			);
			_ASSERTE(retval != 0);
		}

		// wchar_t is native WIN
		// obviously go that way for speed
		// or do the char's
		void out(const char * from, const char * to) const
		{
			_ASSERTE(from != nullptr);
			_ASSERTE(to != nullptr);
			_ASSERTE(from != to);

			std::wstring wide{ from, to };

			out(wide.data(), wide.data() + wide.size());
		}

		void out(const char * from) const
		{
			_ASSERTE(from != nullptr);

			std::string narrow{ from };
			std::wstring wide{ narrow.begin(), narrow.end() };

			out(wide.data(), wide.data() + wide.size());
		}


		/* as dictated by the interface implemented */
		inline void out(const std::wstring_view wp_) const
		{
			const HANDLE & output_h_ = this->output_handle_;
			auto retval = ::WriteConsoleW
			(
				output_h_,
				wp_.data(),
				static_cast<DWORD>(wp_.size()),	NULL, NULL
			);
			_ASSERTE(retval != 0);
		}

		void out(const std::string_view ns_) const
		{
			this->out(std::wstring{ ns_.begin(), ns_.end() });
		}

		void out(const std::u16string_view  u16_) const
		{
			this->out(std::wstring{ u16_.begin(), u16_.end() });
		}

		void out(const std::u32string_view  u32_) const
		{
			this->out(std::wstring{ u32_.begin(), u32_.end() });
		}

	}; // WideOut

				/*
				here we hide the single application wide console instance
				this is single app wide instance
				*/
	inline WideOut & instance()
	{
		static WideOut single_instance
			= [&]() -> WideOut {
			// TODO:
			// this is anonymous lambda called only once
			// by default console used code page 65001
			// we need to make this user configurable
			return { CODE::page_65001 };
		}(); // call immediately
		return single_instance;
	};
	inline WideOut & console_ = instance();
	/* we expose the HANDLE to the print-ing because of future requirements
	wanting to use error handle etc ... */
	inline HANDLE  HANDLE_{ console_.handle() };

	// we are here in dbj::win::con
	// we need to have only a single instance 
	/*
	inline auto switch_console(dbj::win::con::CODE code_) {
		console_ = dbj::win::con::WideOut(code_);
		return console_;
	};
	*/

#pragma endregion 
#pragma region "print-ing implementation"
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
				win::con::out(val_);
				if ((arg_count++) < (argsize - 1)) win::con::out(delim);
			};

			win::con::out(prefix); win::con::out(space);
			for (auto item : range) {
				delimited_out(item);
			}
			win::con::out(space); win::con::out(suffix);
		};

		/* also called from void out(...) functions for compound types. e.g. void out(tuple&) */
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
				win::con::out(val_);
				if (arg_count < pack_size) {
					win::con::out(delim);
				}
				arg_count += 1;
			};

			win::con::out(prefix); win::con::out(space);

			delimited_out(first);

			if constexpr (pack_size > 0) {
				(delimited_out(args), ...);
			}

			win::con::out(space); win::con::out(suffix);
		};

	} // internal nspace
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
		inline void out(N cmd_) {
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

	/*
	above is lovely but that will catch more than we hoped for
	for example bool-eans too
	*/
	/*
				template
					<typename B,
					typename = std::enable_if_t<
								std::is_same_v<B, bool>
								>
					>
					inline void out( B && val_) {
	*/
	inline void out(const bool val_) {
		console_.out((true == val_ ? L"true" : L"false"));
	}

	template< typename T, size_t N >
	inline void out(const std::reference_wrapper< T[N] > wrp)
	{
		_ASSERTE(std::addressof( wrp  ) != nullptr);
		_ASSERTE(std::addressof(wrp[0]) != nullptr);
		using nativarref = T(&)[N];
		internal::print_range((nativarref)wrp.get());
	}

#pragma region strings to string views does not happen automatically
	/* here are the out() overloads for intrinsic types */
	inline void out(const std::wstring & s_) {
		if (! s_.empty())
		console_.out(s_);
	}

	inline void out(const std::string & s_) {
		if (!s_.empty())
			console_.out(s_);
	}

	inline void out(const std::u16string  & s_) {
		if (!s_.empty())
			console_.out(s_);
	}

	inline void out(const std::u32string  & s_) {
		if (!s_.empty())
			console_.out(s_);
	}
#pragma endregion 

	inline void out(const std::string_view sv_) {
		if (sv_.empty()) return;
		console_.out(sv_);
	}

	inline void out(const std::wstring_view sv_) {
		if (sv_.empty()) return;
		console_.out(sv_);
	}

	inline void out(const std::u16string_view sv_) {
		if (sv_.empty()) return;
		console_.out(sv_);
	}

	inline void out(const std::u32string_view sv_) {
		if (sv_.empty()) return;
		console_.out(sv_);
	}
	/*	implement for these when required

	template<typename T, size_t N>
	inline void out(const T(*arp_)[N]) {
		using arf_type = T(&)[N];
		arf_type arf = (arf_type)arp_;
		internal::print_range(arf);
	}


	template<size_t N>
	inline void out(const char(&car_)[N]) {
		console_.out(
			std::string_view(car_, car_ + N)
		);
	}

	template<size_t N>
	inline void out(const wchar_t(&wp_)[N]) {
		console_.out(std::wstring(wp_, wp_ + N));
	}

	inline void out(const char * cp) {
		_ASSERTE(cp != nullptr);
		console_.out(std::string(cp));
	}

	inline void out(const wchar_t * cp) {
		_ASSERTE(cp != nullptr);
		console_.out(std::wstring(cp));
	}

	inline void out(const char16_t * cp) {
		_ASSERTE(cp != nullptr);
		console_.out(std::u16string(cp));
	}

	inline void out(const char32_t * cp) {
		_ASSERTE(cp != nullptr);
		console_.out(std::u32string(cp));
	}

	inline void out(const wchar_t wp_) {
		console_.out(std::wstring(1, wp_));
	}

	inline void out(const char c_) {
		char str[] = { c_ , '\0' };
		console_.out(std::wstring(std::begin(str), std::end(str)));
	}

	inline void out(const char16_t wp_) {
		console_.out(std::u16string{ 1, wp_ });
	}

	inline void out(const char32_t wp_) {
		console_.out(std::u32string{ 1, wp_ });
	}

	/*
	now we will deliver out() overloads for "compound" types using the ones above
	made for intrinsic types
	------------------------------------------------------------------------
	output the exceptions
	*/

	/* print exception and also color the output red */
	/*
				inline void out(const dbj::Exception & x_) {
					paint(painter_command::bright_red);
					// "magic" calls std::wstring casting operator
					// not a good idea?
					console_.out((std::wstring)(x_));
					paint(painter_command::text_color_reset);
				}
   */
				inline void out(const std::exception x_) {
					paint(painter_command::bright_red);
					console_.out( x_.what() );
					paint(painter_command::text_color_reset);
				}

				template<typename T>
				inline void out(const std::variant<T> & x_) {
					out(std::get<0>(x_));
				}

				template<typename T, typename A	>
				inline void out(const std::vector<T, A> & v_) {
					if (v_.empty()) return;
					internal::print_range(v_);
				}

				template<typename K, typename V	>
				inline void out(const std::map<K, V> & map_) {
					if (map_.empty()) return;
					internal::print_range(map_);
				}
				
				template<typename T, std::size_t S	>
				inline void out(const std::array<T, S> & arr_) {
					if (arr_.empty()) return;
					internal::print_range(arr_);
				}

				template <class... Args>
				inline void out(const std::tuple<Args...>& tple) {
					
					if (std::tuple_size< std::tuple<Args...> >::value < 1) return;

					std::apply(
						[](auto&&... xs) {
						internal::print_varargs(xs...);
					},
						tple);
				}

				template <typename T1, typename T2>
				inline void out(const std::pair<T1, T2>& pair_) {
					std::apply(
						[](auto&&... xs) {
						internal::print_varargs(xs...);
					},
						pair_);
				}

				/* output the { ... } aka std::initializer_list<T> */
				template <class... Args>
				inline void out(const std::initializer_list<Args...> & il_) 
				{
					if (il_.size() < 1) return;
					std::apply(
						[](auto&&... xs) {
						internal::print_varargs(xs...);
					},
						il_);
				}
} // dbj::win::con

// back to dbj 
namespace dbj {
/*
forget templates, variadic generic lambda saves you of declaring them 
*/
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


namespace dbj::console::config {

		/*
		TODO: usable interface for users to define this
		*/
		inline const bool & instance()
		{
			using namespace dbj::win;
			static auto configure_once_ = []() -> bool
			{
				auto font_name_ = L"Lucida Console";
				auto code_page_ = con::instance().code_page() ; 
				try {
					// TODO: switch code page on a single running instance
					// auto new_console[[maybe_unused]] = con::switch_console(code_page_);

					con::setfont(font_name_);
					DBJ::TRACE(L"\nConsole code page set to %d and font to: %s\n"
						, code_page_, font_name_
					);
				}
				catch (...) {
					// can happen before main()
					// and user can have no terminators and abort set up
					// so ...
					dbj::wstring message_ = dbj::win32::getLastErrorMessage(__FUNCSIG__);
					DBJ::TRACE(L"\nException %s", message_.data());
					throw dbj::Exception(message_);
				}
				return true;
			}();
			return configure_once_;
		} // instance()
			inline const bool & single_start = instance();

} // dbj::console::config


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
