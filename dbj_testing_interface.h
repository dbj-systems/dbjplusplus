#pragma once

#include "dbj_testing.h"
#include "dbj_console.h"
#include "dbj_console_painter.h"

/*
   testing interface depends on the dbj::console
   therefore it is moved here so that the whole testing
   does not depend on dbj::console
*/
namespace dbj {
	namespace testing {
		
		typedef typename dbj::console::painter_command CMD;
		using dbj::console::print;

		template< typename ... Args >
		inline void text_line (
			CMD && cmd_ , 
			Args && ... args 
		) {
			paint(cmd_); 
			print( "\n" );
			if constexpr (sizeof...(args) < 1 ) {
				print(DBJ::LINE());
			}
			else {
				print( args... );
			}
			paint(CMD::text_color_reset);

		};

		inline auto white_line = [&]( auto && ... args ) {
				text_line( CMD::white, args... );
		};

		inline auto blue_line = [&](auto && ... args) {
			text_line(CMD::bright_blue, args...);
		};

		inline auto green_line = [&](auto && ... args) {
			text_line(CMD::green, args... );
		};

		inline auto red_line = [&](auto && ... args) {
			text_line(CMD::bright_red, args ... );
		};

		inline auto prefix (
			/* 
			   since we do only unicode builds
			   argv[0] is wchar_t *
			 */
			const wchar_t * prog_full_path 
		) {
			white_line();
			white_line( dbj::testing::TITLE, "\n");
			white_line( DBJ::YEAR(), " by ", DBJ::COMPANY());
			white_line();
			white_line("[", internal::dbj_tests_map_.size(), "] tests registered");
			white_line("Application: ", DBJ::FILENAME(dbj::range_to_string(prog_full_path)));
			white_line();
		}

		inline auto suffix() {
			white_line();
			white_line("\n", dbj::testing::ALLDN);
			white_line();
			// no needed --> print(CMD::text_color_reset);
		}

		inline auto unit_prefix(const char * name_) {
			blue_line();
			blue_line("BEGIN TEST UNIT ", name_ , " ");
			blue_line();
		}

		inline auto unit_suffix(const char * name_) {
			blue_line();
			blue_line("\nEND TEST UNIT ", name_, " \n");
		}

		inline auto space_prefix(const char * name_) {
			white_line();
			white_line("Runtime started", name_ );
			white_line();
		}

		inline auto space_suffix(const char * name_) {
			white_line();
			white_line("\nRuntime finished", name_ );
			white_line();
		}

		/*  execute all the tests collected  */
		inline void _stdcall execute(
			const int		DBJ_MAYBE(argc),
			const wchar_t * DBJ_MAYBE(argv) [],
			const wchar_t * DBJ_MAYBE(envp) []
		) noexcept 
		{
			if ( internal::dbj_tests_map_.size() < 1) {
				white_line();
				white_line("No tests registered");
				white_line();
				return;
			}

			prefix(argv[0]);
			for (auto && tunit : internal::dbj_tests_map_ )
			{
				unit_prefix(tunit.second.c_str());
				try {
					white_line(" ");
					internal::unit_execute(tunit.first);
					white_line(" ");
				}
				catch (dbj::Exception & x) {
					red_line(x);
				}
				catch (...) {
					red_line( dbj::Exception("\nUnknown Exception") );
				}
				unit_suffix(tunit.second.c_str());
			}
			suffix();
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
		template<typename return_type>
		// inline decltype(auto)
		inline return_type &
			test_lambada(const char * expression, const return_type & anything)
		{
			using namespace dbj::console;
			static dbj::c_line<80, '-'> L80;
			dbj::console::print(
				painter_command::green, "\n", L80,
				"\n- expression -> ", painter_command::text_color_reset, expression,
				painter_command::green, "\n\t- rezult type-> ", painter_command::text_color_reset, typeid(anything).name(),
				painter_command::green, "\n\t\t- value -> ", painter_command::text_color_reset, anything);
			// return std::move(anything) ;
			return const_cast<return_type &>( anything ) ;
		};

#define DBJ_TEST_ATOM(x) dbj::testing::test_lambada( DBJ_EXPAND(x), [&] { return (x);}() ) 
#endif // DBJ_TEST_ATOM

	} // testing
} // dbj

  /*
  Copyright 2017,2018 by dbj@dbj.org

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
