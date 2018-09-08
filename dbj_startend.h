#pragma once

/*
Following creates unique type (and instance)
as long as it is not repeated
somewehere else in the same app
-------------------------------------------------------------
void f1 () { printf("Start once!"); } ;
void f2 () { printf("End   once!"); } ;
static inline auto gbe =  dbj::begin_end_singleton<f1,f2>() ;
*/

#pragma region begin_end

namespace dbj {
	    
	    auto vvl = []() -> void {};
		using  voidvoidfun = void(*) ();
		using  vv_lambda = decltype( vvl );

		/*
		call ATSTART on start and at ATEND 
		on end of instance lifetime
		(possibly too)  simple class to do whatever two functions provided do when called
		*/
		template< typename BF, typename EF>
		class __declspec(novtable)	begin_end final
		{
			BF begin_f_ ;
			EF end_f_   ;
			/*stop copying moving and swapping*/
			begin_end(const begin_end &) = delete;
			begin_end & operator = (const begin_end &) = delete;
			begin_end(begin_end &&) = delete;
			begin_end && operator = (begin_end &&) = delete;
			/* we also hide the default ctor so that instances of this class can not be delete-d */
			begin_end() {};
		public:
			explicit begin_end( BF begin_, EF end_) 
				: begin_f_(begin_), 
				  end_f_(end_)
			{ 
					// do something once and upon construction
					try {
						begin_f_();
					}
					catch (...) {
						throw "Calling ATSTART() failed in " __FUNCDNAME__;
					}
			}

			~ begin_end() {
					// before destruction do something once
					try {
						end_f_();
					}
					catch (...) {
						// must not throw from destructor
						DBJ::TRACE("Calling ATEND() failed in %s", __FUNCSIG__);
					}
			}
		};

	} // dbj
#pragma endregion

/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017,2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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
