#pragma once

namespace dbj {
	/*
	Why std::optional?

	Examples:

	Extremely preferable to having something like a std::unique_ptr<std::string>
	for each string class member! std::optional gives you data locality, which is great
	for performance.

	std::optional<int> try_parse_int(std::string )
	above is **much** simpler/safer/faster vs
	int * try_parse_int(std::string )
	bool try_parse_int(std::string , int & )

	Avoiding '-1' as 'special value'
	std::optional<int> find_in_string(std::string , std::string );

	'optional_handle' is better than "naked" optional<T>
	it preserves the inner type and using value of 
	it also makes operator<< much simpler to implement

	(c) 2018 DEC by dbj@dbj.org
	*/
	template<typename T>
	struct optional_handle final
	{
		// here we enforce type requirements for T
		// we specificaly bar the pointers handling
		// smart pointers are made for that
		static_assert(
			!is_pointer<T>::value,
			__FILE__ " optional_handle -- T can not be a pointer"
			);
		// somewhat controversial requirement ;)
		// https://foonathan.net/blog/2018/07/12/optional-reference.html
		static_assert(
			!is_reference_v <T>,
			__FILE__ " optional_handle -- T can not be a reference"
			);
		static_assert(
			is_copy_constructible_v <T>,
			__FILE__ " optional_handle -- T has to be copy  constructible"
			);

		using	type = optional_handle;
		using	value_type = T ;
		mutable optional<T>	val_{};
		// using   inner_type = T;

		// notice the nullopt usage
		// it signals the 'uninitialized state'
		// of optional
		optional_handle() noexcept : val_(nullopt) {}

		// allowed conversion constructors
		optional_handle( T const & opt_) noexcept : val_(opt_) {}
		optional_handle(T && opt_) noexcept : val_(move(opt_)) {}
		// explicit optional_handle(const T & opt_) noexcept : val_(opt_) {}

		bool has_value() const noexcept { return val_.has_value(); }

		// set the value through the reference
		operator T & () {
			if (val_.has_value())
				return val_.value();
			throw  dbj::exception("optional_handle not initialized");
		}

		// get the copy of the value
		operator T () const {
			if (val_.has_value())
				return val_.value();
			throw  dbj::exception("optional_handle not initialized");
		}

		// for containers mebership we need this operator
		friend bool operator < (type left_, type right_) {
			return (left_.val_ < right_.val_);
		}

		friend bool operator == (type left_, type right_) {
			return (left_.val_ == right_.val_);
		}

		friend wostream & operator << (wostream & os, type k_) {
			if (k_.val_.has_value())
				return os << k_.val_.value();
			return os << L"{}";
		}

		friend ostream & operator << (ostream & os, type k_) {
			if (k_.val_.has_value())
				return os << k_.val_.value();
			return os << "{}";
		}

		friend void out(const type & opt_) {

			if (opt_.has_value())
				::dbj::console::out(opt_.val_.value());
			else
				::dbj::console::out(L"{}");
		}
	};


} // dbj

/* inclusion of this file defines the kind of a licence used */
#include "../dbj_gpl_license.h"