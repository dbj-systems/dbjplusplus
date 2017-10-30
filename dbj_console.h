#pragma once
// #include <windows.h> before this header
namespace dbj {
	namespace win {
		namespace con {
			namespace {
#pragma region "colors and painter"
				/* modification of catch.h console colour mechanism */
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

					static const Painter &  obj(HANDLE initial_handle = ::GetStdHandle(STD_OUTPUT_HANDLE)) {
						static Painter obj_{ initial_handle };
						return obj_;
					}

					Painter(
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

					const bool text_reset() const { return this->text(Colour::None); }

				private:
					bool setTextAttribute(const WORD & _textAttribute) const {
						return ::SetConsoleTextAttribute(stdoutHandle, _textAttribute | originalBackgroundAttributes);
					}
					HANDLE stdoutHandle;
					WORD originalForegroundAttributes;
					WORD originalBackgroundAttributes;
				};
#pragma endregion "colors and painter"
#pragma region "commands"
				/*
				Here we use the dbj::cmd::Commander,  define the comand id's and functions to execute them etc..
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
				};

				using PainterCommandFunction = bool(void);
				using PainterCommander = dbj::cmd::Commander<CMD, PainterCommandFunction >;

				inline PainterCommander  & painter_commander() {
					auto maker = []() {
						PainterCommander commander_;
						commander_
							.insert(CMD::nop, [&]() { return true; })
							.insert(CMD::text_color_reset, [&]() { Painter::obj().text_reset(); return true; })
							.insert(CMD::white, [&]() { Painter::obj().text(Colour::White); return true; })
							.insert(CMD::red, [&]() { Painter::obj().text(Colour::Red); return true; })
							.insert(CMD::green, [&]() { Painter::obj().text(Colour::Green); return true; })
							.insert(CMD::blue, [&]() { Painter::obj().text(Colour::Blue); return true; })
							.insert(CMD::bright_red, [&]() { Painter::obj().text(Colour::BrightRed); return true; });
						return commander_;
					};
					static 	PainterCommander commander_ = maker();
					return commander_;
				}
#pragma endregion "commands"
			} // namespace 
				/* overloaded output functions for various wide types output to "wide" console */
			namespace {

				/* the default out op */
				inline void out(const HANDLE & output_handle_, const std::wstring & wp_) {
					assert(0 != ::WriteConsoleW(output_handle_, wp_.data(),
						static_cast<DWORD>(wp_.size()),
						NULL, NULL));
				}

				inline void out(const HANDLE & output_handle_, const CMD & cmd_) {
					painter_commander().execute(cmd_);
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
				based on idea from:	http://en.cppreference.com/w/cpp/language/parameter_pack
				*/
				const void print(const char * arg) const // base function
				{
					out(output_handle_, arg);
				}

				/*
				'%' is a replacement token
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
		} // con
	} // win
} // dbj
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
