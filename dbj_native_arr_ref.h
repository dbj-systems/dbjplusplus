#pragma once
/*
used through dbj++.h
thus dbj_crt.h is included before this

#include <vector>
#include <ctime>
*/

#include <array>

/// <summary>
/// NARF == Native ARray reFerence
/// </summary>
namespace dbj::narf {

	//---------------------------------------------------------------
	// NARF core
	//
	// the key abstraction
	// ARF == Array ReFerrence
	// ARF contains instance of native array T[N]
	// ARF can be safely copied/moved arround
	// in essence one uses ARF to carry arround native arrays
	// unlike std::array
	// ARF easily delivers reference to native array T[N]&
	// example: for() can be easily applied to T[N]&
	//  for (auto & e : arf.get() ) {}
	//
	// std::reference_wrapper<T>
	// -- can not be moved, just copied
	// -- contains T *
	//    which thus might easily be a pointer to stack space
	//---------------------------------------------------------------
	template <typename T, std::size_t N>
	using wrapper = std::reference_wrapper<T[N]>;

	template<typename T, size_t N>
	constexpr auto begin(const wrapper<T, N> & wrp_) {
		return std::begin(wrp_.get());
	}

	template<typename T, size_t N>
	constexpr auto end(const wrapper<T, N> & wrp_) {
		return std::end(wrp_.get());
	}


	/// <summary>
	/// returns std::reference_wrapper copy
	/// that contains reference to native array 
	/// this can be also used with init list
	/// <code>
	///  auto ari = 
	///  native_arr_ref({ 0,1,2,3,4,5,6,7,8,9 });
	///  auto arc = 
	///  native_arr_ref({ "Char array" });
	/// </code>
	/// </summary>
	template<typename T, std::size_t N >
	constexpr auto
		make( const T ( & native_arr)[N])
		// -> wrapper<T, N>
	{
		static_assert( ! std::is_const<T>(), "dbj::narf::make(T(&)[N]) -- T can not be const");
		static_assert( ! std::is_void<T>(), "dbj::narf::make(T(&)[N]) -- T can not be void");
		static_assert( ! std::is_reference<T>(), "dbj::narf::make(T(&)[N]) -- T can not be reference");
		static_assert( ! std::is_null_pointer<T>(), "dbj::narf::make(T(&)[N]) -- T can not be null pointer");
		//		static_assert( false == std::is_pointer<T>(), "dbj::narf::make() -- T can not be a pointer" );

		using nativarref =  T(&)[N];
		
		return std::ref(
			native_arr
		);
	}

	/// <summary>
	/// returns std::reference_wrapper copy
	/// that contains reference to native array 
	/// in turn contained inside the std::array
	/// *value* argument
	/// </summary>
	template<typename T, std::size_t N >
	constexpr auto
		make(const std::array<T, N> & std_arr)
		-> wrapper<const T, N>
	{
		using nativarref = const T(&)[N];

		// a stunt?
		return dbj::narf::make(
			(nativarref)*(std_arr.data())
		);
	}

	/// <summary>
	/// the ARF size is the
	/// size of n-array it holds
	/// </summary>
	template<typename T, std::size_t N >
	constexpr std::size_t size( const wrapper<T, N> & narw_) {
		return N; //  array_size(narw_.get());
	}

	/// <summary>
	/// the NARF data
	/// is the reference of n-array it holds
	/// please note the difference to 
	/// the std::data() version for
	/// native arrays, which returns
	/// a pointer to n-array
	/// <code>
	/// template <class T, std::size_t N>
	///   constexpr T* data(T(&array)[N]) noexcept
	///  {
	///	   return array;
	///  }
	/// </code>
	/// to use bellow one will tend to use auto, but
	/// one will get different types in relation to 
	/// the declaration of the rezult
	/// <code>
	/// auto narf = dbj::narf::make("narf to Array of chars");
	/// auto native_array_pointer = dbj::narf::data(narf);
	/// decltype(auto) not_elegant_ref = dbj::narf::data(narf);
	/// auto & the_arr_ref = dbj::narf::data(narf);
	/// </code>
	/// </summary>
	template<typename T, std::size_t N, typename ARF = T(&)[N] >
	constexpr ARF data(const wrapper<T, N> & narw_) {
#if _DEBUG
		auto p1 = narw_.get() ;
		_ASSERTE(p1);
		// p1 might accidentlay be ok
		// ditto, lets try once more
		auto p2 = narw_.get();
		_ASSERTE(p2);
#endif
		// and finnaly
		return narw_.get(); ;

		// same as 
		// return (ARF)narw_;
	}

	// homage to the range concept
	// auto [B,E] = dbj::narf::range( <some narf> )
	//
	template<typename T, std::size_t N>
	constexpr auto range(const wrapper<T, N> & narw_) {
		return std::array<T*, 2>{
			dbj::narf::begin(narw_), dbj::narf::end(narw_)
		};
	}

	/// <summary>
	/// default for_each function
	/// note: REF is about references
	/// thus the changes will stay in 
	/// the containing native array after
	/// we leave this function
	/// 
	/// please make sure to read
	/// https://en.cppreference.com/w/cpp/algorithm/for_each
	/// to understand the std::for_each 
	/// requirements and behaviour
	/// </summary>
	template< typename T, size_t N, typename FUN >
	constexpr auto for_each(const wrapper<T, N> & arf, const FUN & fun_) {
		const auto & B = begin(arf);
		const auto & E = end(arf);
		return std::for_each(B, E, fun_);
	}

	/// <summary>
	/// generic apply function
	/// callback signature:
	/// <code>
	/// [] ( size_t idx_ , const auto & element ) {}
	/// </code>
	/// </summary>
	template< typename T, size_t N, typename CBK >
	constexpr auto apply(
		const wrapper<T, N> &  arf,
		CBK output_function
	) {
		size_t j = 0;
		auto outfun_wrap = [&](/*const*/ T & element_) {
			output_function(j++, element_);
		};

		return dbj::narf::for_each(arf, outfun_wrap);
	}

} // dbj::narf



/*
Copyright 2017-2018 by dbj@dbj.org

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


