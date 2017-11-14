#pragma once
// #undef DBJ_COM
#ifdef DBJ_COM	
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define STRICT
#include <objbase.h>
namespace com {
	namespace {
		/*
		In anonymous namespace we hide the auto-initializer
		This ensures that COM is initialized “as soon as possible”
		This mechanism really works. Convince yourself once through the
		debugger, and then just forget about COM init/uninit.
		*/
		class __declspec(novtable) COMAUTOINIT final
		{
			unsigned int & counter()
			{
				static unsigned int counter_ = 0;
				return counter_;
			}
			/*stop copying moving and swapping*/
			COMAUTOINIT & operator = (const COMAUTOINIT &) = delete;
			COMAUTOINIT(const COMAUTOINIT &&) = delete;
			COMAUTOINIT && operator = (const COMAUTOINIT &&) = delete;
			/* also singleton swap has no meaning */
			friend void swap(COMAUTOINIT &, COMAUTOINIT &) {
				__noop;
			}
			/*
			also hide the default ctor so that instances can not be delete-d

			If you call ::CoInitialize(NULL), after this method is used
			most likely the HRESULT will be :
			hRes = 0×80010106 — Cannot change thread mode after it is set.
			*/
			COMAUTOINIT()
			{
				const UINT & ctr = (counter())++;
				if (0 == ctr)
#if ( defined(_WIN32_DCOM)  || defined(_ATL_FREE_THREADED))
					HRESULT result = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
					HRESULT result = ::CoInitialize(NULL);
#endif
				/*TODO: log the result here*/
			}
		public:
			COMAUTOINIT(const COMAUTOINIT &) = default;
			/* TODO: resilience in presence of multiple threads */
			static COMAUTOINIT & singleton() {
				static COMAUTOINIT  singleton_;
				return singleton_;
			}

			~COMAUTOINIT()
			{
				const UINT ctr = --(counter());
				if (ctr < 1)
					::CoUninitialize();
			}

		};
		static auto insta = COMAUTOINIT::singleton() ;
	} // anonspace
} // com
#endif // DBJ_COM

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
/*
The only requirement for this file is not to have any #include's
*/
