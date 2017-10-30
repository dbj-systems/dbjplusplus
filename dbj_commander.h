#pragma once
/* include through dbj++.h */
/* Command pattern mechanism */
#pragma region "commands"
namespace dbj {
	namespace cmd {
		
		template < typename T>
		struct less {
			typedef typename less type;
			constexpr bool operator()(const T &lhs, const T &rhs) const
			{
				return lhs < rhs;
			}
		};
#if 0
		template<
			typename CMD_ENUM,
			typename CMD_FUN,
			typename CMD_COMPARATOR = void,
			typename Enable = void
		> class Commander {};
#endif
		/*  CMD_ENUM defined commands id's
		CMD_FUN is function type to execute them. Whatever satisfies std::is_function<CMD_FUN>
		CMD_COMPARATOR is function to compare the CMD_ENUM elements
		*/
		template<
			typename CMD_ENUM, 
			typename CMD_FUN, 
			typename CMD_COMPARATOR = typename dbj::cmd::less<CMD_ENUM>::type,
			typename Enable = std::enable_if<std::is_function_v<CMD_FUN>>::type >
		  /* specialization for functions */
			class Commander
			{
			public:
				Commander() {}

				using comparator_type = CMD_COMPARATOR;
				using executor_type = std::function<CMD_FUN>;
				/*
				typename bellow is necessary since compiler at the moment of compilation of the template
				does not know that executor_type::result_type exists, it can not predict that
				std::function<CMD_FUN> can be compiled in future instantiations and thus can not predict
				that ::result_type will be possible to use
				this 'typename' was not required before C++11
				this is required only when compiling templates even wothout any instantions of them

				DBJ 2017-10-25
				*/
				typedef typename executor_type::result_type executor_return_type;
				using command_map_type = std::map<	CMD_ENUM, executor_type, comparator_type >;

				/*	commander's function */
				const executor_return_type execute(const CMD_ENUM & command) const
				{
					try {
						return command_map_.at(command)();
					}
					catch (std::out_of_range &) {
						throw  dbj::Exception(" Unknown command?");
					}
				}

				/* register a function by key given
				if function found will throw the exception if replace is false -- this is default
				if replace == true will replace if found

				*/
				template< typename F>
				const Commander & insert(const CMD_ENUM & command_, F function_) const
				{
					try {   // do not replace
						auto fun = command_map_.at(command_);
						// found + do not replace
					}
					catch (std::out_of_range &) {
						// new registration
						command_map_[command_] = executor_type(function_);
					}
					return (*this);
				}

				template< typename F>
				const Commander & replace(const CMD_ENUM & command_, F function_) const
				{
					try {   // found it, so replace
						auto fun = command_map_.at(command_);
						command_map_[command_] = executor_type(function_);
					}
					catch (std::out_of_range &) {
						// not found, just ignore
					}
					return (*this);
				}

			private:
				mutable command_map_type command_map_{};
		};
	} // cmd
} // dbj
#pragma endregion "commands"

  /* standard suffix */
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ )   /*
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



