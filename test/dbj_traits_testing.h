#pragma once

#include <vector>
#include <variant>
#include <dbj_testing_interface.h>

DBJ_TEST_SPACE_OPEN(dbj_traits_testing)

/*****************************************************************************************/

template< class T >
struct remove_cvref {
	typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

template <class ARGT>
using to_base_t =
std::remove_pointer_t< std::remove_all_extents_t< remove_cvref_t < ARGT > > >;

/*****************************************************************************************/
namespace hbc {
	template <typename T, typename = void> struct base { using type = std::remove_cv_t<T>; };

	template <typename T> using base_t = typename base<T>::type;

	template <typename T> struct base<T, std::enable_if_t<std::is_array_v<T>>>
	{
		using type = base_t<std::remove_all_extents_t<T>>;
	};

	template <typename T> struct base<T, std::enable_if_t<std::is_reference_v<T>>>
	{
		using type = base_t<std::remove_reference_t<T>>;
	};

	template <typename T> struct base<T, std::enable_if_t<std::is_pointer_v<T>>>
	{
		using type = base_t<std::remove_pointer_t<T>>;
	};
} // hbc
DBJ_TEST_UNIT( compound_type_reduction )
{
	// array of pointers to string
	using arr_of_sp = std::string * (&)[42];

	// should pass
	static_assert(std::is_same_v<std::string, hbc::base_t<arr_of_sp>>);
	static_assert(std::is_same_v<std::string, to_base_t<arr_of_sp>>);

	static_assert(std::is_same_v<void(), hbc::base_t<void()>>);
	static_assert(std::is_same_v<void(), to_base_t<void()>>);

	// 
	struct X { char data{}; char method() const { return {}; } };
	static_assert(std::is_same_v<X, hbc::base_t<X(&)[]>>);
	static_assert(std::is_same_v<X, to_base_t<X(&)[]>>);

	//
	using method_t = char (X::*)();
	static_assert(std::is_same_v<method_t, hbc::base_t< method_t(&)[]>>);
	static_assert(std::is_same_v<method_t, to_base_t< method_t(&)[]>>);

}

/*****************************************************************************************/

   template<typename T, typename dbj::require_integral<T> = 0 >
	inline auto Object(T&& t) { return std::variant<T>(t); }

	template<typename T, typename dbj::require_floating<T> = 0>
	inline auto Object(T&& t) { return std::variant<T>(t); }

	/*usage*/
	DBJ_TEST_UNIT(dbj_basic_traits_tests)
	{

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

	DBJ_TEST_UNIT(containertraitstests) 
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


DBJ_TEST_SPACE_CLOSE

