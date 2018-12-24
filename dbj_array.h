#pragma once
#include "dbj_traits.h"

namespace dbj::arr {

	using namespace std;

	template<typename T, size_t N>
	inline void array_copy(
		T(&dst)[N], const T(&src)[N]
	) {
		static_assert( 
			is_trivially_copy_assignable_v<T>, 
			" dbj::arr::array_copy() -- trivial copy-assignment is required of T"
		);
#ifdef _DEBUG
		void * rez =
#endif 
	    memcpy(dst, src, N * sizeof(T));
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
				::dbj::tt::array_as_tuple_t< remove_cv_t<T>, N>
				to_tuple_impl(T(&a)[N], index_sequence<I...>)
			{
				return make_tuple( a[I]... );
			}

		}

	/*
	Transform native array into std array at compile time
	*/
	template <class T, size_t N>
	inline array<remove_cv_t<T>, N> 
		native_to_std_array(T(& narf)[N])
	{
		return inner::to_array_impl(narf, make_index_sequence<N>{});
	}

	/*
	Transform native array into tuple at compile time
	*/
	template <class T, size_t N >
	inline 
		::dbj::tt::array_as_tuple_t< remove_cv_t<T>, N>
		native_arr_to_tuple(T(&narf)[N])
	{
		return inner::to_tuple_impl(narf, make_index_sequence<N>{});
	}

	/*
	Transform native array into vector at compile time
	*/
	template<typename Type, size_t N, typename outype = vector<Type> >
	inline constexpr outype array_to_vector(const Type(&arr_)[N])
	{
		return { arr_, arr_ + N };
	}

		/*
		return array reference to the C array inside array
		usage:
				decltype(auto) iar =
					dbj::arr::reference(
							array<int,3>{1,2,3}
				) ;
		*/
		template<typename T, size_t N,
			typename ARR = array<T, N>, /* array */
			typename ART = T[N],    /* C array */
			typename ARF = ART & ,  /* reference to it */
			typename ARP = ART * >  /* pointer   to it */
				constexpr inline	ARF
			reference(const array<T, N> & arr)
		{
			return *(ARP) const_cast<typename ARR::pointer>(arr.data());
		}

		// we allow references, but not references to temporaries
		template<typename T, size_t N,
			typename ARR = array<T, N>, /* array */
			typename ART = T[N],    /* C array */
			typename ARF = ART & ,  /* reference to it */
			typename ARP = ART * >  /* pointer   to it */
			constexpr inline	ARF
			reference( array<T, N> && arr) = delete;

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
			typedef vector<T> ARV;
			// array type
			typedef array<T, N> ARR;
			// native ARray type
			typedef T ART[N];
			// reference to ART
			typedef ARH::ART& ARF;
			// pointer to ART
			typedef ARH::ART* ARP;


			/*
			return pointer to the underlying array
			of an instance of array<T,N>
			*/
			static constexpr ARP to_arp( const ARR & arr)
			{
				return (ARP)const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			static constexpr void to_arp(ARR && arr) = delete;

			/*
			return reference to the underlying array
			of an instance of array<T,N>
			*/
			static constexpr ARF to_arf( const ARR & arr)
			{
				return *(ARP) const_cast<typename ARR::pointer>(arr.data());
			}

			/* disallow args as references to temporaries */
			static constexpr void to_arf(ARR && arr) = delete;

			/*
			return reference to the underlying array
			of an instance of vector<T>
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
					copy(narf, narf + N, retval_.data());
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
				copy(arr_, arr_ + N, retval_.begin() );
				return retval_;
			}
			/* disallow args as references to temporaries */
			static constexpr void to_std_array(T(&&arr_)[N]) = delete;

			/*		make and return empty array<T,N> */
			static constexpr ARR to_std_array()
			{
				return {};
			}

		}; // struct ARH

} // dbj::arr


/* inclusion of this file defines the kind of a licence used */
#include "dbj_gpl_license.h"