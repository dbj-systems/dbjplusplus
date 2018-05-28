#pragma once

#ifdef DBJ_TESTING_EXISTS

DBJ_TEST_UNIT(" dbj application environment test ") {

	using dbj::print;

	auto application_rt_environment
		= DBJ_TEST_ATOM(dbj::app_env::structure::instance());

	DBJ_TEST_ATOM(application_rt_environment.cli_data);
	print("\nEnv vars found");
	int counter = 0;
	std::wstring key, val;
	for (auto && kv : application_rt_environment.env_vars)
	{
		key = kv.first.data();
		val = kv.second.data();
		print("\n[", counter++, "] Key: ", key, "\t = \tValue: ", val);
	}
}

#endif