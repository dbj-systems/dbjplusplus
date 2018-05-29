#pragma once

/*
include dbj_crt.h before this

#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <map>
*/

/// <summary>
/// The Command Line encapsulation aka TCL-ENC
/// by dbj.org
/// (c) statement is on the bottom of this file
/// <remarks>
/// Perhaps interesting in this design is the decision 
/// not to succumb to C like cli 
/// which is based on char ** (or wchar_t ** in case of Windows). 
/// Internaly to represent the command line we will  use 
/// <code>
///  std::array< std::string, size_t > ;
/// </code>
/// when dealing with the actual cli we will transform asap to  this
/// thus internaly we do not have to deal with raw pointers.
/// </remarks>
/// </summary>
namespace dbj::app_env {

	/// <summary>
	/// we develop only unicode windows app's
	/// this dats type we use everyhwere
	/// to provide CLI interface implementation
	/// </summary>
	using data_type = std::vector< std::wstring >;

	using map_type =
		std::map<std::wstring, std::wstring>;

	namespace {

		inline auto app_env_initor () {

			/// <summary>
			/// transform argw to data_type
			/// base your cli proc code on data_type
			/// instead of raw pointers 
			/// data_type is standard c++ range
			/// </summary>
			/// <param name="args">__argw</param>
			/// <param name="ARGC">__argc</param>
			/// <returns>
			/// instance of data_type
			/// </returns>
			auto command_line_data = [](size_t ARGC, wchar_t **  args)
				-> data_type
			{
				_ASSERTE(*args != nullptr);
				return data_type{ args, args + ARGC };
			};

			/// <summary>
			/// pointers to the runtime environment
			/// from UCRT
			/// </summary>
#define _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY
			wchar_t **  warg = (__wargv);
			std::size_t argc = (__argc);
			wchar_t **  wenv = (_wenviron);
#undef _CRT_DECLARE_GLOBAL_VARIABLES_DIRECTLY

			/// <summary>
			///  we are here *before* main so 
			/// __argv or __argw might be still empty
			/// thus we will wait 1 sec for each of them 
			/// if need to be
			///  TODO: this is perhaps naive implementation?
			/// </summary>
			if (*warg == nullptr) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			_ASSERTE(*warg != nullptr);

			if (*wenv == nullptr) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			_ASSERTE(*wenv != nullptr);

			// extract the unicode  command line
			data_type
				warg_data{ command_line_data(argc, warg) };

			// calculate the count of env vars 
			auto count_to_null = [](auto ** list_) constexpr->size_t {
				size_t rez{ 0 };
				_ASSERTE(*list_ != nullptr);
				for (; list_[rez] != NULL; ++rez) {};
				return rez;
			};

			size_t evc = count_to_null(wenv);

			// extract the environment pointer list
			data_type
				wenvp_data{ command_line_data(evc, wenv) };
			app_env::map_type
				wenvp_map{};

			//transform env vars to k/v map
			// each envar entry is 
			// L"key=value" format
			for (auto kv : wenvp_data) {
				auto delimpos = kv.find(L"=");
				auto key = kv.substr(0, delimpos);
				auto val = kv.substr(delimpos + 1);
				wenvp_map[key] = val;
			}

			return make_tuple(argc, warg_data, evc, wenvp_map);
		};
	}

	// client code gets the instance of this
	class structure final {
	public:
		const std::size_t	cli_args_count{};
		const data_type		cli_data{};
		const std::size_t	env_vars_count{};
		const map_type		env_vars{};

		operator std::string  () const noexcept 
		{
			return { typeid(*this).name() };
		}

	private:

		void* operator new(std::size_t sz);
		void operator delete(void* ptr);

		structure(
			std::size_t argc,
			data_type   argv,
			std::size_t env_count,
			map_type	env_vars_
		)
			: cli_args_count(argc),
			cli_data(argv),
			env_vars_count(env_count),
			env_vars(env_vars_)
		{}

	public:
		static structure instance() {
			auto once = []() {
				auto[argc, warg, envc, env_map] = app_env_initor();
				return structure{ argc, warg, envc, env_map };
			};
			static structure app_env_single_instance_ = once();
			return app_env_single_instance_;
		}
	};

} // dbj::cli

  /* standard suffix for every dbj.org header */
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
