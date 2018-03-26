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

			typedef typename dbj::win::con::painter_command CMD;
			using dbj::print ;

			auto white_line = [&]( const char * arg = "") { 
				print(arg, CMD::white,line(), CMD::text_color_reset); 
			};
			auto blue_line =  [&]( const char * arg = "") { 
				print(arg, CMD::bright_blue,line(), CMD::text_color_reset); 
			};

			white_line("\n");
			print("\ndbj++ Testing Framework");
			white_line("\n");
			print("\n[", tu_map().size(),"] tests defined");
			white_line("\n");
			for (auto tunit : tu_map())
			{
				// blue_line("\n");
				print(CMD::bright_blue, "\nBEGIN [", tunit.second, "]", CMD::text_color_reset);
				blue_line("\n");
				try {
					print("\n");
					unit_execute(tunit.first);
					print("\n");
				}
				catch (dbj::Exception & x) {
					print(x);
				}
				catch (...) {
					print(dbj::Exception("\nUnknown Exception"));
				}
				blue_line("\n");
				print(CMD::bright_blue, "\nEND [", tunit.second,"]\n", CMD::text_color_reset);
				// blue_line("\n");
			}
			white_line("\n");
			print("\nFINISHED ALL Tests");
			white_line("\n");
			print(CMD::text_color_reset);
		}
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
