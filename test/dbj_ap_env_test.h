#pragma once

DBJ_TEST_SPACE_OPEN(app_env_)


DBJ_TEST_UNIT(_application_environment) {

	using dbj::console::print;

	auto application_rt_environment
		= DBJ_TEST_ATOM(dbj::app_env::structure::instance());

	auto DBJ_MAYBE(rez) = DBJ_TEST_ATOM(application_rt_environment.cli_data);
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

DBJ_TEST_SPACE_CLOSE

