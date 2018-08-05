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
			typedef T value_type;
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

			constexpr static const size_t size{ N };

			/*
			return pointer to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARP
				to_arp( const ARR & arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			// static constexpr ARP to_arp(ARR && arr) = delete;

			/*
			return reference to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARF to_arf( const ARR & arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			// static constexpr ARF to_arf(ARR && arr) = delete;


			/*		native	array to vector			*/
			static constexpr ARV
				to_vector(const ARF & arr_)
			{
				return ARV{ arr_, arr_ + N };
			}

			/* disallow args as references to temporaries */
			// static constexpr ARV to_vector( ARF && arr_) = delete;

			/*		"C"	array to std array			*/
			static constexpr ARR 
				to_std_array(const ARF & arr_)
			{
				ARR retval_{};
				std::copy(arr_, arr_ + N, retval_.begin() );
				return retval_;
			}
			/* disallow args as references to temporaries */
			// static ARR to_std_array(ARF && arr_) = delete;

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

