#pragma once
/*
(c) dbj.org, see the license at file bottom

An C++17 micro standalone testing framework

if DBJ_TESTING_EXISTS is undefined test are not compiled and not executed.
dbj::testing::RUNING is also true or false constexpr, depending on it

// C++17 
if constexpr(dbj::testing::RUNING) {
// testing code will be compiled into existence
// or not
}

How to add test unit, with no macros:

#if DBJ_TESTING_EXISTS
namespace {
static auto dummy = dbj::testing::add(
" Critical Test" ,
[](){ std::cout << "Test" ;}
) ;
}
#endif

instead of in line lambda one can add function name or lambda name
and anonymous namespace is optional but good practice.
Here is the mandatory macro for certified pain lovers:

DBJ_TEST_CASE( "Critical Test" )
{
std::cout << "Test" ;
}

All tests are collected as declared and then executed from main() like so:

int main(int argc, char* argv[])
{
// will do nothing if DBJ_TESTING_EXISTS is undefined
dbj::testing::execute();
return 0;
}

No exceptions are thrown outside. They are reported to console.

*/
#include "dbjio.h"
#include <map>

namespace dbj {
	namespace testing {
		// if not false on command line it will be compiled into existence
#ifdef DBJ_TESTING_EXISTS
		constexpr bool RUNING = true;
#else
		constexpr bool RUNING = false;
#endif
		using testunittype = void(*)();
		static __forceinline void __stdcall null_unit() {}

		struct Exception : public std::runtime_error {
		public:
			typedef std::runtime_error _Mybase;

			explicit Exception(const std::string & _Message)
				: _Mybase(_Message.c_str())
			{	// construct from message string
			}

			explicit Exception(const char *_Message)
				: _Mybase(_Message)
			{	// construct from message string
			}
		};

	}
}

namespace dbj {
	namespace testing {
#ifdef DBJ_TESTING_EXISTS


		namespace {

			struct FPcomparator {
				bool operator () (const testunittype & lhs, const testunittype & rhs) const
				{
					return lhs < rhs;
				};
			};

			// test unit function pointer is the key
			typedef  std::map<
				testunittype,
				std::string, FPcomparator > TUMAP;

			DBJ_INLINE bool operator == (
				TUMAP::value_type pair_,
				TUMAP::key_type rhs_) noexcept
			{
				return pair_.first == rhs_;
			}

			static __forceinline TUMAP & tu_map() {
				static TUMAP test_units_{};
				return test_units_;
			}

			DBJ_INLINE TUMAP::iterator find(testunittype tunit_) {
				return tu_map().find(tunit_);
			}

			DBJ_INLINE bool found(testunittype tunit_) {
				return (
					tu_map().end() != find(tunit_)
					);
			}

			DBJ_INLINE void append(testunittype tunit_, const std::string & description_) {
				/* do not insert twice the same test unit */
				if (!found(tunit_))
					tu_map()[tunit_] = description_;
			}

			DBJ_INLINE bool unit_execute(testunittype tunit_) {
				try {
					tunit_();
					return true;
				}
				catch (...) {
					throw dbj::testing::Exception(
						std::string("Unknown exception while executing testing unit") + tu_map()[tunit_]
					);
				}
			}

			struct adder {
				const bool operator ()(const std::string & msg_, testunittype tunit_) const {
					/* we could have called append straight from client code */
					/* technicaly this struct is not necessary but it is here for chage resilience of this design */
					append(tunit_, msg_);
					return true;
				}

				static  adder & instance() {
					static adder singleton_{};
					return singleton_;
				}
			};
		}
		adder & add = adder::instance();

#define DBJ_STR(x) #x
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )
#define DBJ_TEST_UNIT_REGISTER( description, function ) namespace { static auto DBJ_CONCAT( __dbj_dummy__, __COUNTER__ ) = dbj::testing::add( description, function ); }
#define DBJ_TEST_CASE_IMPL(description, name ) static void name(); DBJ_TEST_UNIT_REGISTER(description, name); static void name() 

#define DBJ_TEST_CASE( description ) DBJ_TEST_CASE_IMPL( description , DBJ_CONCAT( __dbj_test_unit__, __COUNTER__ ))

		/*  execute all the tests collected  */
		DBJ_INLINE void _stdcall execute() noexcept {

			using dbj::io::printex;

			printex("\n\nSTARTING [", tu_map().size(), "] tests\n\n");
			for (auto tunit : tu_map())
			{
				try {
					printex("\n\tTEST BEGIN [", tunit.second, "]\n");
					unit_execute(tunit.first);
					printex("\n\tTEST END   [", tunit.second, "]\n");
				}
				catch (dbj::testing::Exception & x) {
					printex("\n\t\tException: [", x.what(), "] thrown from the testing unit: ");
				}
			}
			printex("\n\nFINISHED ALL Tests\n\n");
		}
#else // DBJ_TESTING_EXISTS is undefined
		auto add = [](void(*)()) { return true; };
		static __forceinline void _stdcall execute() { }

#define DBJ_TEST_UNIT(code)

#endif // 
	} // testing
} // dbj

#define DBJVERSION __DATE__ __TIME__
#pragma message( "--------------> Compiled: " __FILE__ ", Version: " DBJVERSION)
#pragma comment( user, "(c) " __DATE__ " by dbj@dbj.org | Version: " DBJVERSION )
#undef DBJVERSION
  /*
  Copyright 2017 by dbj@dbj.org

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http ://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  */
