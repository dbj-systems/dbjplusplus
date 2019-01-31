#pragma once
#include "../core/dbj++core.h"

DBJ_TEST_SPACE_OPEN(core_tests)


DBJ_TEST_UNIT(core_utils) 
{
	// time stamp
	namespace u = ::dbj::core::util;

	std::error_code ec_;

	// uses u::TIME_STAMP_SIMPLE_MASK by default
	DBJ_ATOM_TEST( u::make_time_stamp(ec_, u::TIME_STAMP_SIMPLE_MASK));
	DBJ_ATOM_TEST( u::make_time_stamp(ec_, u::TIME_STAMP_FULL_MASK));
	
	// env vars
	DBJ_ATOM_TEST(u::dbj_get_envvar("USERNAME"));

	// programdata path
	DBJ_ATOM_TEST(u::program_data_path());


}

DBJ_TEST_SPACE_CLOSE