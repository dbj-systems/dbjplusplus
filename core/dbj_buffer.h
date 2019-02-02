#pragma once
// 2019-01-30	DBJ	Created
// char buffer type
// faster and lighter vs the usual suspects: string and vector
// look into the friend's region for a lot of utilities

// note: this is part of the core , thus we will try and absolutely minimise
// the dependancies on other parts of dbj++, and thus we will sometimes
// reimplement things here which perhaps exist in other part of the dbj++ 

#ifndef DBJ_LIGHT_BUFFER
#define  DBJ_LIGHT_BUFFER

namespace dbj {
	namespace buf {

		template<typename C>
		using smarty = std::unique_ptr<C[]>;

		using smart_carr  = smarty<char> ;
		using smart_warr = smarty<wchar_t>;

		// use it for passing (to functions) as arguments
		// since narrow pointer can not be copied
		using smart_arr_ref = std::reference_wrapper<smart_carr>;
		using smart_warr_ref = std::reference_wrapper<smart_warr>;

		// always use this function to make fresh smarty-es!
		template<typename C>
		[[nodiscard]] inline auto smart(size_t S_) noexcept
			-> smarty<C>
		{
			static_assert(
				::dbj::is_char_v< C > || ::dbj::is_wchar_v< C >,
				"\n\n" __FUNCSIG__ "\njust char or wchar_t please, also non ref, non const and not unsigned\n"
				);
			return std::make_unique<C[]>(S_ + 1);
		}

		template<typename C, size_t N>
		[[nodiscard]] inline auto smart(const C(&charr)[N]) noexcept
			-> smarty<C>
		{
			smarty<C> sp_ = smart<C>(N);
			void * rez_ = ::memcpy(sp_.get(), charr, N);
			_ASSERTE(rez_);
			return sp_; // the move
		}

		/* make smarty<C> from basic_string_view<C> */
		template<typename C>
		[[nodiscard]] inline auto
			smart
			(
				std::basic_string_view<C> sv_
			) noexcept
			-> smarty<C> 
		{
			smarty<C> sp_;
			// is this naive?
			// should I use strnlen() ?
			const size_t N = sv_.size();
			static_assert(std::is_trivially_copyable_v<C>);
			sp_.release();
			sp_ = smart<C>(N);
			void * rez_ = ::memcpy(sp_.get(), sv_.data(), N);
			_ASSERTE(rez_);
			return sp_;
		}

		/* 
		make smarty<C> from smarty<C> 
		this is clever since we do not pass C*
		but... can we rely on the the caller 
		making a properly zero terminated string in there
		so that strlen will work?
		*/
		template<typename C>
		[[nodiscard]] inline auto
			smart
			(
				smarty<C> const & sv_
			) noexcept
			-> smarty<C> 
		{
			smarty<C> sp_;
			// is this naive? should I use strnlen() ?
			const size_t N =  ::strlen( sv_.get() ) ;
			static_assert(std::is_trivially_copyable_v<C>);
			sp_.release();
			sp_ = smart<C>(N);
			void * rez_ = ::memcpy(sp_.get(), sv_.get(), N);
			_ASSERTE(rez_);
			return sp_;
		}

		/*
		assign array to instance of unique_ptr<T[]>
		*/
		template<typename C, size_t N>
		inline auto
			assign
			(
				smarty<C> & sp_,
				const C(&arr)[N]
			) noexcept
			-> smarty<C> &
		{
			static_assert(std::is_trivially_copyable_v<C>);
			sp_.release();
			sp_ = smart<C>(N);
			void * rez_ = ::memcpy(sp_.get(), arr, N);
			_ASSERTE(rez_);
			return sp_;
		}

		/*
	This is a runtime buffer. Just like std::vector<char>, but lighter and faster.
	I knew it is a good practice to have and use one's own char buffer class
	instead of using std::vector<char>
	Bellow are the measurements too, in case you are hotly against.
	Generaly the smaller the buffer the faster this is.
	For normal buffer sizes (posix BUFSIZ or multiplies of it)
	using this type has a lot  in favour.
	Also this class works in code where no exceptions are used
	please do let me know if problems there
	*/
		struct char_buffer final {

			using type = char_buffer;
			using value_type = char;
			using iterator = value_type * ;
			using pointer = smart_carr ;
			/*
			one can "copy" the unique_ptr by moving, example:
			char_buffer<char> buf2 = std::move(buf1) ;
			this leaves the buf1 "destroyed" ... no good this is
			so how to pass them to functions?
			instead, for passing instances of this class as function arg's
			use the reference_type. example:
			extern void whatever ( char_buffer<char>::reference_type br ) ;
			char_buffer<char> bf(BUFSIZ) ;
			and then, normal and simple calling. example:
			whatever(bf);
			*/
			using reference_type = typename type &;

			explicit char_buffer(size_t size)
			{
				this->reset(size);
			}

			char_buffer(const char_buffer & another_)
			{
				this->reset(another_.size_);
				(void)::memcpy(data(), another_.data(), this->size());
			}
			char_buffer & operator =(const char_buffer & another_)
			{
				this->reset(another_.size_);
				(void)::memcpy(data(), another_.data(), this->size());
			}

			template<typename T, size_t N>
			char_buffer(T(&charr)[N])
			{
				static_assert(dbj::is_char_v< std::remove_cv_t<T> >,
					"\n\n" __FUNCSIG__ "\nonly char's please!\n");

				this->size_ = N;
				this->data_ = smart<char>(charr);
			}

			void reset(size_t new_size_) const {
				_ASSERTE(new_size_ > 0);
				data_.release();
				this->size_ = new_size_;
				this->data_ = smart<char>(new_size_);
			}

			char & operator [] (size_t idx_)
			{
				if (idx_ > size())
					throw std::make_error_code(std::errc::invalid_argument);
				return data_.get()[idx_];
			}

			iterator data() const { return data_.get(); }

			size_t const & size() const { return size_; }
			DWORD const & dword() const { return static_cast<DWORD>(size_); }

			value_type ** const address() const noexcept {
				auto p = std::addressof(data_.get()[0]);
				return std::addressof(p);
			}

			iterator begin() { return data_.get(); }
			iterator end() { return data_.get() + size_; }

			// to avoid "never releasing smart pointer" syndrome
			// we wil ban creation of this class on the heap
			void* operator new(std::size_t sz) = delete;
			void* operator new[](std::size_t sz) = delete;
			void  operator delete(void* ptr, std::size_t sz) = delete;
			void  operator delete[](void* ptr, std::size_t sz) = delete;
		private:
			mutable size_t  size_;
			mutable	pointer data_{}; // size == 0
#pragma region char_buffer friends

// obviously this here will not work 
// since ::dbj::console is not included 
// before
#ifdef DBJ_CONSOLE_SYTEM
			void out(
				::dbj::buf::char_buffer const & cb_
			)
			{
				using namespace ::dbj::console;
				prinf(L"%S", cb_.data());
			}
#endif

			friend std::string to_string(const reference_type from_) noexcept {
				return { from_.data() };
			}

			friend std::wstring to_wstring(const reference_type from_) noexcept {
				return { from_.begin(), from_.end() };
			}

			friend std::vector<char> to_vector(const reference_type from_) noexcept {
				return { from_.begin(), from_.end() };
			}

			/*
			copy this one to the buffer of the same size
			return error_code(std::errc::invalid_argument)
			on different sizes
			return OK error_code on no error
			*/
			[[nodiscard]] friend
				std::error_code copy(const reference_type from_, reference_type target_)
				noexcept /* beware: std::copy may throw bad_alloc! */
			{
				if (from_.size() != target_.size())
					return std::make_error_code(std::errc::invalid_argument);

				std::copy(from_.data_.get(), from_.data_.get() + from_.size(),
					target_.data_.get());

				return std::error_code{}; // OK
			}
#ifdef _WIN32
		public:
			using wide_pointer = smart_warr;
			/* in order not to loose the size info we will return this structure */
			struct wide_copy_result {
				std::error_code ec{};
				size_t size{};
				wide_pointer pointer{};
			};
		private:
			/*
			for the inevitable WIN32 aficionados
			we will deliver the wide copy of internal array,
			transformed and inside the unique_ptr inside the wide_copy_result
			as ever we do not use exceptions
			*/
			friend wide_copy_result wide_copy(reference_type source_) noexcept
			{
				auto const & source_size_ = source_.size_;
				auto & source_pointer_ = source_.data_;

				wide_pointer wp = ::dbj::buf::smart<wchar_t>(source_size_);

				size_t rezult_size;
				auto mbstowcs_errno = ::mbstowcs_s(
					&rezult_size,
					wp.get(), source_size_ + 1,
					source_pointer_.get(), source_size_
				);
				std::error_code ec{}; if (0 != mbstowcs_errno) {
					ec = std::make_error_code(std::errc::invalid_argument);
				}
				return { ec, source_size_ , std::move(wp) };
			}
#endif // _WIN32
#pragma endregion // char_buffer friend utilities
		}; // char_buffer

	} // buf
} // dbj


#endif // DBJ_LIGHT_BUFFER
