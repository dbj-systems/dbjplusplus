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
   static auto dummy = dbj::testing::add( " Critical Test" , [](){ std::cout << "Test" ;}
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

	}
}

namespace dbj {
	namespace testing {
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

			/*static*/ __forceinline TUMAP & tu_map() {
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
				tunit_();
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
	} // testing
} // dbj
  /* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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
