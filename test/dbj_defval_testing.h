#pragma once

#ifdef	DBJ_TESTING_EXISTS
namespace {
	using namespace Gdiplus;

	// test the const onstance behaviour
	struct S {
		mutable dbj::holder<REAL> width{ 10 };
	};

	const S konst_{}; // must  be initialized if const

	dbj::holder<SmoothingMode> smoothnes{ SmoothingMode::SmoothingModeAntiAlias };
	dbj::holder<LineCap> linecap{ LineCap::LineCapRound };

	DBJ_TEST_UNIT(" : dbj def val option three") {
		auto width_ = konst_.width(1024);
		const auto w1_ = konst_.width();
		auto w2_ = konst_.width();

		_ASSERTE(width_ == 1024);
		_ASSERTE(w2_ == w1_ && w2_ == width_ && w1_ == width_);

		auto lc_ = linecap();
		auto sness = smoothnes();
	}
}
#endif

#ifdef	DBJ_TESTING_EXISTS
namespace {
	using namespace Gdiplus;

	DBJ_TEST_UNIT(" : dbj def val option 2") {
		// create default values holders
		// creation happens since these are first calls
		// nice, but what is stopping us to do this 2 or more times?
		// we will have 2 or more lambdas for each def val type
		auto default_smoot = dbj::holder_maker(SmoothingMode::SmoothingModeAntiAlias);
		auto default_lncap = dbj::holder_maker(LineCap::LineCapRound);
		auto default_width = dbj::holder_maker(10);

		// just get the dflt width
		auto dw0 = default_width();
		auto dw1 = default_width();
		// note: const ref is returned so we can not assign (without stunts) 
		// to value
		// somewhere and sometimes latter someone might change the default value
		auto dw2 = default_width(42);
	}
}
#endif
