#pragma once
#include "dbj_traits.h"

namespace dbj::arr {

	template<typename T, size_t N>
	inline void array_copy(
		T(&dst)[N], const T(&src)[N]
	) {
		static_assert( 
			std::is_trivially_copy_assignable_v<T>, 
			" dbj::arr::array_copy() -- trivial copy-assignment is required of T"
		);
		void * rez = std::memcpy(dst, src, N * sizeof(T));
		_ASSERTE( rez );
	}

	// http://cpptruths.blogspot.rs/2011/10/multi-dimensional-arrays-in-c11.html
	template <class T, size_t ROW, size_t COL>
	using Matrix = typename std::array<std::array<T, COL>, ROW>;
	// usage: Matrix<float, 3, 4> mat;

	template <class T, size_t ROW, size_t COL>
	using NativeMatrix = T[ROW][COL];
	// usage: NativeMatrix<float, 3, 4> mat;

		// http://en.cppreference.com/w/cpp/experimental/to_array
		namespace inner {
			
			using namespace std;

			template <class T, size_t N, size_t... I>
			/*constexpr*/ inline array<remove_cv_t<T>, N>
				to_array_impl(T(&a)[N], index_sequence<I...>)
			{
				return { { a[I]... } };
			}

			// std array is indeed "tuple like"
			// but in some use cases one might 
			// need to make a tuple from an native array
			template <
				class T, 
				size_t N, 
				size_t... I>
			/*constexpr*/ inline 
				::dbj::tt::array_as_tuple_t< std::remove_cv_t<T>, N>
				to_tuple_impl(T(&a)[N], index_sequence<I...>)
			{
				return make_tuple( a[I]... );
			}

		}

	/*
	Transform native array into std::array at compile time
	*/
	template <class T, std::size_t N>
	inline std::array<std::remove_cv_t<T>, N> 
		native_to_std_array(T(& narf)[N])
	{
		return inner::to_array_impl(narf, std::make_index_sequence<N>{});
	}

/*
Transform native array into std::tuple at compile time
*/
	template <class T, std::size_t N >
	inline 
		::dbj::tt::array_as_tuple_t< std::remove_cv_t<T>, N>
		native_arr_to_tuple(T(&narf)[N])
	{
		return inner::to_tuple_impl(narf, std::make_index_sequence<N>{});
	}

	/*
	Transform native array into std::vector at compile time
	*/
	template<typename Type, size_t N, typename outype = std::vector<Type> >
	inline constexpr outype array_to_vector(const Type(&arr_)[N])
	{
		return { arr_, arr_ + N };
	}

		/*
		return array reference to the C array inside std::array
		usage:
				decltype(auto) iar =
					internal_array_reference(
							std::array<int,3>{1,2,3}
				) ;
		*/
		template<typename T, size_t N,
			typename ARR = std::array<T, N>, /* std::array */
			typename ART = T[N],    /* C array */
			typename ARF = ART & ,  /* reference to it */
			typename ARP = ART * >  /* pointer   to it */
				constexpr inline	ARF
			reference(const std::array<T, N> & arr)
		{
			return *(ARP) const_cast<typename ARR::pointer>(arr.data());
		}

		// we allow references, but not references to temporaries
		template<typename T, size_t N,
			typename ARR = std::array<T, N>, /* std::array */
			typename ART = T[N],    /* C array */
			typename ARF = ART & ,  /* reference to it */
			typename ARP = ART * >  /* pointer   to it */
			constexpr inline	ARF
			reference( std::array<T, N> && arr) = delete;

		/*
		native ARray Helper

		(c) 2018 by dbj.org
		*/
		template< typename T, size_t N >
		struct ARH
		{
			constexpr static const size_t size{ N };
			typedef T value_type;
			// vector type
			typedef std::vector<T> ARV;
			// std::array type
			typedef std::array<T, N> ARR;
			// native ARray type
			typedef T ART[N];
			// reference to ART
			typedef ARH::ART& ARF;
			// pointer to ART
			typedef ARH::ART* ARP;


			/*
			return pointer to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARP to_arp( const ARR & arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			static constexpr void to_arp(ARR && arr) = delete;

			/*
			return reference to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARF to_arf( const ARR & arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			static constexpr void to_arf(ARR && arr) = delete;

			/*
			return reference to the underlying array
			of an instance of std::vector<T>
			*/
			static constexpr ARF to_arf(const ARV & vct_)
			{
				return *(ARP) const_cast<typename ARR::pointer>(vct_.data());
			}

			/* disallow args as references to temporaries */
			static constexpr void to_arf(ARV && vct_) = delete;

			/* to std array from std vector */
			static constexpr ARR from_vector(const ARV vct_)
			{
				// T(&narf)[N] = *(T(*)[N])vct_.data();
				ARF narf = to_arf(vct_);
				ARR retval_;
					std::copy(narf, narf + N, retval_.data());
				return retval_;
			}

			/*		native	array to vector			*/
			static constexpr ARV to_vector(const ARF arr_)
			{
				return ARV{ arr_, arr_ + N };
			}

			/* disallow args as references to temporaries */
			static constexpr void to_vector( T (&&arr_)[N]) = delete;

			/*		"C"	array to std array			*/
			static constexpr ARR to_std_array(const ARF arr_)
			{
				ARR retval_{};
				std::copy(arr_, arr_ + N, retval_.begin() );
				return retval_;
			}
			/* disallow args as references to temporaries */
			static constexpr void to_std_array(T(&&arr_)[N]) = delete;

			/*		make and return empty std::array<T,N> */
			static constexpr ARR to_std_array()
			{
				return {};
			}

		}; // struct ARH

} // dbj::arr

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

