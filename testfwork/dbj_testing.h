#pragma once
/*
(c) dbj.org, see the license at file bottom

An C++17 micro standalone testing framework

if DBJ_TESTING_ONAIR is undefined test are not compiled and not executed.
dbj::testing::RUNING is also true or false constexpr, depending on it

// C++17 
if constexpr(dbj::testing::RUNING) {
// testing code will be compiled into existence
// or not
}

How to add test unit, with no macros:

#if DBJ_TESTING_ONAIR
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
// will do nothing if DBJ_TESTING_ONAIR is undefined
dbj::testing::execute();
return 0;
}

No exceptions are thrown outside. They are reported to console.

*/

#include "../core/dbj_synchro.h"
// #include "dbj_micro_log.h"
#include "../util/dbj_string_util.h"

namespace dbj {
	namespace testing {

		using namespace std::string_view_literals;

		constexpr inline auto TITLE = "dbj++ Testing Framework [" __DATE__ "]"sv ;
		constexpr inline auto ALLDN = "dbj++ Testing Framework -- ALL TESTS DONE"sv ;

		using testunittype = void(*)();
		inline void __stdcall null_unit() {}

		namespace internal {

			// sort by key is std::map default behaviour
			// if compiler can not find a comparator for the key type 
			// one has to be provided
			struct function_ptr_comparator {
				bool operator () (const testunittype & lhs, const testunittype & rhs) const
				{
					return std::addressof(lhs) < std::addressof(rhs);
				};
			};

			// test unit function pointer is the key of the tests map
			typedef  std::map<
				testunittype,
				std::string, function_ptr_comparator > TUMAP;

			inline TUMAP & tumap_instance()
			{
				static TUMAP single_instance
					= [&]() -> TUMAP {
					// this is anonymous lambda 
					// called only once
					// do some more complex initialization
					// here, if need be
					return {};
				}(); // call immediately
				return single_instance;
			};
			// C++17 inline vars
			inline TUMAP & dbj_tests_map_ = tumap_instance();


			inline  bool operator == (
				const TUMAP::value_type & pair_,
				const TUMAP::key_type & rhs_) noexcept
			{
				return pair_.first == rhs_;
			}

			inline  TUMAP::iterator find(testunittype tunit_) {
				return dbj_tests_map_.find(tunit_);
			}

	       inline  bool found(testunittype tunit_) {
				return (
					dbj_tests_map_.end() != find(tunit_)
					);
			}

			   /// <summary>
			   /// we could have called append straight from client code 
			   /// technicaly the adder struct bellow is not necessary 
			   /// but it is here for chage resilience of this design 
			   /// </summary>
			   /// <param name="tunit_"></param>
			   /// <param name="description_"></param>
			   inline  auto append(testunittype tunit_, 
				       std::unique_ptr<char[]> const & description_) 
			   {

				   auto next_test_id = []() -> std::string {
					   static int tid{ 0 };
					   return   "[TID:" + dbj::str::string_pad(tid++, '0', 3) + "]";
				   };

				   std::string final_description_(next_test_id()); 
				   final_description_.append(description_.get());

				   /* the same test unit ? do not insert twice */
				   auto rez = 
					   dbj_tests_map_.try_emplace( tunit_ ,final_description_ );

				   // NOTE: rez.second is false if no insertion ocured
				   if (rez.second == false) {
					   ::dbj::core::trace("\nNot inserted %s, because found already", final_description_.c_str());
				   }
				   else {
					   ::dbj::core::trace("\nInserted test unit: %s", final_description_.c_str());
				   }
				   return rez.first;
			   }

				inline  void unit_execute(testunittype tunit_) {

					tunit_();

				}

			struct adder final {
				inline auto operator ()(
					std::unique_ptr<char[]> const & msg_, 
					testunittype tunit_ 
					) const noexcept
				{
					// mt safe in any build
					dbj::sync::lock_unlock auto_lock;
					return internal::append(tunit_, msg_);
				}

				 adder ( )  noexcept
				{	
					 ::dbj::core::trace("%s", __func__);
				}

			};

			// https://dbj.org/c-play-it-only-once-sam/
			inline  const adder & 
				adder_instance() 
			{
			// mt safe in any build
			static adder singleton_{ };
			  return singleton_ ;
			}

		   } // internal  

		// C++17 inline vars
		inline const internal::adder & 
			add = internal::adder_instance() ;

	} // testing
} // dbj

#ifdef DBJ_TEST_UNIT
#error "DBJ_TEST_UNIT Already defined?"
#else
/*
moved to dbj_crt.h
#define DBJ_STR_IMPL(x) #x
#define DBJ_STR(x) DBJ_STR_IMPL(x)
#define DBJ_CONCAT_IMPL( x, y ) x##y
#define DBJ_CONCAT( x, y ) DBJ_CONCAT_IMPL( x, y )
*/

// #define DBJ_TEST_SPACE_OPEN(x) namespace DBJ_CONCAT(dbj_test_namespace_, x) {  \
//constexpr const char * dbj_test_namespace_name_ = DBJ_STR(DBJ_CONCAT(dbj_test_namespace_, x)); \
//__pragma(message("\n" __FILE__  "(" DBJ_STR(__LINE__) ")\nCompiling : " DBJ_STR(DBJ_CONCAT(dbj_test_namespace_, x))));

//#define DBJ_TEST_SPACE_CLOSE \
//__pragma(message("\n" __FILE__ "(" DBJ_STR(__LINE__) ")\nCompiled: " DBJ_STR(DBJ_CONCAT(dbj_test_namespace_, x)) "\n")); \
//}

#ifndef DBJ_NO_TESTING
#define DBJ_TEST_SPACE_OPEN(x) namespace _dbj_testing_namespace_ {  
#define DBJ_TEST_SPACE_CLOSE }
#else
#define DBJ_TEST_SPACE_OPEN(x) #if 0
#define DBJ_TEST_SPACE_CLOSE } #endif
#endif
/*
remember: anonymous namespace variableas are static by default
that is they are "internal linkage"
thus bellow we generate unique namespace name too
to avoid that phenomenon
*/
#define DBJ_TEST_CASE_IMPL(description, name ) \
void name(); \
namespace __dbj_register__namespace_  { \
  inline auto DBJ_CONCAT( dbj_dumsy_ , __COUNTER__ ) \
           = ::dbj::testing::add( description, name ); \
} \
inline void name() 

#define DBJ_TEST_CASE( description, x ) \
DBJ_TEST_CASE_IMPL ( description , DBJ_CONCAT( __dbj_test_unit__, x ) )

#define DBJ_TEST_UNIT(x) DBJ_TEST_CASE( ::dbj::core::fileline(__FILE__, __LINE__) , x )

#endif

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

/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"