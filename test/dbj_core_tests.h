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

DBJ_TEST_SPACE_OPEN(core_tests)

DBJ_TEST_UNIT(core_utils)
{
	// time stamp
	namespace u = ::dbj::core::util;

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

}

DBJ_TEST_SPACE_CLOSE