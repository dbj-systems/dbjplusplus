#pragma once
// 2019-01-30	DBJ	Created
// char buffer type
// faster and lighter vs the usual suspects: string and vector
// look into the friend's region for a lot of utilities

// note: this is part of the core , thus we will try and absolutely minimise
// the dependancies on other parts of dbj++, and thus we will sometimes
// reimplement things here which perhaps exist in other part of the dbj++ 

#define  DBJ_LIGHT_BUFFER
#ifdef DBJ_LIGHT_BUFFER

namespace dbj{ 
	namespace buf {
	/*
	This is a runtime buffer. Just like std::vector, but better because it
	is lighter and faster.
	I knew it is a good practice to have and use one's own char buffer class
	instead of using std::vector<char>
	Here are the measurements too, in case you are hotly against.
	Generaly the smaller the buffer the faster this is.
	For normal buffer sizes (posix BUFSIZ or multiplies of it)
	using this has a lot of in favour.
	Also this class works in code where no exceptions are used
	please do let me know if problems there
	*/
	struct char_buffer final {

		using type = char_buffer;
		using value_type = char;
		using iterator = value_type * ;
		using pointer = std::unique_ptr<value_type[]>;
		/*
		one can "copy" the unique_ptr by moving, example:
		char_buffer<char> buf2 = std::move(buf1) ;
		this leaves the buf1 "destroyed" ... no good this is
		so how to pass them to functions?
		instead, for passing instances of this class as function arg's
		use this reference_type

		extern void whatever ( char_buffer<char>::reference_type br ) ;
		char_buffer<char> bf(BUFSIZ) ;
		// normal and simple calling
		whatever(bf);
		*/
		using reference_type = std::reference_wrapper<char_buffer>;

		explicit char_buffer(size_t size)
			// reference counted pointer to auto-delete the buffer
			: data_(std::make_unique<value_type[]>(size + 1)), size_(size)
		{
		}

		// result of this operator is also
		// to change the single char
		char & operator [] (size_t idx_)
		{
			if (idx_ > size())
				throw std::make_error_code(std::errc::invalid_argument);
			return data_.get()[idx_];
		}

		iterator data() const { return data_.get(); }

		size_t const & size() const { return size_; }

		void reset() const {
			data_.reset('\0');
			data_ = std::make_unique<char[]>(size_ + 1);
		}

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
		mutable	pointer data_{};
	#pragma region char_buffer friends

		friend std::string to_string(const reference_type from_) noexcept {
			return { from_.get().data() };
		}

		friend std::wstring to_wstring(const reference_type from_) noexcept {
			return { from_.get().begin(), from_.get().end() };
		}

		friend std::vector<char> to_vector(const reference_type from_) noexcept {
			return { from_.get().begin(), from_.get().end() };
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
			auto const & frm_ = from_.get();
			auto & to_ = target_.get();

			if (frm_.size() != to_.size())
				return std::make_error_code(std::errc::invalid_argument);

			std::copy(frm_.data_.get(), frm_.data_.get() + frm_.size(),
				to_.data_.get());

			return std::error_code{};
		}
	#ifdef _WIN32
	public:
		using wide_pointer = std::unique_ptr<wchar_t[]>;
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
		friend wide_copy_result wide_copy(reference_type cb) noexcept
		{
			auto const & source_ = cb.get();
			auto const & source_size_ = source_.size_;
			auto & source_pointer_ = source_.data_;

			wide_pointer wp = std::make_unique<wchar_t[]>(source_size_ + 1);
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
	#endif 
	#pragma endregion // char_buffer friend utilities
	};

	} // buf
} // dbj

#endif // DBJ_LIGHT_BUFFER
