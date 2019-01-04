#pragma once
#include <exception>
#include <cassert>
#include <string>
#include <string_view>
#include "dbj_crt.h"
/*
DBJ++ error concept
-----------------------------

As Bjarne suggests we will have both return "things" and throw exceptions.
Also we will have catch almost knowhere but inside the main().

We wil addopt the conceptual architecture/design/implementation style 
that means very little prescribed implementation and a lot of conceptual definitions

1.
Exceptions: we do not inherit from std, and have very small hierarchy 
based on dbj::exception
2.
Returns: simply return both error and return values like so:

auto [e, r] = dbj::function() ;

where types of e and r are not predefined, but the meaning is defined:

e -- empty value or error descriptor value 
r -- empty value or return value (just a value)

to know if error is empty or value is empty we will prescribe two families 
of function templates:

template<typename ET> bool is_empty_error( ET error_ ) ;
template<typename VT> bool is_empty_value( VT value_ ) ;

dbj++ (aka ::dbj) is not part of ::std
std::error_code is system/os/platform dependant thus not for dbj++ to inherit from
std::error_condition is independent to the OS etc
Basically we do not iherit, we simply exhibit the same interface
The only inheritance is for the dbj++ exception
To understand a lot about ::std exception and error observe very carefully
the ios::base_failure exception positioning as explained here
https://en.cppreference.com/w/cpp/io/ios_base/failure
As we already said: dbj++ (aka ::dbj) is not part of ::std
Thus we will *not* simply inherit from ::std::exception
Note: ::std::runtime_error and the rest are legacy artefacts. 
      From past times when inheritance
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
	using namespace ::std;
	using namespace ::std::string_view_literals;
	using ::std::string;
	using ::std::wstring;


	/*
	one should not do anything inside std space or inherit
	from any std abstractions, thus
	we deliberately do not inherit from std::exception
	dbj::exception is the same interface and functionality
	as std::exception
	Also. We do not want to be "mixed" with things from the std space.

	Note: we leave it to the compiler to generate the whole copy/move mechanism here

	*/
#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
	class exception
	{
		std::string data_{};
	public:

		exception() noexcept : data_() {	}

		explicit exception(char const* const message_) noexcept
			: data_(message_)
		{
			_STL_VERIFY(nullptr != message_, __FUNCSIG__ " constructor argument can not be a null");
		}

		exception(wchar_t const* const message_, int) noexcept
			: data_( nano::transform_to<string>(wstring(message_)))
		{
			_STL_VERIFY(nullptr != message_, __FUNCSIG__ " constructor argument can not be a null");
		}

		template< typename C >
		exception(::std::basic_string_view<C> msg_)
			: data_(nano::transform_to<string>(msg_))
		{
			_STL_VERIFY(msg_.size() > 0, __FUNCSIG__ " constructor argument can not be a null");
		}

		template< typename C >
		exception(const std::basic_string<C> & msg_)
			: data_(nano::transform_to<string>(msg_))
		{
			_STL_VERIFY(msg_.size() > 0, __FUNCSIG__ " constructor argument can not be a null");
		}

		virtual char const* what() const
		{
			return data_.size() > 1 ? data_.c_str() : "Unknown dbj exception";
		}

		/*
		Caution: keep the retval in the wstring do not take the c_str() 
		of it immediately.
		*/
		virtual wstring wwhat() const
		{
			return data_.size() > 1 ? 
				::dbj::nano::transform_to<wstring>(data_)
				: L"Unknown dbj exception" ;
		}
	};



#pragma warning(pop)
} // dbj
#pragma endregion 

/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"