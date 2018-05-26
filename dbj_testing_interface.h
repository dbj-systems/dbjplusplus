#pragma once

#include "dbj_testing.h"
/*
   testing interface depends on the dbj::win::con
   therefore it is moved here so that the whole testing
   does not depend on the dbj::win::con
*/
namespace dbj {
	namespace testing {


		/*  execute all the tests collected  */
		inline void _stdcall execute() noexcept {

			typedef typename dbj::win::con::painter_command CMD;
			using dbj::print ;
			using namespace dbj::testing;

			auto white_line = [&]( const char * arg = "") { 
				print(arg, CMD::white, dbj::testing::LINE , CMD::text_color_reset);
			};
			auto blue_line =  [&]( const char * arg = "") { 
				print(arg, CMD::bright_blue, dbj::testing::LINE, CMD::text_color_reset);
			};

			white_line("\n");
			print("\n", dbj::testing::TITLE, "\n");
			print("\n(c)", dbj::testing::YEAR, " by ", dbj::testing::COMPANY);
			white_line("\n");
			print("\n[", tu_map().size(),"] tests defined");
			white_line("\n");
			for (auto tunit : tu_map())
			{
				// blue_line("\n");
				print(CMD::bright_blue, "\nBEGIN TEST UNIT [", tunit.second, "]", CMD::text_color_reset);
				blue_line("\n");
				try {
					print("\n");
					unit_execute(tunit.first);
					print("\n");
				}
				catch (dbj::Exception & x) {
					print(CMD::bright_red, x, CMD::text_color_reset);
				}
				catch (...) {
					print(CMD::bright_red,  dbj::Exception("\nUnknown Exception"), CMD::text_color_reset);
				}
				blue_line("\n");
				print(CMD::bright_blue, "\nEND TEST UNIT [", tunit.second,"]\n", CMD::text_color_reset);
				// blue_line("\n");
			}
			white_line("\n");
			print("\n", dbj::testing::ALLDN);
			white_line("\n");
			print(CMD::text_color_reset);
		}

#ifndef DBJ_TEST_ATOM

		///<summary>
		/// usage example: 
		/// <code>
		/// auto any_ = DBJ_TEST_ATOM( dbj::any::range({ 42 }) );
		/// </code>
		/// argument of the macro gets printed as a string and then
		/// get's executed as a single expression 
		/// of the lambda bodys
		/// whose return value is returned
		/// </summary>
		template< typename lambada_type >
		inline auto test_lambada(const char * expression, lambada_type && lambada)
		{
			using namespace dbj::win::con;
			auto anything = lambada();
			dbj::print(
				painter_command::green, "\n- expression -> ", painter_command::text_color_reset, expression,
				painter_command::green, "\n\t- rezult type-> ", painter_command::text_color_reset,  typeid(anything).name(),
				painter_command::green, "\n\t\t- value -> ", painter_command::text_color_reset, anything);
			return anything;
		};

#define DBJ_TEST_ATOM(x) dbj::testing::test_lambada( DBJ_EXPAND(x), [&] { return (x);} ) 
#endif // DBJ_TEST_ATOM

	} // testing
} // dbj
  /* standard suffix */
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
