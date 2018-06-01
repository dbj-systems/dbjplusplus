#pragma once

#ifdef	DBJ_TESTING_EXISTS
namespace dbj_testing_space {
	using namespace Gdiplus;

	// test the const onstance behaviour
	struct S final {
		mutable dbj::holder<REAL> width{ 0 };
		S(REAL def_width) : width(def_width) {}
	};

	inline const S konst_{1024}; // initialized to 1024

	inline dbj::holder<SmoothingMode> smoothnes
		{ SmoothingMode::SmoothingModeAntiAlias };

	inline dbj::holder<LineCap> linecap
		{ LineCap::LineCapRound };

	DBJ_TEST_UNIT(" : dbj def val option three") {

		// set the width to 512 for all the calls latter 
		auto	width_		= konst_.width(512);
		const	auto w1_	= konst_.width(); // 512
		auto	w2_			= konst_.width(); // 512

		_ASSERTE(width_ == 512);
		_ASSERTE(w2_ == w1_ && w2_ == width_ && w1_ == width_);

		//use the previosuly declared 
		// and instantiated def val holders
		auto lc_ = linecap();
		auto sness = smoothnes();
	}

	DBJ_TEST_UNIT(" : dbj def val option two") {
		// create default values holders
		// creation happens on first calls
		// nice, but what is stopping us to do this 2 or more times?
		// we will have 2 or more lambdas for each def val type
		auto default_smoot = dbj::v_two::holder_maker(SmoothingMode::SmoothingModeAntiAlias);
		auto default_lncap = dbj::v_two::holder_maker(LineCap::LineCapRound);
		auto default_width = dbj::v_two::holder_maker(10);

		// just get the dflt width
		auto dw0 = default_width();
		auto dw1 = default_width();
		// note: const ref is returned so we can 
		// not assign (without stunts)  to ret value
		// somewhere and sometimes latter someone 
		// might change the default value
		auto dw2 = default_width(42);
	}
}
#endif
