#pragma once

#if 0

#include <vector>
#include <variant>
#include <dbj_testing_interface.h>

DBJ_TEST_SPACE_OPEN(dbj_traits_testing)

	template<typename T, typename dbj::require_integral<T> = 0 >
	inline auto Object(T&& t) { return std::variant<T>(t); }

	template<typename T, typename dbj::require_floating<T> = 0>
	inline auto Object(T&& t) { return std::variant<T>(t); }

	/*usage*/
	DBJ_TEST_UNIT(" dbj basic traits tests"){

		DBJ_TEST_ATOM(dbj::is_floating<decltype(42.0f)>());
		DBJ_TEST_ATOM(dbj::is_integral<decltype(42u)>());
		DBJ_TEST_ATOM(dbj::is_object<decltype(42u)>());

		DBJ_TEST_ATOM(Object(42));
		DBJ_TEST_ATOM(Object(42.0f));
	}

	//-----------------------------------------------------------------------------
	// RC == micro Range and Container
	// I like it. A lot.
	/*
	template< typename T, std::size_t N>
	struct range_container final {
		using data_ref = T(&)[N];
		T data_[N]{};
		T * begin() { return data_; }
		T * end() { return data_ + N; }
		size_t size() const { return N; }
		data_ref data() const { return data_; }
	};
	*/

	DBJ_TEST_UNIT(" dbj container traits tests") 
	{
		using namespace std;

		using ia3 = array<int, 3>;
		using vi =  vector<int>;

		DBJ_TEST_ATOM( dbj::is_std_array_v<array<int,3>> );
		DBJ_TEST_ATOM( dbj::is_std_array_v<vector<int>>);

		DBJ_TEST_ATOM(dbj::is_std_vector_v<array<int, 3>>);
		DBJ_TEST_ATOM(dbj::is_std_vector_v<vector<int>>);

		DBJ_TEST_ATOM(dbj::is_range_v<array<int, 3>>);
		DBJ_TEST_ATOM(dbj::is_range_v<vector<int>>);

		DBJ_TEST_ATOM(dbj::is_range_v<range_container<int, 3>>);
	}


DBJ_TEST_SPACE_CLOSE(dbj_traits_testing)

#endif