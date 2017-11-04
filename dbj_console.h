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
		: implements dbj::win::con::IConsole
	{
		HANDLE output_handle_;
		UINT   previous_code_page_;
		// UINT   code_page_ = 1252;
		const UINT   code_page_ = 65001;
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

#pragma region "print-ing implementation"
/*
  print-ing implementation+interface is whole inside dbj::win::con::anonymous_name_space
  that makes it acessible to clients. E.g. :

  using dbj::win::con::print ;  print("Hi!");

  besides parts which are not because they are in :

  dbj::win::con::anonymous_name_space::anonymous_name_space
*/
namespace {

	/*
	here we hide the single application wide console instance
	it is still acessible to the dbj::win::con::anonymous_name_space
	but not to the users of it
	*/
	namespace {
		WideOut console_{};
		/* we expose the HANDLE to the print-ing because of future requirements
		   wanting to use error handle etc ...
		*/
		HANDLE  HANDLE_{ console_.handle() };
	}

	/*
		dbj::win::con::print(...)  depends on out() overloads for printing various types
		to the console. We add them bellow as required.

		console.out(...) is the only method to output to a console

		this is the special out that does not use the console output class
		but painter commander
	*/
	inline void out(const CMD & cmd_) {
		painter_commander().execute(cmd_);
	}

	/* here are the out() overloads for intrinsic types */
	inline void out(const std::wstring & ws_) {
		console_.out(HANDLE_, ws_);
	}

	/* by using enable_if we make sure this template instances are made
	only for types we want
	*/
	template<typename N, typename = std::enable_if_t<std::is_arithmetic<N>::value > >
	inline void out(const N & number_) {
		// static_assert( std::is_arithmetic<N>::value, "type N is not a number");
		console_.out(HANDLE_, std::to_wstring(number_));
	}

	inline void out(const std::string & s_) {
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
		console_.out(HANDLE_, std::wstring(1, wp_));
	}

	inline void out(const char c_) {
		char str[] = { c_ };
		console_.out(HANDLE_, std::wstring(std::begin(str), std::end(str)));
	}

	/*
	now we have out() overloads for "other" types using the ones above
	made for intrinsic types
	keep in mind that both print() and printex() use this overloads
	------------------------------------------------------------------------
	output the exceptions
	*/
	inline void out(const dbj::Exception & x_) {
		out(x_.what());
	}

	inline void out(const std::exception & x_) {
		out(x_.what());
	}

	template<typename T>
	inline void out(const std::variant<T> & x_) {
		out( std::get<0>(x_) );
	}
} // nspace
} // con
} // win
// back to ::dbj nspace

		namespace {
			using namespace win::con;
		/* 
		------------------------------------------------------------------------
		print(...) implementation. Various sources use the same variadic mechanism, 
		for example : http://en.cppreference.com/w/cpp/language/parameter_pack
		------------------------------------------------------------------------
		
		bellow are the special print() overloads: 
		
		receives a single CMD and passes it to the printer commander
		*/
		inline void print (const CMD & cmd_) {
			painter_commander().execute(cmd_);
		}
		/* print exception and also color the output */
		inline void print(const dbj::Exception & x_) {
			painter_commander().execute(CMD::bright_red);
			out(x_.what());
			painter_commander().execute(CMD::text_color_reset);
		}


		/*	recursion stopper		*/
		inline void print(const char * arg) {
			out(arg);
		}

		/*	'%' is a positioning token	*/
		constexpr char pos_token = '%';

		template<typename T, typename... Targs>
		void print(const char* format, T value, Targs... Fargs) 
			// recursive variadic function
		{
			for (; *format != '\0'; format++) {
				if (*format == pos_token) {
					out(value);
					print(format + 1, Fargs...); // recursive call
					return;
				}
				else {
					// yes ... single char one by one ... 
					out(*format);
				}
			}
		}
		/*
		 for above. NEXT ITERATION print(...): no recursion
		 1. tokenize the format by pos_token into 'words' sequence
		 2.  expand the param pack args into dummy array
		 3.  before each arg take the next 'word' from the step 1.
		*/

		/*	
			No format token '%', because it is tedious for
			when there is a lot of them to remember on the right
			of the format sentence what values to provide and in which order.
			non recursive version
		*/
		template<typename... Targs>
		inline	void printex(Targs... args)
		{
			if constexpr (sizeof...(Targs) > 0) {
				// since initializer lists guarantee sequencing, this can be used to
				// call a function on each element of a pack, in order:
				char dummy[sizeof...(Targs)] = { (out(args), 0)... };
			}
		}
	} // nspace
#pragma endregion "printer implementation"

} // dbj