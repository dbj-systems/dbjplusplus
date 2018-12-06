#pragma once
#include <exception>
#include <cassert>
#include <string>
#include <string_view>
/*
DBJ++ error concept
-----------------------------
dbj++ (aka ::dbj) is not part of ::std
std::error_code is system/os/platform dependant thus not for dbj++ to inherit from
std::error_condition is independent to the OS etc
Basically we do not iherit, we simply exhibit the same interface
The only inheritance is for the dbj++ exception
To understand a lot about ::std exception and error observe very carefully
the ios::base_failure exception positioning as explained here
https://en.cppreference.com/w/cpp/io/ios_base/failure
As we already said: dbj++ (aka ::dbj) is not part of ::std
Thus we will simple inherit from ::std::exception
Note: ::std::runtime_error is legacy artefact. From past times when inheritance
and deep inheritance have been considered a fashion statement.
*/
#pragma region error codes and messages
// first a good decades old VERIFY macro
namespace dbj {
	[[noreturn]]
	inline void terror
		(const char * msg_, const char * file_, const int line_)
	{
		assert(msg_);
		assert(file_);
		assert(line_);
		::fprintf(stderr, "\n\ndbj++ ERROR:%s\n%s (%d)", msg_, file_, line_);
		exit(EXIT_FAILURE);
	}

#define DBJ_VERIFY_(x, file, line ) if (false == (x) ) ::dbj::terror( #x ", failed", file, line )

#define DBJ_VERIFY(x) DBJ_VERIFY_(x,__FILE__,__LINE__)
} // dbj
namespace dbj::err {

	using namespace ::std::literals::string_view_literals;

	// equvalent to std::errc
	enum class errc : size_t {
		err_generic, bad_argument, bad_value, bad_type
	};

	// equivalent to std::is_error_code_enum
	template<class T>
	struct is_error_code_enum : ::std::false_type
	{	};
	template<>
	struct is_error_code_enum<::dbj::err::errc> : ::std::true_type
	{	};
	template<class T>
	inline constexpr bool is_error_code_enum_v = is_error_code_enum<T>::value;

	/*
	NOTE: this are compile time messages
	thus they are not printf format strings since applying values to
	format strings at compile time is probably not possible
	*/
	namespace inner {
		constexpr inline ::std::string_view msg_[]
		{
				u8"ERROR: generic"sv,
				u8"ERROR: bad argument"sv,
				u8"ERROR: bad value"sv,
				u8"ERROR: bad type"sv
		};
	} // inner

	constexpr inline auto message(::dbj::err::errc idx) {
		return inner::msg_[size_t(idx)];
	}
}
#pragma endregion
#pragma region dbj exceptions
namespace dbj {

	using namespace::std;

	struct exception : public ::std::exception
	{

	public:
		typedef ::std::exception base_type;

		exception(string_view _Message)
			: base_type(_Message.data())
		{	// construct from message string
		}

		exception(wstring_view _WMessage)
			: base_type(
				std::string(_WMessage.begin(), _WMessage.end()).c_str()
			)
		{	// construct from message unicode std string
		}

		exception(const char *_Message)
			: base_type(_Message)
		{	// construct from message string
		}

		// wide what()
		wstring wwhat() const
		{
			string s_{ this->what() };
			return { begin(s_), end(s_) };
		}
	};
} // dbj
#pragma endregion 

/*
Copyright 2018, 2019 by dbj@dbj.org

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
