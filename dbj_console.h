#pragma once
// #include <windows.h> before this header
#pragma region "Console Interfaces"
namespace dbj {
	namespace win {
		namespace con {
			/* interface to the wide char console */
			__interface IConsole {
				/* what code page is used */
				const unsigned code_page() const;
				/* out is based on HANDLE and std::wstring */
				HANDLE handle() const;
				void out(const HANDLE & output_handle_, const std::wstring & wp_);
			};
#if 0
			/* printer uses the console as part of its implementation */
			struct IPrinter {
				/*	'%' is a replacement token	*/
				const char token() const;
				const void print(const char * arg) const; // base function recursion stopper
				template<typename T, typename... Targs>
				void print(const char* format, T value, Targs... args); // recursive variadic function			
			};
#endif

		}
	}
}
#pragma endregion "Console Interfaces"

namespace dbj {
namespace win {
namespace con {
#pragma region "WideOut"
namespace /* WideOut*/ {
	/*
	Not FILE * but HANDLE based output.
	It also uses Windows.1252 Code Page.
	This two are perhaps why this almost always works.

	https://msdn.microsoft.com/en-us/library/windows/desktop/dd374122(v=vs.85).aspx

	Even if you get your program to write UTF16 correctly to the console,
	Note that the Windows console isn't UTF16 friendly and may just show garbage.
	*/
	struct __declspec(novtable)	WideOut final
		: implements dbj::win::con::IConsole
	{
		HANDLE output_handle_;
		UINT   previous_code_page_;
		UINT   code_page_ = 1252;
	public:
		WideOut()
		{
			this->output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
			assert(INVALID_HANDLE_VALUE != this->output_handle_);
			previous_code_page_ = ::GetConsoleOutputCP();
			assert(0 != ::SetConsoleOutputCP(code_page_));
			/*			TODO: GetLastError()			*/
		}

		~WideOut()
		{
			assert(0 != ::SetConsoleOutputCP(previous_code_page_));
			// TODO: should we "relase" this->output_handle_ ?
			/*			TODO: GetLastError()  		*/
		}

		/* what code page is used */
		const unsigned code_page() const { return this->code_page_; }
		/* out is based on HANDLE and std::wstring */
		HANDLE handle() const { return this->output_handle_; }

		/* the default out op */
		inline void out(const HANDLE & output_handle_, const std::wstring & wp_) {
			assert(0 != ::WriteConsoleW(output_handle_, wp_.data(),
				static_cast<DWORD>(wp_.size()),	NULL, NULL));
		}

	}; // WideOut
} //nspace
#pragma endregion "WideOut"

#pragma region "printer implementation"
namespace {
				/* here we hide the single application wide console instance */
				namespace {
					WideOut console_{};
					HANDLE  HANDLE_{ console_.handle() };

					/* this is the special out that does not use the console output class 
					   but painter commander 
					 */
					inline void out(const CMD & cmd_) {
						painter_commander().execute(cmd_);
					}

					/* by using enable_if we make sure this template instances are made
					only for types we want
					*/
					template<typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value > >
					inline void out(const N & number_) {
						// static_assert( std::is_arithmetic<N>::value, "type N is not a number");
						console_.out(HANDLE_, std::to_wstring(number_));
					}

					inline void out( const std::string & s_) {
						console_.out(HANDLE_, std::wstring(s_.begin(), s_.end()));
					}


					inline void out(const char * cp) {
						std::string s(cp);
						console_.out(HANDLE_, std::wstring(s.begin(), s.end()));
					}

					inline void out(const wchar_t * cp) {
						console_.out(HANDLE_, std::wstring(cp));
					}

					template<size_t N>
					inline void out(const wchar_t(&wp_)[N]) {
						console_.out(HANDLE_, std::wstring(wp_));
					}

					inline void out(const wchar_t wp_) {
						wchar_t str[] = { wp_, L'\0' };
						console_.out(HANDLE_, str);
					}

					inline void out(const char c_) {
						char str[] = { c_ };
						console_.out(HANDLE_, std::wstring(std::begin(str), std::end(str)));
					}
				}

				/* this is the special print that receives a single CMD and passes it to the 
				   printer commander
				*/
				inline void print (const CMD & cmd_) {
					painter_commander().execute(cmd_);
				}

				/*
				based on idea from:	http://en.cppreference.com/w/cpp/language/parameter_pack
				*/
				inline void print(const char * arg) // base function
				{
					out(arg);
				}

				/*	'%' is a positioning token	*/
				constexpr char pos_token = '%';

				template<typename T, typename... Targs>
				void print(const char* format, T value, Targs... Fargs) // recursive variadic function
				{
					for (; *format != '\0'; format++) {
						if (*format == pos_token) {
							out(value);
							print(format + 1, Fargs...); // recursive call
							return;
						}
						else {
							// single char one by one ... ok optimization latter
							out(*format);
						}
					}
				}
			}
#pragma endregion "printer implementation"
} // con
} // win
} // dbj