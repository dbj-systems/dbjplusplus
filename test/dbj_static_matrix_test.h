#pragma once
#include "../dbj_gpl_license.h"
#include "../dbj_stack_matrix.h"
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

DBJ_TEST_UNIT(dbj_static_matrix) {

	constexpr size_t R = 3, C = 3;

	// dbj static matrix solution
	using mx9a = compile_time_stack_matrix<int, R, C, DBJ_UID >;
	using mx9b = compile_time_stack_matrix<int, R, C, DBJ_UID >;

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
#if 0
// example call
// printarr((int(&)[3][3])array);
template<typename T, size_t M, size_t N>
inline void printarr(const T(&array)[M][N])
{
	using dbj::console::print;
	print("\n", typeid(T(&)[M][N]).name());
	// 
	for (int r = 0; r < M; r++)
	{
		print("\n[ ");
		for (int c = 0; c < N; c++)
			print(" [", array[r][c], "] ");
		print(" ]");
	}
	print("\n");
}
#endif

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

