#pragma once
/*
   testing interface depends on the dbj::win::con
   therefore it is moved here so that the whole testing
   does not depend on the dbj::win::con
*/
namespace dbj {
	namespace testing {

		namespace {
			inline const char * line(const unsigned & len_ = 80) {
				static std::string line_(len_, '-');
				return line_.c_str();
			}
		}
		/*  execute all the tests collected  */
		DBJ_INLINE void _stdcall execute() noexcept {

			typedef typename dbj::win::con::CMD CMD;
			static dbj::win::con::WideOut wout{};

			auto white_line = [&]() { wout.print("%%", CMD::white, line()); };

			white_line();
			wout.print("\n\nStarting [%] tests\n\n", tu_map().size());
			white_line();
			for (auto tunit : tu_map())
			{
				try {
					wout.print("\n\tTEST BEGIN [%]\n", tunit.second);
					unit_execute(tunit.first);
					wout.print("\n\tTEST END [%]\n", tunit.second);
				}
				catch (dbj::Exception & x) {
					wout.print("%\n\t\tException: [%] thrown from the testing unit: % %", CMD::bright_red, x.what(), tunit.second, CMD::white);
				}
				catch (...) {
					wout.print("%\n\t\tUnknown Exception thrown from the testing unit: % %", CMD::bright_red, tunit.second, CMD::white);
				}
			}
			white_line();
			wout.print("\n\nFINISHED ALL Tests\n\n");
			white_line();
			wout.print("%", CMD::text_color_reset);
		}
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
