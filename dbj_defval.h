#pragma once
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
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

#include <assert.h>

#include <functional>
#include <utility>
#include <optional>
namespace dbj {

	/*
	Task: implement this for any type
	inline int default_int_width(int new_ = 1) {
		static int dflt_ = new_;
		if (new_ != dflt_) dflt_ = new_;
		return new_;
	}
	*/

#pragma region "Version THREE"
	/*
	No stunts. A plain old function object.
	Simple usable and working
	*/
#pragma warning( push ) 
#pragma warning( disable : 4522 )
	template<typename T>
	class holder {
		// mutable members of const class instances are modifiable
		mutable T default_value_{};
		// ask compiler for default ctor
		holder() _NOEXCEPT = default;
		// dbj::holder is neither copyable nor movable.
		holder(const holder&) = delete;
		holder& operator=(const holder&) = delete;
		holder& operator=(const holder&) volatile = delete;
	public:
		holder(const T & defval_) : default_value_(defval_) {	};
		/* first operator () overload just returns the value */
		const T & operator () () const noexcept {
			return default_value_;
		}
		/*
		second operator () overload sets and returns the value
		please read
		https://msdn.microsoft.com/en-us/library/031k84se.aspx
		to understand the operator declaration bellow
		it allows for const isntances to be used
		through the operator bellow. Example 
		const holder<REAL> width{ 10 }; // set to 10
		width(1024) ; // OK to change to 1024
		*/
		const volatile T & operator ()
			(const T & new_) const volatile noexcept
		{
			if (new_ != default_value_)
				default_value_ = new_;
			return default_value_;
		}
	};
#pragma warning( pop ) 

#ifdef	DBJ_TESTING_EXISTS
	namespace {
		using namespace Gdiplus;

		// test the const onstance behaviour
		struct S {
			mutable holder<REAL> width{ 10 };
		};

		const S konst_{}; // must  be initialized if const

		holder<SmoothingMode> smoothnes{ SmoothingMode::SmoothingModeAntiAlias };
		holder<LineCap> linecap{ LineCap::LineCapRound };

		DBJ_TEST_CASE("dbj def val option three") {
			auto width_ = konst_.width(1024);
			const auto w1_ = konst_.width();
			auto w2_ = konst_.width();

			assert(width_ == 1024);
			assert(w2_ == w1_ && w2_ == width_ && w1_ == width_);

			auto lc_ = linecap();
			auto sness = smoothnes();
		}
	}
#endif
#pragma endregion "Version THREE"

#pragma region "Version TWO"
	/*
	Function template returning a lambda
	Closer to real stunt, but works.
	And compiler has less problems disambiguating the instances	vs the solution one
	type T has to be move assignable
	*/
	template<typename T,
	typename std::enable_if< std::is_move_assignable_v<T> > * = 0 >
	inline auto holder_maker(T defval_) {

		/*
		Requirement: auto df = defval() should return def val on first and all the other calls.
		So what do we do to make this behave as  we want  it to? 
		On the first call we execute the lambda and return it. 
		Thus on second and all the other calls it will be used with a proper def val from the first call
		*/
		auto defval_handler = [&](
			/* No if we do bellow T new_ = defval_ , that will not compile, try...
			   the elegant (or is it a stunt?) way arround, is to use std::optional
			   BIG NOTE 2: lambda bellow requires T to be a moveable type
			*/
			const std::optional<T> & new_ = std::nullopt
			) -> const T &&
		{
			static T default_value = defval_;
			if (new_ != std::nullopt) {
				T new_val_ = new_.value_or(default_value);
				if (new_val_ != default_value)
						default_value = new_val_ ;
				}
			return std::forward<T>( default_value );
		};

		/* only on the first call execution will reach here and
		will return the lambda to be used on the rest of the calls */
		static auto first_call_ = defval_handler();
		return defval_handler;
	}

#ifdef	DBJ_TESTING_EXISTS
	namespace {
		using namespace Gdiplus;

		DBJ_TEST_CASE("def val option 2") {
			// create default values holders
			// creation happens since these are first calls
			// nice, but what is stopping us to do this 2 or more times?
			// we will have 2 or more lambdas for each def val type
			auto default_smoot = holder_maker(SmoothingMode::SmoothingModeAntiAlias);
			auto default_lncap = holder_maker(LineCap::LineCapRound);
			auto default_width = holder_maker(10);

			// just get the dflt width
			auto dw0 = default_width();
			auto dw1 = default_width();
			// note: const ref is returned so we can not assign (without stunts) 
			// to value
			// somewhere and sometimes latter someone might change the default value
			auto dw2 = default_width(42);
		}
	}
#endif

#pragma endregion "Version TWO" 

#pragma region "Version ONE"
#if 0


	template< typename T, T defval_ = T() >
	inline T & static_default(const T & new_ = defval_)
	{
		static T default_value = defval_;
		if (new_ != default_value)
			default_value = new_;
		return default_value;
	}

	namespace {
		using namespace Gdiplus;

		const auto sm0 = static_default<SmoothingMode, SmoothingMode::SmoothingModeAntiAlias>();
		const auto sm1 = static_default<SmoothingMode>();

		auto sm2 = static_default<SmoothingMode, SmoothingMode::SmoothingModeAntiAlias>;
		auto lc0 = static_default<LineCap, LineCap::LineCapRound>;
		auto sm3 = static_default<SmoothingMode>;
		auto dlcF = static_default<LineCap>;

		auto dlc = dlcF(LineCap::LineCapRound);

		/*
		but there is a problem here ...
		auto default_problem = static_default < REAL, REAL{10} > ;

		error C3535: cannot deduce type for 'auto' from 'overloaded-function'
		error C2440: 'initializing': cannot convert from 'T &(__cdecl *)(const T *)' to 'int'
		note: Context does not allow for disambiguation of overloaded function

		above template instance is not "different enough" from two previous ones
		*/
	}


#endif
#pragma endregion "Version ONE"
} // namespace dbj