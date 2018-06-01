#pragma once

#ifdef DBJ_TESTING_EXISTS

namespace dbj_testing_space  {

	template<typename T, typename dbj::require_integral<T> = 0>
	inline auto Object(T&& t) { return std::variant<T>(t); }

	template<typename T, typename dbj::require_floating<T> = 0>
	inline auto Object(T&& t) { return std::variant<T>(t); }

	/*usage*/
	inline void dbj_traits_tests() {

		DBJ_TEST_ATOM(dbj::is_floating<decltype(42.0f)>());
		DBJ_TEST_ATOM(dbj::is_integral<decltype(42u)>());
		DBJ_TEST_ATOM(dbj::is_object<decltype(42u)>());

		DBJ_TEST_ATOM(Object(42));
		DBJ_TEST_ATOM(Object(42.0f));
	}


	DBJ_TEST_UNIT(": dbj traits") {
		dbj_traits_tests();
	}

} // namespace

#endif // DBJ_TESTING_EXISTS
