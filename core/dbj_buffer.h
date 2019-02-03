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

		// plenty of space -- 65535
		// in posix terms BUFSIZ * 2 * 64 aka 64KB
		constexpr inline std::uintmax_t max_length = UINT16_MAX;
		/*
		template<
			typename C,
			std::enable_if_t<::dbj::is_char_v<C> || ::dbj::is_wchar_v<C>, int> = 0
		>
		using smarty = std::unique_ptr<C[]>;
		*/
		template<typename C > struct smartarr final
		{
			static_assert(
				::dbj::is_char_v< C > || ::dbj::is_wchar_v< C >,
				"\n\njust char or wchar_t please, also: non ref, non const and not unsigned\n"
				);
			using value_type = C;
			using type = std::unique_ptr<C[]>;
			using ref_type = std::reference_wrapper< type >;
			using carr = smartarr<char>;
			using warr = smartarr<wchar_t>;
		};

		template< typename C >
		using smarty = typename smartarr< ::dbj::tt::remove_cvref_t<C> >::type;

		template< typename C >
		using smarty_ref = typename smartarr< ::dbj::tt::remove_cvref_t<C> >::ref_type;

		using smart_carr = smartarr<char>::type;
		using smart_warr = smartarr<wchar_t>::type;

		// use it for passing (to functions) as arguments
		// since narrow pointer can not be copied
		using smart_carr_ref = smartarr<char>::ref_type;
		using smart_warr_ref = smartarr<wchar_t>::ref_type;

		// always use this function to make fresh smarty-es!
		template<typename C>
		[[nodiscard]] inline auto smart(size_t S_) noexcept
			-> typename smartarr<C>::type
		{
			DBJ_VERIFY(S_ < dbj::buf::max_length);
			return std::make_unique<C[]>(S_ + 1);
		}

		template<typename C>
		[[nodiscard]] inline auto len(smarty_ref<C> buf_ref_)
			noexcept -> size_t
		{
			return std::strlen((buf_ref_.get()).get());
		}

		template<typename C, size_t N>
		[[nodiscard]] inline auto smart(const C(&charr)[N])
			noexcept -> smarty<C>
		{
			_ASSERTE(N > 0);
			smarty<C> sp_ = smart<C>(N);
			void * rez_ = ::memcpy(sp_.get(), charr, N);
			_ASSERTE(rez_);
			return sp_; // the move
		}

		/* make smarty<C> from basic_string_view<C> */
		template<typename C>
		[[nodiscard]] inline auto	smart
		(std::basic_string_view<C> sv_) noexcept
			-> smarty<C>
		{
			_ASSERTE(!sv_.empty());
			static_assert(
				::dbj::is_char_v< C > || ::dbj::is_wchar_v< C >,
				"\n\n" __FUNCSIG__ "\njust char or wchar_t please\n"
				);
			smarty<C> sp_;
			// is this naive?
			// should I use strnlen() ?
			const size_t N = sv_.size();
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
			smart ( smarty_ref<C> buf_ref_ ) noexcept -> smarty<C>
		{
			smarty<C> & from_ = buf_ref_ ;
			_ASSERTE(from_);
			const size_t N = ::dbj::buf::len<C>(buf_ref_);
			_ASSERTE( N > 1 );
			smarty<C> sp_ = ::dbj::buf::smart<C>(N);
			_ASSERTE(sp_);
			void * rez_ = ::memcpy(sp_.get(), buf_ref_.get().get(), N);
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
			sp_.release();
			sp_ = smart<C>(N);
			_ASSERTE(sp_);
			void * rez_ = ::memcpy(sp_.get(), arr, N);
			_ASSERTE(rez_);
			return sp_;
		}

		// do not try this at home. ever.
		extern "C"	inline void	secure_reset(void *s, size_t n) noexcept
		{
			volatile char *p = (char *)s;
			while (n--) *p++ = 0;
		}

		// dangerously low, bit it works
		template<typename C>
		inline void	
			fill(smarty_ref<C> bref_, size_t N = 0,C val_ = C{}) noexcept
		{
			smarty<C> & buff_ = bref_;
			if (!N) N = ::dbj::buf::len<C>(bref_); 
			void *p = (void *)buff_.get();
			if constexpr (::dbj::is_char_v<C>)
			{
				::std::memset(p, val_, N);
			}
			else {
				::std::wmemset(p, val_, N);
			}
		}
/*
Above is for fast-and-dangerous code and bellow is a comfortable runtime char buffer. 
Slower but still approx 2x to 3x faster vs vector<char>
I have the code for measuring the difference, 
in case you are hotly against not using vector<char>.
Generaly the smaller the buffer the faster it is vs vector<char>.
For *normal* buffer sizes (posix BUFSIZ or multiplies of it)
using this type has a lot in favour.
Also this class works in code where no exceptions are used
please do let me know if problems there
*/
		struct char_buffer final {

			using type = char_buffer;
			using pointer = smarty<char>;
			using value_type = char;
			using iterator = value_type * ;
			using reference_type = type &;

			explicit char_buffer(size_t size) noexcept {	this->reset(size); }

			char_buffer(const char_buffer & another_) noexcept 
			{
				this->reset(another_.size_);
				(void)::memcpy(data(), another_.data(), this->size());
			}
			char_buffer & operator =(const char_buffer & another_) noexcept 
			{
				if (&another_ != this) {
					this->reset(another_.size_);
					(void)::memcpy(data(), another_.data(), this->size());
				}
				return *this;
			}

			template<typename T, size_t N>
			char_buffer(T(&charr)[N]) noexcept
			{
				static_assert(dbj::is_char_v< std::remove_cv_t<T> >,
					"\n\n" __FUNCSIG__ "\nonly char's please!\n");

				this->size_ = N;
				this->data_ = smart<char>(charr);
			}

			void reset(size_t new_size_) const {
				_ASSERTE(new_size_ > 0);
				_ASSERTE(new_size_ < ::dbj::buf::max_length);
				data_.release();
				this->size_ = new_size_;
				this->data_ = smart<char>(new_size_);
			}

			char & operator [] (size_t idx_)
			{
				if (idx_ > size())
					throw std::make_error_code(std::errc::invalid_argument);
				if (idx_ < ::dbj::buf::max_length)
					throw std::make_error_code(std::errc::invalid_argument);
				return data_[idx_]; //  std::unique_ptr<T[]> has this operator
			}

			iterator data() const noexcept { return data_.get(); }

			size_t const & size() const noexcept { return size_; }
			DWORD const & dword() const noexcept { return static_cast<DWORD>(size_); }

			value_type ** const address() const noexcept {
				auto p = std::addressof(data_[0]);
				return std::addressof(p);
			}
#pragma region std lib conformance

			iterator begin() noexcept { return data_.get(); }
			iterator end()   noexcept { return data_.get() + size_; }
			const iterator begin() const noexcept { return data_.get(); }
			const iterator end()   const noexcept { return data_.get() + size_; }

			void fill(char val_ = char{}) const noexcept
			{
				::dbj::buf::fill(
					std::reference_wrapper<pointer>(data_),
					size(),
					val_
				);
			}
#pragma endregion 

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

#ifdef DBJ_CONSOLE_SYSTEM
// obviously this here will not work 
// if ::dbj::console is not included 
// *before*
			void out(
				const type & cb_
			)
			{
				using namespace ::dbj::console;
				prinf(L"{ size: %d, data: '%s' }", cb_.size(), cb_.data());
			}
#endif

			friend std::string to_string(const reference_type from_) noexcept {
				// this will not copy the whole buffer
				return { from_.data() };
			}

			friend std::wstring to_wstring(const reference_type from_) noexcept {
				// this will not copy the whole buffer
				std::string str_(from_.data());
				// cast to wide ... but locale free!
				return { str_.begin(), str_.end() };
			}

			friend std::vector<char> to_vector(const reference_type from_) noexcept {
				// this will not copy the whole buffer
				std::string str_(from_.data());
				return { str_.begin(), str_.end() };
			}

			/*
			copy this one to the buffer of the same size
			return error_code(std::errc::invalid_argument)
			on different sizes
			return OK error_code on no error
			*/
			[[nodiscard]] friend
				std::error_code copy_to(const reference_type from_, reference_type target_)
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
			struct wide_copy_result {
				std::error_code ec{};
				size_t size{};
				smart_warr pointer{};
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

				smart_warr wp = ::dbj::buf::smart<wchar_t>(source_size_);

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
