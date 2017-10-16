#pragma once
namespace dbj {
	namespace fun {
		/*
		http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
		*/
		template <class T>
		class reference_wrapper {
		public:
			// types
			typedef T type;

			// construct/copy/destroy
			reference_wrapper(T& ref) noexcept : _ptr(std::addressof(ref)) {}
			reference_wrapper(T&&) = delete;
			reference_wrapper(const reference_wrapper&) noexcept = default;

			// assignment
			reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

			// access
			operator T& () const noexcept { return *_ptr; }
			T& get() const noexcept { return *_ptr; }

			template< class... ArgTypes >
			std::invoke_result_t<T&, ArgTypes...>
				operator() (ArgTypes&&... args) const {
				return std::invoke(get(), std::forward<ArgTypes>(args)...);
			}

		private:
			T* _ptr;
		};

#ifndef _MSC_VER 
		// C++17 deduction guides
		template<class T>
		reference_wrapper(reference_wrapper<T>)->reference_wrapper<T>;
#endif
	} // namespace fun
} // namespace dbj
#define DBJVERSION __DATE__ __TIME__
  // #pragma message("-------------------------------------------------------------")
#pragma message( "============> Compiled: " __FILE__ ", Version: " DBJVERSION)
  // #pragma message("-------------------------------------------------------------")
#pragma comment( user, "(c) 2017 by dbj@dbj.org | Version: " DBJVERSION ) 
#undef DBJVERSION
