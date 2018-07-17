#pragma once

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

inline void test_dbj_defval_option_three() {
// set the width to 512 for all the calls latter 
auto	width_ = konst_.width(512);
const	auto w1_ = konst_.width(); // 512
auto	w2_ = konst_.width(); // 512

_ASSERTE(width_ == 512);
_ASSERTE(w2_ == w1_ && w2_ == width_ && w1_ == width_);

//use the previosuly declared 
// and instantiated def val holders
auto lc_ = linecap();
auto sness = smoothnes();
}

	DBJ_TEST_UNIT(" : dbj def val option three") {
		// reminder: since C++11 local static vars
		// are thread safe
		static bool retval = [] { test_dbj_defval_option_three(); return true; }();
	}

	/****************************************************************/
	/*OPTION 2.1*/
	/****************************************************************/

using dbj::v_two::holder_maker;

// inside named namespace
// create default values holders
// creation happens on first call
inline auto default_smoot = 
	dbj::v_two::holder_maker(SmoothingMode::SmoothingModeAntiAlias);

inline auto default_lncap = 
	dbj::v_two::holder_maker(LineCap::LineCapRound);

inline auto default_width = 
	dbj::v_two::holder_maker(10);

inline void test_dbj_defval_option_two() {
	// just get the dflt width
	auto df0 = default_smoot();
	auto df1 = default_lncap();

	// somewhere and sometimes latter someone 
	// might change the default value
	auto df2 = default_width(42);
}

	DBJ_TEST_UNIT(" : dbj def val option two") {
		static bool retval = [] { test_dbj_defval_option_two(); return true; }();
	}
}
