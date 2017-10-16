#pragma once

namespace dbj {
#pragma region STARTEND

		using  voidvoidfun = void(*) ();
		/*
		(possibly too)  simple class to do whatever two functions provided do when called
		*/
		template< voidvoidfun ATSTART, voidvoidfun ATEND>
		class __declspec(novtable)	STARTEND final
		{
			unsigned int & counter()
			{
				static unsigned int counter_ = 0;
				return counter_;
			}
			/*stop copying moving and swapping*/
			STARTEND(const STARTEND &) = delete;
			STARTEND & operator = (const STARTEND &) = delete;
			STARTEND(const STARTEND &&) = delete;
			STARTEND && operator = (const STARTEND &&) = delete;
			/* also singleton swap has no meaning */
			friend void swap(STARTEND &, STARTEND &) {
				__noop;
			}
			/* we also hide the default ctor so that instances of this class can not be delete-d */
			STARTEND() {
				const UINT & ctr = (counter())++;
				if (0 == ctr) {
					// do something once and upon construction
					try {
						ATSTART();
					}
					catch (...) {
						throw "Calling ATSTART() failed in " __FUNCDNAME__;
					}
				}
			}
		public:
			/* TODO: resilience in presence of multiple threads */
			STARTEND & singleton() {
				static STARTEND  singleton_;
				return singleton_;
			}

			~STARTEND() {
				const UINT & ctr = --(counter());
				if (0 == ctr) {
					// before destruction do something once
					try {
						ATEND();
					}
					catch (...) {
						throw "Calling ATEND() failed in " __FUNCDNAME__;
					}
				}
			}
		};
		/*
		Following creates unique type as long as it is not repeated
		somewehere else in the same app
		-------------------------------------------------------------
		void f1 () { printf("Start once!"); } ;
		void f2 () { printf("End   once!"); } ;
		auto gbe =  STARTEND<f1,f2>::singleton() ;
		*/
	}
#pragma endregion STARTEND
/* standard suffix for every header here */
#define DBJVERSION __DATE__ __TIME__
#pragma message( "--------------> Compiled: " __FILE__ ", Version: " DBJVERSION)
#pragma comment( user, "(c) " STR(__YEAR__) " by dbj@dbj.org | Version: " DBJVERSION ) 
#undef DBJVERSION
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