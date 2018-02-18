#pragma once

#include <vector>
#include <array>

namespace dbj {
	namespace arr {
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
			constexpr inline
			ARF
			internal_array_reference(const std::array<T, N> & arr)
		{
			return *(ARP) const_cast<typename ARR::pointer>(arr.data());
		}

		/*
		Array Handler

		(c) 2018 by dbj.org
		*/
		template< typename T, size_t N >
		struct ARH
		{
			// std::array type
			typedef std::array<T, N> ARR;
			// inbuilt ARray type
			typedef T ART[N];
			// reference to ART
			typedef ART& ARF;
			// pointer to ART
			typedef ART* ARP;

			/*
			return pointer to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARP
				to_arp(const ARR & arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/*
			return reference to the underlying array
			of an instance of std::array<T,N>
			*/
			static constexpr ARF
				to_arf(const ARR & arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}
		};

			/*		"C"	array to vector			*/
			template<typename Type, size_t N, typename VType = std::vector<Type> >
			inline constexpr VType
				intrinsic_array_to_vector(const Type(&arr_)[N])
			{
				return VType{ arr_, arr_ + N };
			}

			/*		"C"	array to std array			*/
			template<typename Type, size_t N, typename AType = std::array<Type,N> >
			inline constexpr AType &&
				intrinsic_array_to_array(const Type(&arr_)[N])
			{
				AType retval_{};
				std::copy(arr_, arr_ + N, retval_.begin() );
				return std::forward<AType>(retval_);
			}

#ifdef DBJ_TESTING_EXISTS
		inline void test()
		{
			{
				// the "C" way
				char arr_of_chars[]{ 'A','B','C' };
				char(&ref_to_arr_of_chars)[3] = arr_of_chars;
			}
			{   // manual C++ old school
				std::array<char, 3> three_chars{ 'A','B','C' };
				const char(&uar)[3] = *(char(*)[3])three_chars.data();
			}
			// the modern C++ dbj way
			using A16 = ARH<int, 3>;
			A16::ARR arr { 1,2,3 };
			A16::ARP arp = A16::to_arp(arr);
			A16::ARF arf = A16::to_arf(arr);

			// prove that the type is right
			auto rdr0 = intrinsic_array_to_vector(arf);

			/*
			testing the internal_array_reference

			decltype(auto) bellow reveals the underlying type
			namely it transform int* to int(&)[]
			that is reference to c array inside std::array
			*/
			decltype(auto) arf2 = internal_array_reference(arr);
			decltype(auto) rdr1 = intrinsic_array_to_vector(arf2);

			decltype(auto) arf3 = arf2;
			auto rdr2 = intrinsic_array_to_vector(arf3);
		}
#endif // DBJ_TESTING
	} // arr
} // dbj

/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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

