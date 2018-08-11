#pragma once

#include "dbj_kalends.h"
DBJ_TEST_SPACE_OPEN( dbj_kalends )

DBJ_TEST_UNIT(dbj_timers_) {

	using namespace dbj::kalends;

	auto test = [&](timer_kind which_) -> time_ticks_type {
		using namespace dbj::kalends;
		auto timer_ = create_timer(which_);
		time_ticks_type stp = DBJ_TEST_ATOM(timer_.start());
		dbj_sleep_seconds(1);
		time_ticks_type esd = DBJ_TEST_ATOM(timer_.elapsed());
		return esd;
	};

	MilliSeconds elaps_1 = to_desired_unit<MilliSeconds>(test(timer_kind::win32));
	MilliSeconds elaps_2 = to_desired_unit<MilliSeconds>(test(timer_kind::modern));

	// this means the difference can be max 10
	time_ticks_type tolerance{ 10 };

	bool is_inside_tolerance = ((tolerance + 1) >
		static_cast<time_ticks_type>(std::llabs(elaps_1.count() - elaps_2.count()))
		);

	dbj::console::print(
		"\nWIN32  timer has measured ", elaps_1.count(), " milliseconds",
		"\nModern timer has measured ", elaps_2.count(), " milliseconds",
		"\nThe difference is ", (is_inside_tolerance ? "" : "not"), " inside tolerance of ", tolerance, " milliseconds"
	);
}

DBJ_TEST_SPACE_CLOSE