#pragma once

#ifndef UNICODE
#error __FILE__ requires UNICODE builds
#endif

#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>
#endif

namespace dbj {
	namespace win32 {
#pragma region "dbj win32 strings"
		using CHAR_T = wchar_t;
		using STRING = std::wstring;
		using long_string_pointer = CHAR_T *; // LPWSTR;
#pragma endregion "dbj win32 strings"

											  //Returns the last Win32 error, in string format. Returns an empty string if there is no error.
		__forceinline auto getLastErrorMessage(
			const STRING & prompt = STRING{}, DWORD errorMessageID = ::GetLastError()
		)
		{
			//Get the error message, if any.
			if (errorMessageID == 0)
				return STRING(); //No error message has been recorded

			long_string_pointer messageBuffer = nullptr;

			size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (long_string_pointer)&messageBuffer, 0, NULL);

			STRING message(messageBuffer, size);

			//Free the buffer.
			::LocalFree(messageBuffer);

			if (!prompt.empty())
				return STRING(prompt).append(message);

			return message;
		}

		__forceinline auto getLastErrorMessage(
			const char * prompt, DWORD errorMessageID = ::GetLastError())
		{
			return getLastErrorMessage(dbj::wide(prompt), errorMessageID);
		}

		namespace sysinfo {
			using std::string;
			DWORD	INFO_BUFFER_SIZE = 1024;
			static wstring  infoBuf(INFO_BUFFER_SIZE, (char)0);
			//
			template<class F, class... Pack>
			constexpr __forceinline auto
				call
				(F&& fun, Pack&&... args) {
				infoBuf.clear();
				if (0 == std::invoke(fun, (args)...))
					throw getLastErrorMessage(typeid(F).name());
				return (infoBuf);
			}
			// 
			__forceinline STRING computer_name() {
				static STRING computer_name_ =
					call(GetComputerName, infoBuf.data(), &INFO_BUFFER_SIZE);
				return computer_name_;
			}

			__forceinline STRING user_name() {
				static STRING user_name_ =
					call(GetUserName, infoBuf.data(), &INFO_BUFFER_SIZE);
				return user_name_;
			}

			__forceinline STRING system_directory() {
				static STRING system_directory_ =
					call(GetSystemDirectory, infoBuf.data(), INFO_BUFFER_SIZE);
				return system_directory_;
			}

			__forceinline STRING windows_directory() {
				static STRING windows_directory_ =
					call(GetWindowsDirectory, infoBuf.data(), INFO_BUFFER_SIZE);
				return windows_directory_;
			}
		} // sysinfo
#ifdef DBJ_TESTING_EXISTS
		namespace {
			using namespace sysinfo;
			DBJ_TEST_CASE(dbj::nicer_filename(__FILE__)) {
				try {
					dbj::io::printex(
						"\n\t\t", DBJ_NV(computer_name()),
						"\n\t\t", DBJ_NV(user_name()),
						"\n\t\t", DBJ_NV(system_directory()),
						"\n\t\t", DBJ_NV(windows_directory())
					);
				}
				catch (...) {
					dbj::io::printex("\nUknown Exception?");
				}
			};
		}
#endif // DBJ_TESTING_EXISTS
	} // win32
} // dbj

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
