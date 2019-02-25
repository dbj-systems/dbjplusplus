#pragma once

#include "../win/dbj_uuid.h"
/*

Why this?
---------
T[][N] would be called "array of array [N] of T" and be an incomplete matrix_type,
while T[][] would be an invalid matrix_type (all except the last dimensions
must have a known size).

T[N][M] is "array [N] of array[M] of T",
while T[sizeX] is "array [sizeX] of T" where T is a pointer to an int.
Creating dynamically an 2d array works like this:

 // C++11 onwards -- allocate (with initializatoin)
	auto array = new double[M][N]();
	delete [] array ;

It will create an array of an allocated-matrix_type int[X][Y].
This is a "hole" in C++'s matrix_type system, since the ordinary matrix_type system
of C++ doesn't have array dimensions with sizes not known at compile time,
thus these are called "allocated types"

But. If you are worried about above being slow and scatered arround non contiguous
memory blocks, you can use the following:

int *ary = new int[sizeX * sizeY]; // single memory block

Access x,y element as:

	 ary[y*sizeX + x]

Don't forget to do:  delete[] ary.
int Matrix based on the above might look like:

class Matrix {
	int* array;
	int m_width;
	public:
	Matrix( int w, int h ) : m_width( w ), array( new int[ w * h ] )
	{
	}

	~Matrix() {
		delete[] array;
	}
	int at( int x, int y ) const
	{
		return array[ index( x, y ) ];
	}
	protected:
	int index( int x, int y ) const { return x + m_width * y; }
};

This is certianly better memory layout but equaly certainly more
computation intensive.

We can now have lambdas return lvalue references, which can make things
a little more user friendly:

constexpr size_t num_cols = 0xF ;
constexpr size_t num_rows = 0xF ;
auto one_dim_array = new int[num_rows * num_cols];
auto two_dim_array = [num_cols, &one_dim_array](size_t row, size_t col) -> int&
{
   return one_dim_array[row*num_cols + col];
}

Use like :

	 two_dim_array(i, j) = 5
Really just like modern macros. Could be the fastest solution.

A bit of a conceptual and practila mess ..

Thus from me first the all stack all static almost-a-pod variant.
In a standard C++ way.
*/

#pragma warning( push)
#pragma warning( disable: 4307 )

// NOTE: __COUNTER__ is also a GCC predefined macro 
// not just MSVC
// #define DBJ_UID  __COUNTER__ + dbj::util::dbj_get_seed()
#define DBJ_UID  __COUNTER__ + 100
// BIG WARNING: __COUNTER__ macro does not work in a loop. 
// It is a macro, a simple text substituton pre-processing mechanism

namespace dbj::arr {
	/*
	GPLv3 (c) 2018
	dbj's static matrix_type , almost a pod.
	*/
	template <
		typename	T,
		size_t		R,
		size_t		C,
		std::uint64_t UID_,
		size_t		MAXSIZE = 0xFFFF
		/*
		INT_MAX as a absolute max is 0x7FFFFFFF -- limits.h
		but it is very unlikely your compiler will allow such
		a large array on the stack to be created
		*/
	>
		class stack_matrix final
	{
	public:
		using uid_type = std::uint64_t;
		// 'type' is like a 'this' for static template instances
		// without it code bellow will be much more complex
		using type = stack_matrix;
		/*	we also clean const and volatile if used "by accident" */
		using value_type = std::remove_cv_t<T>;
		using matrix_type = value_type[R][C];
		using matrix_ref_type = value_type(&)[R][C];
	private:
		/* here we enforce usage policies first, specifically

		The 64-bit PECOFF executable format used on Windows
		doesn't support creating executables that have
		a load size of greater than 2GB

		Since this matrix is all on stack we have choosen here
		much smaller MAXSIZE then INT_MAX  which is 0x7FFFFFFF -- limits.h
		and thus enforce that as stack matrix size policy here
		*/
		static_assert(
			(R * C * sizeof(value_type)) < /* = */ MAXSIZE,
			"Total size of stack_matrix must not exceed 0xFFFF (65536) bytes"
			);

		static_assert(
			std::is_pod_v<value_type>,
			"stack_matrix can be made out of POD types only"
			);

		/*
		almost just a pod 2D array on stack
		since it is static we must implement some mechinsm so that
		every definition does NOT share this same 2d native array
		this is the role of the UID__ template parameter
		*/
		inline static value_type data_[R][C]{};

		/*
		IMPOTANT: this is R x C matrix
		*/

	public:
		/*
		please make sure you do understand how this makes for template definition
		wide uid, because it is a template parameter.
		stack_matrix<int,4,4,0>
		is different type vs
		stack_matrix<int,4,4,1>

		thus: if not for the last parameter, all the stack matrices of the same size 
		and type will share the same data. 
		we add the last param and thus we have two different types.
		*/
		static constexpr const uid_type  uuid() { return UID_; }
		static constexpr char const * type_name() { return typeid(matrix_type).name(); }
		static constexpr size_t rows() noexcept { return R; };
		static constexpr size_t cols() noexcept { return C; };
		static constexpr size_t size() noexcept {
			return sizeof(matrix_type);
			// same as: R * C * sizeof(value_type);
			// same as sizeof(value_type[R][C]) 
		};
		static constexpr size_t rank()		noexcept { return std::rank_v  <  matrix_type   >; }
		static constexpr size_t max_size()	noexcept { return MAXSIZE; };

		/*
		construcotr is largely irrelevant for anything but
		sanity cheks of the implementation
		*/
		constexpr explicit stack_matrix()
		{
			static_assert(2 == std::rank_v  <  matrix_type   >);
			static_assert(R == std::extent_v< matrix_type, 0 >);
			static_assert(C == std::extent_v< matrix_type, 1 >);
		}
		~stack_matrix() { }

		/*
		Not returning pointer but reference
		this is criticaly important quality of this design
		alo this reference never becomes ivalid as it refers to
		a static data block
		*/
		constexpr  static matrix_ref_type data() noexcept
		{
			return matrix_ref_type(type::data_);
		}

		// data overload for changing the cells in the matrix
		// return type is a reference so we can both
		// set and get values in the matrix cells easily
		constexpr static value_type & data(size_t r, size_t c) noexcept
		{
			return (value_type &)(type::data_[r][c]);
		}

		// callback signature has to be
		// bool (*fun)( value_type & val, size_t row, size_t col);
		// if callback returns false, process stops
		// if callback throws the exception, process stops
		using callback = bool(*)(value_type &, size_t, size_t);

		/*
		for each row visit every column
		*/
		template<typename F>
		constexpr static void for_each(const F & fun)
		{
			for (size_t row_ = 0; row_ < R; row_++) {
				for (size_t col_ = 0; col_ < C; col_++) {
					bool result = 
						fun((value_type &)type::data_[row_][col_], row_, col_); 
					if (result == false)
						return;
				}
			}
		}

		// utility function for printing to the console
		// F has to be a variadic print function
		// lambda example of such a function:
		//
		// inline auto print = 
		//     [](const auto & first_param, auto && ... params);
		//
		template<typename F>
		constexpr static void printarr(F print)
		{
			print("\n", type::type_name());
			for (int r = 0; r < R; r++)
			{
				print("\n{ ");
				for (int c = 0; c < C; c++)
					// note how F has to be able to print the type T of this matrix
					print(" {", type::data_[r][c], "} ");
				print(" }");
			}
			print("\n");
		}

		template<typename A, typename B, typename R>
		friend void stack_matrix_multiply();

	}; // stack_matrix<T,R,C,UID>

	namespace inner {
		/*
	C11 code to multiply two matrices:
	https://codereview.stackexchange.com/questions/179043/matrix-multiplication-using-functions-in-c?answertab=active#tab-top
		*/

		/*
		theory:
		The definition of matrix multiplication is that:
		if C = AB for an n × m matrix A and an m × p matrix B, then C is an n × p matrix
		https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm
		*/

		template<typename T, size_t N, size_t M, size_t P>
		inline void multiply(T(&a)[N][M], T(&b)[M][P], T(&c)[N][P])
		{
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < P; j++) {
					c[i][j] = 0;
					for (int k = 0; k < M; k++) {
						c[i][j] += a[i][k] * b[k][j];
					}
				}
			}
		}

		// one notch above the most simple iterative algorithm above
		// is the one bellow .. the recursive version
		template<typename T, size_t N, size_t M, size_t P>
		inline void multi_rx(T(&a)[N][M], T(&b)[M][P], T(&c)[N][P])
		{
			// i and j are used to keep the current cell of 
			// result matrix C[i][j]. k is used to keep the
			// current column number of A[.][k] and row 
			// number of B[k][.] to be multiplied 
			static int i = 0, j = 0, k = 0;

			// If all rows traversed. 
			if (i >= N)
				return;

			// If i < N 
			if (j < P)
			{
				if (k < M)
				{
					c[i][j] += a[i][k] * b[k][j];
					k++;
					multi_rx(a, b, c);
				}
				k = 0;	j++;
				multi_rx(a, b, c);
			}
			j = 0;	i++;
			multi_rx(a, b, c);
		}
	}
	/*
	  A[n][m] x B[m][p] = R[n][p]

	stack_matrix<T,R,C,UID> matrix is R x C matrix
	*/
	template<typename A, typename B, typename R>
	inline void stack_matrix_multiply()
	{
		// check types to match on all 3 matrices
		static_assert(std::is_same_v<A::value_type, B::value_type>,
			"\n\n" __FUNCSIG__ "\n");
		static_assert(std::is_same_v<A::value_type, R::value_type>,
			"\n\n" __FUNCSIG__ "\n");
		// Rows of A must be the same as columns of B
		static_assert(A::cols() == B::rows(),
			"\n\n" __FUNCSIG__ "\n");
		// R must be rows of A x cols of B
		static_assert(R::rows() == A::rows(),
			"\n\n" __FUNCSIG__ "\n");
		static_assert(R::cols() == B::cols(),
			"\n\n" __FUNCSIG__ "\n");

		// ::dbj::arr::multiply
		::dbj::arr::inner::multi_rx
			/*< A::value_type, A::rows(), A::cols(), B::cols() >*/
		(A::data_, B::data_, R::data_);
	}

#pragma warning( pop )

} // eof namespace dbj::arr
