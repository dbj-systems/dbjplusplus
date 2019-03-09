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

namespace u = ::dbj::core::util;

DBJ_TEST_UNIT(core_format_utils)
{
	std::error_code ec_;

	::dbj::fmt::print(
		"\nTIME_STAMP_SIMPLE_MASK\n\t%s",	u::make_time_stamp(ec_, u::TIME_STAMP_SIMPLE_MASK)
	);
	::dbj::fmt::print( 
		"\nTIME_STAMP_FULL_MASK\n\t%s",	u::make_time_stamp(ec_, u::TIME_STAMP_FULL_MASK)
	);

	::dbj::fmt::print("\nENV VAR 'USERNAME'\n\t%s", u::dbj_get_envvar("USERNAME", ec_));

	// programdata path
	::dbj::fmt::print("\nprogramdata path\n\t%s", u::program_data_path(ec_));

	::dbj::fmt::print("\nchar_buffer\n\t%s",::dbj::buf::buffer("hello!"));
}

DBJ_TEST_UNIT(core_utils)
{
	std::error_code ec_;
	using namespace ::dbj::buf;
	using namespace ::dbj::fmt;
	using namespace ::std::string_view_literals;

	{
		// mid value
		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0811r2.html
		DBJ_TEST_ATOM( ::dbj::core::util::midpoint(3.3f, 7.7f ) );
		DBJ_TEST_ATOM( ::dbj::core::util::midpoint(3.3, 7.7 ) );
		DBJ_TEST_ATOM( ::dbj::core::util::midpoint(3U, 7U ) );
		DBJ_TEST_ATOM( ::dbj::core::util::midpoint(3l, 7l ) );
		// and so on ... numbers only
	}

	// uses u::TIME_STAMP_SIMPLE_MASK by default
	DBJ_ATOM_TEST(u::make_time_stamp(ec_, u::TIME_STAMP_SIMPLE_MASK));
	DBJ_ATOM_TEST(ec_);
	DBJ_ATOM_TEST(u::make_time_stamp(ec_, u::TIME_STAMP_FULL_MASK));
	DBJ_ATOM_TEST(ec_);

	// env vars
	DBJ_ATOM_TEST(u::dbj_get_envvar("USERNAME", ec_ ));
	DBJ_ATOM_TEST( ec_ );

	// programdata path
	DBJ_ATOM_TEST(u::program_data_path(ec_));
	DBJ_ATOM_TEST(ec_);

	::dbj::buf::buffer cb("hello!");

	DBJ_TEST_ATOM( cb );
	{
		using u8arr = std::uint8_t[];
		u8arr uar{ 0,1,2 };
		std::shared_ptr<u8arr> smart_arr_;

		smart_arr_.reset(uar);

		print("\n\n%d\t%d\t%d"sv, smart_arr_[0], smart_arr_[1], smart_arr_[2]);

		print("\n\n%S\n"sv, buf_type::make("jupi?"));
	}
}

DBJ_TEST_SPACE_CLOSE

#undef DBJ_NO_TESTING
