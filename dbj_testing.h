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

#include <string>
#include <map>

#include "dbj_util.h"

#ifndef DBJ_NV
/*
use dbj print or printex to show the symbol and its value, for example:
printex ( DBJ_NV( typeid(whatever).name ), DBJ_NV( typeid(xyz).name ) ) ;

WARNING: this is primitive, use with caution
TODO: if symbol contains comma this is not going to work
*/
#define DBJ_NV_DELIMITER " , "
#define DBJ_NV( symbol) DBJ_EXPAND(symbol)  DBJ_NV_DELIMITER , symbol 
#endif

namespace dbj {
	namespace testing {

		inline const char * TITLE  { "dbj++ Testing Framework [" __DATE__ "]" };
		inline const char * ALLDN  { "dbj++ Testing Framework -- ALL TESTS DONE" };
		inline const char * LINE   { "--------------------------------------------------------------------------------" };
		inline const char * COMPANY{ "DBJ.Systems Ltd." };
		inline const char * YEAR   { (__DATE__ + 7) } ;

		inline std::string  FILENAME(const std::string  &  file_path) {
				auto pos = file_path.find_last_of('\\');
				return
					(std::string::npos != pos
						? file_path.substr(pos, file_path.size())
						: file_path
						);
			}

		inline std::string FILELINE(const std::string & file_path, 
			unsigned line_, 
			const std::string & suffix = 0) 
		{
				return FILENAME(file_path) + "(" + std::to_string(line_) + ")"
					+ (suffix.empty() ? "" : suffix);
		}

		// if not false on command line it will be compiled into existence
#ifdef DBJ_TESTING_EXISTS
		constexpr bool RUNING = true;
#else
		constexpr bool RUNING = false;
#endif
		using testunittype = void(*)();
		static __forceinline void __stdcall null_unit() {}

		namespace {

			struct FPcomparator {
				bool operator () (const testunittype & lhs, const testunittype & rhs) const
				{
					return lhs < rhs;
				};
			};

			// test unit function pointer is the key of the tests map
			typedef  std::map<
				testunittype,
				std::string, FPcomparator > TUMAP;

			inline  bool operator == (
				TUMAP::value_type pair_,
				TUMAP::key_type rhs_) noexcept
			{
				return pair_.first == rhs_;
			}

			static TUMAP & tu_map() {
				static TUMAP test_units_{};
				return test_units_;
			}

			inline  TUMAP::iterator find(testunittype tunit_) {
				return tu_map().find(tunit_);
			}

	       inline  bool found(testunittype tunit_) {
				return (
					tu_map().end() != find(tunit_)
					);
	}

	inline  void append(testunittype tunit_, const std::string & description_) {

		auto next_test_id = []() -> std::string {
			static int tid{ 0 };
			return   "[TID:" +  dbj::util::string_pad( tid++, '0', 3 ) + "]" ;
		};
				/* do not insert twice the same test unit */
				// if (!found(tunit_))
					tu_map()[tunit_] = next_test_id() + description_ ;
	}

	inline  void unit_execute(testunittype tunit_) {
		         tunit_();
		}
#ifdef _DEBUG
	inline size_t adder_call_count() {
		static size_t count_{ 0 };
		return count_++;
	}
#endif
			struct adder {
				const bool operator ()(const std::string & msg_, testunittype tunit_, const int counter_ ) const {
#ifdef _DEBUG
DBJ::TRACE("\n\ndbj testing adder [%p] operator () called %d times, counter is: %d", this, adder_call_count(), counter_);
auto & seeit = adder::instances_registry;
#endif
					/* we could have called append straight from client code */
					/* technicaly this struct is not necessary but it is here for chage resilience of this design */
					append(tunit_, msg_);
					return true;
				}

				 static size_t instances_counter ;
				 static size_t instances_registry[256];

				 size_t IID{ 0 };

				adder( ) : IID(instances_counter++) {
					instances_registry[instances_counter++] = this->IID;
#ifdef _DEBUG
DBJ::TRACE("\ninstances_counter : %d, IID : %d", instances_counter, instances_registry[this->IID] );
#endif
				}

				static  adder && instance() {
					static adder singleton_{ };
					return std::forward<adder> ( singleton_) ;
				}
			};

			size_t adder::instances_counter{ 0 };
			size_t adder::instances_registry[256]{ 0 };

		} // namespace
		
		static inline adder && add = adder::instance();

	} // testing
} // dbj

#ifdef DBJ_TEST_UNIT
#error "DBJ_TEST_UNIT Already defined?"
#else

#define DBJ_STR(x) #x
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )
/*
#define DBJ_TEST_UNIT_REGISTER( description, function ) \
namespace { \
inline auto DBJ_CONCAT(__dbj_register__, function )\
      = dbj::testing::add( description, function, __COUNTER__ ); \
}
*/
#define DBJ_TEST_CASE_IMPL(description, name, counter_ ) \
static void name(); \
namespace { \
  static auto DBJ_CONCAT(__dbj_register__, name )\
      = dbj::testing::add( description, name, counter_ ); \
} \
static void name() 

#define DBJ_TEST_CASE( description, x ) \
DBJ_TEST_CASE_IMPL ( description , DBJ_CONCAT( __dbj_test_unit__, x ), x )

#define DBJ_TEST_UNIT(x) DBJ_TEST_CASE( dbj::testing::FILELINE(__FILE__, __LINE__, x) , __COUNTER__ )

#endif

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
