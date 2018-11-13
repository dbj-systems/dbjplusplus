#pragma once
/*
her we offer a "proper" main that deals with uncaught exceptions
in a standard way
*/

// #define DBJ_WMAIN_USED

#ifdef DBJ_WMAIN_USED

#if defined(_WIN32) || defined (_WIN64)

#endif

namespace dbj 
{
	inline std::wstring app_base_name ;
	/*
	this works only if and when called from inside the catch block
	*/
	inline void handle_eptr(std::exception_ptr eptr) // passing by value is ok
	{
		using namespace ::dbj::console;
		try {
			if (eptr) {
				std::rethrow_exception(eptr);
			}
		}
		catch (const std::exception& x_) {
			print("\nstd exception: ", x_.what());
		}
	}

	inline void final_exceptions_processor()
	{
		using namespace ::dbj::console;

		std::exception_ptr eptr;

		print("\n", app_base_name, " -- Exception caught! -- ", painter_command::bright_red);

		try {
			throw;
		}
		catch (::dbj::Exception & x_) {
			print("dbj Exception, ", x_.what());
		}
		catch (std::system_error & x_) {
			print("system error: ", x_.code());
		}
		catch (std::exception & x_) {
			print("std exception: ", x_.what());
		}
		catch (...) {
			eptr = std::current_exception(); // final capture
		}
		handle_eptr(eptr);
		print(painter_command::text_color_reset, "\n");
	}

} // dbj


extern "C" void dbj_program_start(int argc, const wchar_t * argv[], const wchar_t *envp[]);

#if defined(UNICODE) || defined(_UNICODE)
int wmain(const int argc, const wchar_t *argv[], const wchar_t *envp[])
#else
#error "What could be the [t]reason this is not an UNICODE build?"
int main(int argc, char* argv[], char *envp[])
#endif
{
	try {

		dbj::app_base_name = argv[0];
		dbj::app_base_name.erase(0,
			1 + dbj::app_base_name.find_last_of(L"\\")
		);

		dbj_program_start(argc, argv, envp);
	}
	catch (...) {
		dbj::final_exceptions_processor();
	}
	return  0;
}

#endif // DBJ_WMAIN_USED


/*
Copyright 2017, 2018 by dbj@dbj.org

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
