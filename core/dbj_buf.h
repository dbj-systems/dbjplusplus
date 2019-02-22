#pragma once

// #define DBJ_BUFFER_TESTING

#include "../dbj_gpl_license.h"
#include "dbj_insider.h"

#include <system_error>
#include <cassert>
#include <memory>
#include <string_view>
#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <cstring>

// 2019-01-30	dbj@dbj.org	created
//
// char runtime dynamic buffer type
// faster and lighter vs the usual suspects: string and vector
// note: this is part of the core , thus we will try and absolutely minimise
// the dependancies  thus we will sometimes
// reimplement things here which perhaps exist in other projects
//
// note: this is not C++17 way of doing things
// so that good legacy code can enjoy this too

namespace dbj {
	namespace buf {

		// plenty of space -- 65535
		// in posix terms BUFSIZ * 2 * 64 aka 64KB
		constexpr std::size_t max_length = UINT16_MAX;

		namespace {
			using namespace ::dbj::core::util;
			using inside_1_and_max =
				insider<size_t, 1,
				::dbj::buf::max_length
				, insider_error_code_throw_policy
				>;
		}

		template<typename CHAR> struct smart_buf final
		{
			using type = smart_buf;
			using value_type = CHAR;
			using pointer = std::unique_ptr<value_type[]>;
			using ref_type = type & ;

			// always use this function to make  
			// fresh buff_type-es!
			// array is sized & zeroed
			static pointer make(inside_1_and_max size_) noexcept {
				return std::make_unique<value_type[]>(size_ + 1);
			}

			static bool
				empty(
					typename std::unique_ptr<value_type[]> & buff_
				) noexcept
			{
				return (buff_ && (buff_[0] != '\0'));
			}

			/*
			NOTE! 
			stringlen on char array filled with 0's returns 0 (zero)
			not the allocated size of the array.
			As I am using the inside_1_and_max as a return type 
			it will 'kick the bucket' on the length 0!
			that is the point of using it
			*/
			static auto length(typename std::unique_ptr<value_type[]> & buf_ref_)
				noexcept -> inside_1_and_max
			{
				return std::strlen(buf_ref_.get());
			}

			// the base maker
			static pointer make(value_type const * first_, value_type const * last_) noexcept
			{
				assert(first_ && last_);
				size_t N = std::distance(first_, last_);
				assert(N > 0);
				pointer sp_ = make(N);
				auto * rez_ = std::copy(first_, last_, sp_.get());
				assert(rez_);
				return sp_; // the move
			}
			// here we depend on a zero terminated string
			static pointer make(value_type const * first_) noexcept
			{
				assert(first_);
				// in case someone made and sent "\0" 
				size_t N = (first_[0] == '\0' ? 1 : std::strlen(first_));
				assert(N > 0);
				return make(first_, first_ + N);
			}
			/*	from array of char's	*/
			template<size_t N>
			static pointer make(const value_type(&charr)[N]) noexcept
			{
				return make(charr, charr + N);
			}
			/* from string_view */
			static  pointer	make(std::basic_string_view< value_type > sv_) noexcept
			{
				return make(sv_.data(), sv_.data() + sv_.size());
			}
			/* from string  */
			static  pointer	make(std::basic_string< value_type > sv_ )  noexcept
			{
				return make(sv_.data(), sv_.data() + sv_.size());
			}
			/* from vector  */
			static  pointer	make(std::vector< value_type > sv_ )  noexcept
			{
				return make(sv_.data(), sv_.data() + sv_.size());
			}
			/* from array  */
			template<size_t N>
			static  pointer	make(std::array< value_type, N > sv_ )  noexcept
			{
				return make(sv_.data(), sv_.data() + sv_.size());
			}

			/* from An Other */
			static pointer	make(
				typename std::unique_ptr<value_type[]> & another_) noexcept
			{
				assert(another_);
				return make(another_.get());
			}

			/*
			NOTE! 
			stringlen on char array filled with 0's returns 0!
			not the allocated size of the array.
			if you do not send the size and if it is found to be 0
			and no fill will be done.
			*/
			static  std::unique_ptr<value_type[]> & 
				fill(
				typename std::unique_ptr<value_type[]> & buff_, 
				value_type val_,
				size_t N = 0
				) noexcept
			{
				if ( ! empty(buff_) )
				{
					if (!N) N = length(buff_);
					::std::fill(buff_.get(), buff_.get() + N, val_);
				}
				return buff_;
			}
		};

		using buff_type = typename smart_buf<char>::type;
		using buff_pointer = typename smart_buf<char>::pointer;
		using buff_ref_type = typename smart_buf<char>::ref_type;

		using wbuff_type = typename smart_buf<wchar_t>::type;
		using wbuff_pointer = typename smart_buf<wchar_t>::pointer;
		using wbuff_ref_type = typename smart_buf<wchar_t>::ref_type;

		/*
		assign array to instance of unique_ptr<T[]>
		note: "any" type will do as long as std::unique_ptr
		will accept it
		*/
		template<typename C, size_t N>
		inline auto	assign(std::unique_ptr<C[]> & sp_, const C(&arr)[N]) noexcept -> std::unique_ptr<C[]> &
		{
			// sp_.release();
			sp_ = std::make_unique<C[]>(N + 1);
			assert(sp_);
			void * rez_ = ::memcpy(sp_.get(), arr, N);
			assert(rez_);
			return sp_;
		}

		// do not try this at home. ever.
		extern "C"	inline void	secure_reset(void *s, size_t n) noexcept
		{
			volatile char *p = (char *)s;
			while (n--) *p++ = 0;
		}


		/*----------------------------------------------------------------------------
		binary comparisons
		*/

		template<typename CHAR, typename BT = smart_buf<CHAR>  >
		inline bool operator == (
			const typename BT::ref_type left_, 
			const typename BT::ref_type right_
			)
			noexcept
		{
			const size_t left_size = BT::length(left_);
			const size_t right_size = BT::length(right_);

			if (left_size != right_size)
				return false;

			return std::equal(
				left_.get(), left_.get() + left_size,
				right_.get(), right_.get() + right_size
			);
		}
		/*----------------------------------------------------------------------------
		streaming
		
		using buff_type = typename smart_buf<char>::type;
		using buff_pointer = typename smart_buf<char>::pointer;
		using buff_ref_type = typename smart_buf<char>::ref_type;

		using wbuff_type = typename smart_buf<wchar_t>::type;
		using wbuff_pointer = typename smart_buf<wchar_t>::pointer;
		using wbuff_ref_type = typename smart_buf<wchar_t>::ref_type;

		*/
		inline std::ostream & operator << (std::ostream & os, 
			std::unique_ptr<char[]> & the_buffer_)
		{
			return os << the_buffer_.get();
		}

		inline std::wostream & operator << (std::wostream & os, 
			std::unique_ptr<wchar_t[]> &  the_buffer_)
		{
			return os << the_buffer_.get();
		}

	} // buf
} // dbj

#ifdef DBJ_BUFFER_TESTING
#define TU(x) std::wcout << std::boolalpha << L"\nExpression: '" << (#x) << L"'\n\tResult: '" << (x) << L"'\n"
#include <iostream>

namespace {
	inline bool dbj_testing_dbj_unique_ptr_buffer()
	{

		auto test_1 = [] (auto C, auto specimen) 
		{
			using namespace ::dbj::buf;

			using T = decltype( std::decay_t< C > );
			TU(smart_buf<T>::make(BUFSIZ));
			TU(smart_buf<T>::make(specimen));
			TU(smart_buf<T>::make(std::basic_string<T>(specimen)));
			TU(smart_buf<T>::make(std::basic_string_view<T>(specimen)));

			auto buf = smart_buf<T>::make(BUFSIZ);
			TU(smart_buf<T>::fill(buf, C));

			auto sec = smart_buf<T>::make(buf);
			TU( sec == buf);

		};
			test_1('*', "narrow string");
			test_1(L'*', L"wide string");
		return true;
	} // testing_dbj_buffer

	static auto dbj_testing_dbj_buffer_result
		= dbj_testing_dbj_unique_ptr_buffer();
}

#endif // DBJ_BUFFER_TESTING
#undef TU
#undef DBJ_BUFFER_TESTING
