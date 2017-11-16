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
				void out(const HANDLE & output_handle_, const std::wstring & wp_) const ;
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


	enum class CODE : UINT { page_1252 = 1252, page_65001 = 65001 };

#pragma region "WideOut"
namespace {
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
	template<UINT CODEPAGE>
	struct __declspec(novtable)	WideOut final
		: implements dbj::win::con::IConsole
	{
		mutable		HANDLE output_handle_;
		mutable		UINT   previous_code_page_;
		const		UINT	code_page_{ CODEPAGE };
	public:
		WideOut()
			: output_handle_ (::GetStdHandle(STD_OUTPUT_HANDLE))
			, previous_code_page_ (::GetConsoleOutputCP())
		{
			//this->output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
			assert(INVALID_HANDLE_VALUE != this->output_handle_);
			// previous_code_page_ = ::GetConsoleOutputCP();
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

		/* the default one and only out-put op-erator */
		inline void out(const HANDLE & output_handle_, const std::wstring & wp_) const {
			assert(0 != ::WriteConsoleW(output_handle_, wp_.data(),
				static_cast<DWORD>(wp_.size()),	NULL, NULL));
		}

	}; // WideOut

	/*
	here we hide the single application wide console instance
	this is single app wide instance
	*/
	namespace {
#ifndef _CONSOLE
#pragma message (__FILE__ "["  DBJ_STRINGIFY(__LINE__) "]: WARNING: This is not a console app?")  
#endif
		WideOut<(UINT)CODE::page_1252> console_ ;
		/* we expose the HANDLE to the print-ing because of future requirements
		wanting to use error handle etc ...
		*/
		HANDLE  HANDLE_{ console_.handle() };
	}

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
	// inline void out(const dbj::Exception & x_) { out(x_.what()); }

	inline void out(const std::exception & x_) {
		out(x_.what());
	}

	template<typename T>
	inline void out(const std::variant<T> & x_) {
		out( std::get<0>(x_) );
	}

	/* print exception and also color the output red */
	inline void out(const dbj::Exception & x_) {
		painter_commander().execute(CMD::bright_red);
		// "magic" calls std::wstring casting operator
		// perhaps not a good idea
		console_.out(HANDLE_, (x_));
		painter_commander().execute(CMD::text_color_reset);
	}

	template <class... Args>
	inline void out(const std::tuple<Args...>& tple) {
		std::apply(
			[](auto&&... xs) {
			    dbj::print_sequence(xs...);
		      },
			tple);
	}

	/* output the { ... } aka std::initializer_list<T> */
	template <class... Args>
	inline void out(const std::initializer_list<Args...> & il_) {
		std::apply(
			[](auto&&... xs) {
			dbj::print_sequence(xs...);
		},
			il_);
	}

} // nspace
// another (parallel) namespace removes the danger of accidental endles recursion 
namespace {
/*
	TYPEDEFS FOR basic_string_view
	using string_view = basic_string_view<char>;
*/
	inline void out(const std::string_view & sv_) {
		dbj::win::con::out(
			std::wstring( sv_.begin() , sv_.end() )
		);
	}
/*	implement for these when required 
	using u16string_view = basic_string_view<char16_t>;
	using u32string_view = basic_string_view<char32_t>;
	using wstring_view = basic_string_view<wchar_t>;
*/
}
} // con
} // win

// back to ::dbj nspace

namespace {

	/* also called from void out(...) functions for compound types. e.g. void out(tuple&) */
	template<char prefix = '{', char suffix = '}', char delim = ',', typename... Args >
	inline	void print_sequence( Args... args)
	{
		if constexpr (sizeof...(Args) < 1) return;
		constexpr size_t argsize = sizeof...(Args);
		unsigned arg_count = 0;

		auto delimited_out = [&](auto && val_) {
			win::con::out(val_);
			if (++arg_count < argsize ) win::con::out(delim);
		};

		win::con::out(prefix);
		char dummy[sizeof...(Args)] = { (delimited_out(args), 0)... };
		win::con::out(suffix);
	}

	} // nspace

#if 0

	/*	generic print() API */
	template<typename T, typename... Args>
	inline	void print( const T & first, Args&&... args)
	{
		win::con::out(first);
		// No recursion
		if constexpr (sizeof...(Args) > 0) {
			// since initializer lists guarantee sequencing, this can be used to
			// call a function on each element of a pack, in order:
			char dummy[sizeof...(Args)] = { (win::con::out(args), 0)... };
		}
	}

	template<typename T, typename ...Args>
	inline	void print(std::initializer_list<T> il_, Args&&... args)
	{
		win::con::out(il_);
		if constexpr (sizeof...(Args) > 0) print(args...);
	}
	template<typename T, typename ...Args>
	inline	void print( const T & val_,  std::initializer_list<Args...> il_)
	{
		win::con::out(val_);
		if constexpr (sizeof...(Args) > 0) print(il_...);
	}
#endif
/*
forget templates, variadic generic lambda saves you of declaring them 
*/
	namespace {
		auto print = [](auto... param)
		{
			if constexpr (sizeof...(param) > 0) {
				char dummy[sizeof...(param)] = { 
					(win::con::out( std::forward<decltype(param)>(param) ), 0)... 
				};
			}
			return print;
		};
	}

#pragma endregion "eof printer implementation"

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

#if 0
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
#endif