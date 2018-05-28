#pragma once
/*
#include <vector>
#include <array>
#include <ctime>
*/
namespace dbj::arr {

	// http://cpptruths.blogspot.rs/2011/10/multi-dimensional-arrays-in-c11.html
	template <class T, size_t ROW, size_t COL>
	using Matrix = std::array<std::array<T, COL>, ROW>;
	// usage: Matrix<float, 3, 4> mat;

	template <class T, size_t ROW, size_t COL>
	using NativeMatrix = T[ROW][COL];
	// usage: NativeMatrix<float, 3, 4> mat;

	namespace {
		using namespace std;

		// http://en.cppreference.com/w/cpp/experimental/to_array
		namespace {
			template <class T, size_t N, size_t... I>
			/*constexpr*/ inline array<remove_cv_t<T>, N>
				to_array_impl(T(&a)[N], index_sequence<I...>)
			{
				return { { a[I]... } };
			}
		}
	}

	/*
	Transform native array into std::array at compile time
	*/
	template <class T, std::size_t N>
	inline array<remove_cv_t<T>, N> native_to_std_array(T(& narf)[N])
	{
		return to_array_impl(narf, make_index_sequence<N>{});
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

		/*
		native ARray Helper

		(c) 2018 by dbj.org
		*/
		template< typename T, size_t N >
		struct ARH
		{
			// vector type
			typedef std::vector<T> ARV;
			// std::array type
			typedef std::array<T, N> ARR;
			// inbuilt ARray type
			typedef T ART[N];
			// reference to ART
			typedef ARH::ART& ARF;
			// pointer to ART
			typedef ARH::ART* ARP;

			/*
			return pointer to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARP
				to_arp( ARR arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/*
			return reference to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARF
				to_arf( ARR arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}

			/*		native	array to vector			*/
			static constexpr ARV
				to_vector(const ARF arr_)
			{
				return ARV{ arr_, arr_ + N };
			}

			/*		"C"	array to std array			*/
			static constexpr ARR 
				to_std_array(const ARF arr_)
			{
				ARR retval_{};
				std::copy(arr_, arr_ + N, retval_.begin() );
				return retval_;
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

