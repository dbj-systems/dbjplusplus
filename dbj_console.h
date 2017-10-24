#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define STRICT
#include <windows.h>
namespace dbj {
	namespace win {
		namespace console {


			/* modification of catch2 console colour mechanism */
					enum class Colour : unsigned {
						None = 0,
						White,
						Red,
						Green,
						Blue,
						Cyan,
						Yellow,
						Grey,

						Bright = 0x10,

						BrightRed = Bright | Red,
						BrightGreen = Bright | Green,
						LightGrey = Bright | Grey,
						BrightWhite = Bright | White,

						// By intention
						FileName = LightGrey,
						Warning = Yellow,
						ResultError = BrightRed,
						ResultSuccess = BrightGreen,
						ResultExpectedFailure = Warning,

						Error = BrightRed,
						Success = Green,

						OriginalExpression = Cyan,
						ReconstructedExpression = Yellow,

						SecondaryText = LightGrey,
						Headers = White
					};

				std::ostream& operator << (std::ostream& os, Colour const&); // no op


				class __declspec(novtable) Painter final {
				public:

					static const Painter &  obj( HANDLE initial_handle = ::GetStdHandle(STD_OUTPUT_HANDLE) ) {
						static Painter obj_{ initial_handle };
						return obj_;
					}

					Painter (
						HANDLE another_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE)
					) : stdoutHandle(another_handle_)
					{
						CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
						::GetConsoleScreenBufferInfo(stdoutHandle, &csbiInfo);
						originalForegroundAttributes = csbiInfo.wAttributes & ~(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
						originalBackgroundAttributes = csbiInfo.wAttributes & ~(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					}

				const bool text(const Colour & _colourCode) const {
					switch (_colourCode) {
						case Colour::None:      return setTextAttribute(originalForegroundAttributes);
						case Colour::White:     return setTextAttribute(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
						case Colour::Red:       return setTextAttribute(FOREGROUND_RED);
						case Colour::Green:     return setTextAttribute(FOREGROUND_GREEN);
						case Colour::Blue:      return setTextAttribute(FOREGROUND_BLUE);
						case Colour::Cyan:      return setTextAttribute(FOREGROUND_BLUE | FOREGROUND_GREEN);
						case Colour::Yellow:    return setTextAttribute(FOREGROUND_RED | FOREGROUND_GREEN);
						case Colour::Grey:      return setTextAttribute(0);

						case Colour::LightGrey:     return setTextAttribute(FOREGROUND_INTENSITY);
						case Colour::BrightRed:     return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_RED);
						case Colour::BrightGreen:   return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
						case Colour::BrightWhite:   return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);

						default: throw "Exception in "  __FUNCDNAME__ " : not a valid colour code sent";
						}
				}

				const bool text_reset() const { 	return this->text(Colour::None);	}

				private:
					bool setTextAttribute(const WORD & _textAttribute) const {
						return ::SetConsoleTextAttribute(stdoutHandle, _textAttribute | originalBackgroundAttributes);
					}
					HANDLE stdoutHandle;
					WORD originalForegroundAttributes;
					WORD originalBackgroundAttributes;
				};

#pragma region "commands"
				/*
				Here we define comand id's
				*/
				enum class CMD : unsigned {
					white = 0,
					red,
					green,
					blue,
					cyan,
					yellow,
					grey,
					bright_red,
					text_color_reset,
					nop = (unsigned)-1
				}
				;

				using cmd_fun_t = std::function< bool(void)>;

				struct CMDcomparator {
					bool operator () (const CMD & lhs, const CMD & rhs) const
					{
						return lhs < rhs;
					};
				};

				using command_map_t = std::map<CMD, cmd_fun_t, CMDcomparator >;


				/*
				here we map command id's to lambdas that execute them
				notice we define lambds to capture its run time surroundings by referece
				*/
				inline command_map_t & comand_map() {
					static command_map_t map_ = {
						{ CMD::nop,				 [&]() { return true; } },
						{ CMD::text_color_reset, [&]() { Painter::obj().text_reset(); return true; } },
						{ CMD::white,  [&]() { Painter::obj().text(Colour::White); return true; } },
						{ CMD::red,  [&]() { Painter::obj().text(Colour::Red); return true; } },
						{ CMD::green,  [&]() { Painter::obj().text(Colour::Green); return true; } },
						{ CMD::blue,  [&]() { Painter::obj().text(Colour::Blue); return true; } },
						{ CMD::bright_red,  [&]() { Painter::obj().text(Colour::BrightRed); return true; } }
					};
					return map_;
				}

				/*	commander's function */
				inline const void commander(CMD command)
				{
					auto executor = [&]() {
						try {
							(comand_map().at(command))();
						}
						catch (std::out_of_range &) {
							throw " Unknown command exception";
						}
					};
					return executor();
				}
#pragma endregion "commands"
				
				/* overloaded output functions for various wide types output to "wide" console */
			namespace {

				/* the default out op */
				inline void out(const HANDLE & output_handle_, const std::wstring & wp_) {
					assert(0 != ::WriteConsoleW(output_handle_, wp_.data(),
						static_cast<DWORD>(wp_.size()),
						NULL, NULL));
				}

				inline void out(const HANDLE & output_handle_, const CMD & cmd_) {
					commander(cmd_);
				}

				/* by using enable_if we make sure this template instances are made
				only for types we want
				*/
				template<typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value > >
				inline void out(const HANDLE & output_handle_, const N & number_) {
					// static_assert( std::is_arithmetic<N>::value, "type N is not a number");
					out(output_handle_, std::to_wstring(number_));
				}


				inline void out(const HANDLE & output_handle_, const char * cp) {
					std::string s(cp);
					out(output_handle_, std::wstring(s.begin(), s.end()));
				}

				inline void out(const HANDLE & output_handle_, const wchar_t * cp) {
					out(output_handle_, std::wstring(cp));
				}

				template<size_t N>
				inline void out(const HANDLE & output_handle_, const wchar_t(&wp_)[N]) {
					out(output_handle_, std::wstring(wp_));
				}

				inline void out(const HANDLE & output_handle_, const wchar_t wp_) {
					wchar_t str[] = { wp_, L'\0' };
					out(output_handle_, str);
				}

				inline void out(const HANDLE & output_handle_, const char c_) {
					char str[] = { c_ };
					out(output_handle_, std::wstring(std::begin(str), std::end(str)));
				}

				inline void single_char(const HANDLE & output_handle_, const char c_) {
					char str[] = { c_ };
					out(output_handle_, std::wstring(std::begin(str), std::end(str)));
				}

			} // /* overloaded output functions for various types */

			  /*
			  Not FILE * but HANDLE based output.
			  It also uses Windows.1252 Code Page.
			  This two are perhaps why this almost always works.

			  https://msdn.microsoft.com/en-us/library/windows/desktop/dd374122(v=vs.85).aspx

			  Even if you get your program to write UTF16 correctly to the console,
			  Note that the Windows console isn't UTF16 friendly and may just show garbage.
			  */
			struct __declspec(novtable)	WideOut final
			{
				HANDLE output_handle_;
				UINT   previous_code_page_;
			public:
				WideOut()
				{
					this->output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
					assert(INVALID_HANDLE_VALUE != this->output_handle_);
					previous_code_page_ = ::GetConsoleOutputCP();
					assert(0 != ::SetConsoleOutputCP(1252));
					/*			TODO: GetLastError()			*/
				}

				~WideOut()
				{
					assert(0 != ::SetConsoleOutputCP(previous_code_page_));
					// TODO: should we "relase" this->output_handle_ ?
					/*			TODO: GetLastError()  		*/
				}


				/*
				based on idea from
				http://en.cppreference.com/w/cpp/language/parameter_pack
				*/
				const void print(const char * arg) const // base function
				{
					out(output_handle_, arg);
				}

				/*
				Primitive print(). Tries to handle "words" and "numbers".
				'%' is a replacement token
				No type designators
				No field width or precision values
				*/
				template<typename T, typename... Targs>
				void print(const char* format, T value, Targs... Fargs) // recursive variadic function
				{
					for (; *format != '\0'; format++) {
						if (*format == '%') {
								out(output_handle_, value);
							print(format + 1, Fargs...); // recursive call
							return;
						}
						else {
							single_char(output_handle_, *format);
						}
					}
				}

			}; // WideOut

		} // console

	} // win
} // dbj
#pragma region "console testing"
#ifdef DBJ_TESTING_EXISTS
namespace {
	using namespace dbj::win::console;
	using CMD = dbj::win::console::CMD;

	DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {
	
		WideOut wout{};
		const static std::wstring doubles = L"║═╚";
		const static std::wstring singles = L"│─└";
/*
here we use them commands through the print()
*/
		wout.print(
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"
			"%\n%\t%\t%"	,
			CMD::white,				"White",		doubles, singles,
			CMD::red,				"Red",			doubles, singles,
			CMD::green,				"Green",		doubles, singles,
			CMD::blue,				"Blue",			doubles, singles,
			CMD::bright_red,		"Bright Red",	doubles, singles,
			CMD::text_color_reset,  "Reset",		doubles, singles
			
		);
	}
}
#endif // DBJ_TESTING_EXISTS
#pragma endregion "console testing"

  /* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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
/*
The only requirement for this file is not to have any #include's
*/
