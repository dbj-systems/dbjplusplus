#pragma once
#include "../dbj_gpl_license.h"
#include "../util/dbj_stack_matrix.h"
#include "../testfwork/dbj_testing.h"

DBJ_TEST_SPACE_OPEN(dbj_stack_matrix)

using namespace ::dbj::arr;

template< typename MX, typename T >
inline void test_mx(T new_val)
{
	dbj::console::print("\n ID: ", MX::uuid(), ", size: ", MX::size(), ", rank: ", MX::rank());
	// can update this way -- auto & mx = MX::data() ; mx[1][1] = T(1);
	// or this way -- MX::data(1, 1) = T(1) ;
	// or this way
	MX::for_each(
		[&](typename MX::value_type & val, size_t r, size_t c)
		->bool
	{
		val = new_val++;
		noexcept(r, c); //dummy usage to pacify msvc
		return true; // proceed
	}
	);
	MX::printarr(dbj::console::print);
}

template< typename MX>
inline MX test_mx_arg_retval(MX the_mx)
{
	// leave the trace
	the_mx.data(
		the_mx.rows() - size_t(1),
		the_mx.cols() - size_t(1)
	) = typename MX::value_type(1234);
	return the_mx;
}

DBJ_TEST_UNIT(dbj_static_matrix_multiplication) 
{
	// for_each() callback type
	// bool(*)(value_type &, size_t, size_t);
	auto filler = []( auto & val_, size_t col, size_t row) -> bool {
		val_ = int(1); // int(col + row);
		return true;
	};
	/*
	  A[n][m] x B[m][p] = R[n][p]
	  stack_matrix<T,R,C,UID> matrix is R x C matrix
	  Cols of A must be the same as rows of B
	  R must be A rows x B cols
	*/
	using A = stack_matrix<int, 0xF,		0xF,	DBJ_UID >;
	using B = stack_matrix<int, A::cols(),	0xF,	DBJ_UID >;
	using R = stack_matrix<int, A::rows(),	B::cols(),	DBJ_UID >;

	A::for_each(filler);
	B::for_each(filler);

	stack_matrix_multiply<A, B, R>();

	R::printarr(::dbj::console::print);

	std::reference_wrapper<int[A::rows()][A::cols()]> ref_a = std::ref(A::data());
	A::matrix_ref_type mr_a = ref_a;

	std::reference_wrapper<int[B::rows()][B::cols()]> ref_b = std::ref(B::data());
	B::matrix_ref_type mr_b = ref_b;

	std::reference_wrapper<int[R::rows()][R::cols()]> ref_r = std::ref(R::data());
	R::matrix_ref_type mr_r = ref_r;

	::dbj::console::print("\nRezut is checked by freivald to be: ", ::dbj::arr::inner::freivald(mr_a, mr_b, mr_r) );
}

DBJ_TEST_UNIT(naked_static_matrix) 
{
	using ::dbj::console::print;

	int A[][3] = { {1,1,1},{1,1,1},{1,1,1} };
	int B[][3] = { {1,1,1},{1,1,1},{1,1,1} };
	int C[][3] = { {0,0,0},{0,0,0},{0,0,0} };

	::dbj::arr::inner::multiply(A, B, C);
	::dbj::console::print("\nresult:\n");
	::dbj::console::print("\n", C[0]);
	::dbj::console::print("\n", C[1]);
	::dbj::console::print("\n", C[2]);

	auto times = 0xFF;
	print("\n\nNow we will ask Freivald ", times, ", times to check this result...\n");
	for (int j = 0; j < times; j++) 
	{
		print("\nRezut is checked by freivald to be: ", ::dbj::arr::inner::freivald(A, B, C));
	}
}

DBJ_TEST_UNIT(dbj_static_matrix) {

	constexpr size_t R = 3, C = 3;

	// dbj static matrix solution
	// NOTE! bellow are two different types 
	// since DBJ_UID produces two different UID's
	// NOTE! do not try or test DBJ_UID in a loop as that
	// makes all the uid's to be the same; as DBJ_UID
	// is replaced with a value only once and first, by a pre-processor
	using mx9a = stack_matrix<int, R, C, DBJ_UID >;
	using mx9b = stack_matrix<int, R, C, DBJ_UID >;

	_ASSERTE(mx9a::uuid() != mx9b::uuid());

	test_mx<mx9a>(0);
	test_mx<mx9b>(100);

	// instances are ok but perfectly
	// redundant in this context
	mx9a mxa = test_mx_arg_retval(mx9a());
	mx9b mxb = test_mx_arg_retval(mx9b());

	mxa.printarr(dbj::console::print);
	mxb.printarr(dbj::console::print);
}


DBJ_TEST_UNIT(cpp_dynamic_arrays)
{
	/*
	for dynamic solution I will use std share_ptr as bellow
	*/
	constexpr size_t R = 3;
	constexpr size_t C = 3;
	// shared pointer based solution
	using matriq = std::shared_ptr<int[R][C]>;
	matriq m3(new int[R][C]{ {1,2,3},{1,2,3},{1,2,3} });
	m3.get()[1][1] = 42;
	// printarr((int(&)[R][C])(*m3.get()));

	// shared pointer based *faster* solution
	using matriq2 = std::shared_ptr<int[]>;
	matriq2 m32(new int[R * C]{ 1,2,3,1,2,3,1,2,3 });
	int r = 1, c = 1;
	// *(m32.get() + r * C + c) = 42;
	m32.get()[(r * C + c)] = 42;
	// printarr((int(&)[R][C])(*m32.get()));
}

DBJ_TEST_SPACE_CLOSE

