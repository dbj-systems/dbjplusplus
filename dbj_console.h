#pragma once
// #include <windows.h> before this header
namespace dbj {
	/* Command pattern mechanism */
	namespace cmd {
#pragma region "commands"
	
		/*  CMD_ENUM defined commands id's
			CMD_FUN is function type to execute them. Whatever satisfies std::is_function<CMD_FUN>
			CMD_COMPARATOR is function to compare the CMD_ENUM elements 
		*/
		template< 
			typename CMD_ENUM, typename CMD_FUN, typename CMD_COMPARATOR = std::less<CMD_ENUM>,
			bool = std::is_function_v<CMD_FUN>
		>
		class Commander final {
		public:
			using comparator_type = CMD_COMPARATOR;
			using executor_type = std::function<CMD_FUN>;
			/* 
			typename bellow is necessary since compiler at the moment of compilation of the template 
			does not know that executor_type::result_type exists, it can not predict that 
			std::function<CMD_FUN> can be compiled in future instantiations and thus can not predict
			that ::result_type will be possible to use
			this 'typename' was not required before C++11
			this is required only when compiling templates even wothout any instantions of them

			DBJ 2017-10-25
			*/
			typedef typename executor_type::result_type executor_return_type ;
			using command_map_type = std::map<	CMD_ENUM, executor_type, comparator_type >;

			/*	commander's function */
			const executor_return_type execute (const CMD_ENUM & command) const
			{
					try {
							return command_map_.at(command)();
					}
					catch (std::out_of_range &) {
						throw  dbj::Exception(" Unknown command?" );
					}
			}

			/* register a function by key given 
			   if function found will throw the exception if replace is false -- this is default
			   if replace == true will replace if found

			*/
			template< typename F>
			const Commander & insert (const CMD_ENUM & command_, F function_ , bool replace = false ) const
			{
				if (replace) {
					command_map_[command_] = executor_type(function_);
				}
				else {
					try {   // ! replace
						auto fun = command_map_.at(command_);
						// found + do not replace
					}
					catch (std::out_of_range &) {
						// new registration
						command_map_[command_] = executor_type(function_);
					}
				}
				return (*this);
			}
				Commander() = default;
		private: 
			mutable command_map_type command_map_{};
		};

#ifdef DBJ_TESTING_EXISTS
namespace /* test the Commander*/ {
	/*
	Define comand id's first
	*/
	enum class CMD : unsigned {
		white = 0,		red,		green,			blue,				cyan,
		yellow,			grey,		bright_red,		text_color_reset,
		nop = (unsigned)-1
	};

	DBJ_TEST_CASE("dbj cmd Commander<>") {
		/* 
		observe carefuly!
		declaration of a function type, NOT function pointer type 
		*/
		using cmd_fun_t = bool (void);
		/* proof of how the template is concieved */
		auto proofing = [&]() {
			auto ok = std::is_function_v<cmd_fun_t>;
			/* declare std::function wrapper type */
			using executor_type = std::function<cmd_fun_t>;
			/* define lambda of a compatible type */
			auto e1 = []()-> bool { return true; };
			/* store its copy in the wrapper declared */
			executor_type fun_wrap = e1;
			using executor_return_type = executor_type::result_type;
			executor_return_type result = fun_wrap();
		};
		proofing();
			Commander<CMD, cmd_fun_t > commander_;
		try {
			bool r = commander_.execute(CMD::red);
		}
		catch (dbj::Exception & x) {
			dbj::io::printex("\n inside ",__func__,", Exception was caught: ", x.what() );
		}
	}
}
#endif
#pragma endregion "commands"
	} // cmd

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
				}	;
				inline cmd::Commander<CMD, bool(void) > & comand_map() {
					auto maker = []() {
						cmd::Commander<CMD, bool(void) > commander_;
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
					static cmd::Commander<CMD, bool(void) > commander_ = maker();
					return commander_;
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
					comand_map().execute(cmd_);
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
