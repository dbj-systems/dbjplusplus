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

	::dbj::fmt::print("\nchar_buffer\n\t%s",::dbj::buf::char_buffer("hello!"));
}

DBJ_TEST_UNIT(core_utils)
{
	std::error_code ec_;

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

	::dbj::buf::char_buffer cb("hello!");

	DBJ_TEST_ATOM( cb );

	using u8arr = std::uint8_t[];
	u8arr uar{0,1,2};
	std::unique_ptr<u8arr> smart_arr_;
	
	::dbj::buf::assign(smart_arr_, uar);

	::wprintf(L"\n\n%d\t%d\t%d", smart_arr_[0],smart_arr_[1],smart_arr_[2]);

}

DBJ_TEST_SPACE_CLOSE

#undef DBJ_NO_TESTING
