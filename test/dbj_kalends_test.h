#pragma once

#include "../dbj++.h"

namespace quick_dirty
{
	// to check the validity of dbj timers
	struct Timer final
	{
		typedef std::chrono::high_resolution_clock clock_;
		typedef std::chrono::seconds		seconds_;
		typedef std::chrono::milliseconds	miliseconds_;
		typedef std::chrono::microseconds	microseconds_;
		std::chrono::time_point<clock_> beg_;

		Timer() : beg_(clock_::now()) {}

		void start() { beg_ = clock_::now(); }

		template<typename U = Timer::miliseconds_ >
		auto elapsed() const {
			return std::chrono::duration_cast<U>
				(clock_::now() - beg_).count();
		}
	};

	template<typename F,
		typename U = Timer::miliseconds_
	>
		auto measure(F fun_to_test)
	{
		using namespace dbj::kalends;
		Timer timer_;
		fun_to_test();
		return timer_.elapsed<U>();
	};
}


// #include "dbj_kalends.h"
DBJ_TEST_SPACE_OPEN( dbj_kalends_testing )

DBJ_TEST_UNIT(dbj_timers_) {

	using namespace dbj::kalends;

	auto worker = [& ]() { dbj_sleep_seconds(1); };
	using worker_type = decltype(worker);

	auto t1 = measure< worker_type, dbj::kalends::MilliSeconds, timer_kind::win32>
		(worker);

	auto t2 = measure< worker_type, dbj::kalends::MilliSeconds, timer_kind::modern>
		(worker);

	auto t3 = quick_dirty::measure([&] { dbj_sleep_seconds(1); });

	dbj::console::print(
        "\n\nWIN32, Modern, Quick and dirty timer: ",
		t1,", ",t2,", ", t3, " miliseconds" );
}

DBJ_TEST_SPACE_CLOSE