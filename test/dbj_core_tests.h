#pragma once
#include "../core/dbj++core.h"

#ifdef DBJ_CONSOLE_SYTEM
void out(
	::dbj::buf::char_buffer const & cb_
)
{
	using namespace ::dbj::console;
	prinf(L"%S", cb_.data());
}
#endif

#define DBJ_NO_TESTING

DBJ_TEST_SPACE_OPEN(core_tests)

// Test BUFfer
#define DBJ_T_BUF( F,x ) ::dbj::fmt::print("\n\nExpression: %s\n\tType: %s\n\tValue:" F,\
	(#x), typeid(x).name(), (x) )

namespace u = ::dbj::core::util;
using ::dbj::fmt::print;

DBJ_TEST_UNIT(core_format_utils)
{
	std::error_code ec_;

	print(
		"\nTIME_STAMP_SIMPLE_MASK\n\t%s",	
		u::make_time_stamp(ec_, u::TIME_STAMP_SIMPLE_MASK)
	);
	print( 
		"\nTIME_STAMP_FULL_MASK\n\t%s",	
		u::make_time_stamp(ec_, u::TIME_STAMP_FULL_MASK)
	);

	print("\nENV VAR 'USERNAME'\n\t%s", 
		u::dbj_get_envvar("USERNAME", ec_));

	// programdata path
	print("\nprogramdata path\n\t%s", 
		u::program_data_path(ec_));

	print("\nchar_buffer\n\t%s",::dbj::buf::buffer("hello!"));
}

DBJ_TEST_UNIT(core_utils)
{
	std::error_code ec_;
	using namespace ::dbj::buf;
	using namespace ::std::string_view_literals;

	{
		// mid value
		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0811r2.html
		DBJ_T_BUF( "%3.3f", u::midpoint(3.3f, 7.7f ) );
		DBJ_T_BUF( "%3.3f", u::midpoint(3.3, 7.7 ) );
		DBJ_T_BUF( "%u", u::midpoint(3U, 7U ) );
		DBJ_T_BUF( "%ul", u::midpoint(3l, 7l ) );
		// and so on ... numbers only
	}

	// uses u::TIME_STAMP_SIMPLE_MASK by default
	DBJ_T_BUF("%s", u::make_time_stamp(ec_, u::TIME_STAMP_SIMPLE_MASK));
	DBJ_T_BUF("%s", ec_);
	DBJ_T_BUF("%s", u::make_time_stamp(ec_, u::TIME_STAMP_FULL_MASK));
	DBJ_T_BUF("%s", ec_);

	// env vars
	DBJ_T_BUF("%s", u::dbj_get_envvar("USERNAME", ec_ ));
	DBJ_T_BUF( "%s", ec_ );

	// programdata path
	DBJ_T_BUF("%s", u::program_data_path(ec_));
	DBJ_T_BUF("%s", ec_);

	::dbj::buf::buffer cb("hello!");

	DBJ_T_BUF( "%s", cb );
	{
		using u8arr = std::uint8_t[];
		// u8arr uar{ 0,1,2 };
		auto smart_arr_ = std::make_unique<u8arr>(3);

		smart_arr_[0] = 0; smart_arr_[1] = 1; smart_arr_[2] = 2 ;
		print("\n\n%s\n\t%d, %d, %d"sv, typeid(smart_arr_).name(), smart_arr_[0], smart_arr_[1], smart_arr_[2]);

		 auto jupi = buf_type::make("jupi?");
		// auto jupi = ::dbj::buf::yanb("jupi?");
		 print("\n\n%s\n"sv, jupi );
	}
}

DBJ_TEST_SPACE_CLOSE

#undef DBJ_NO_TESTING
