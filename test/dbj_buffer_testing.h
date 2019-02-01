#pragma once

#define DBJ_LIGHT_BUFFER_TESTING
#ifdef DBJ_LIGHT_BUFFER_TESTING

DBJ_TEST_SPACE_OPEN(dbj_buffer)

using namespace ::dbj::buf;

DBJ_TEST_UNIT(dbj_light_buffer)
{
	const auto bufsiz_ = BUFSIZ;
	char_buffer cb1(BUFSIZ), cb2(BUFSIZ);

	// test the heap (de)allocation
	// no can do --> auto p = new char_buff_type(BUFSIZ);
	// no can do --> auto p = new char_buff_type(255)[BUFSIZ];
	// global new and delete work as before o
	{
		auto p = new char[BUFSIZ];
		delete[] p;
	}
	// can we cheat with placement new?
	// no we can not
	//	auto p = new char_buff_type(BUFSIZ) char_buff_type;

	auto sizeshow = [&](char_buffer::reference_type cbr)
	{

		std::vector<char> v26{}; v26.resize(bufsiz_);
		std::string       s26{}; s26.resize(bufsiz_);
		std::array<char, 26> a26{ {0} };

		auto retval = DBJ_ATOM_TEST(cbr.size());
		DBJ_ATOM_TEST(sizeof(a26));
		DBJ_ATOM_TEST(sizeof(v26));
		DBJ_ATOM_TEST(sizeof(s26));
		return retval;
	};

	// auto j = cb.size();
	auto a1 = cb1.address();

	auto alphabet = [&](char_buffer & cb) {
		char k = 65; // 'A'
		for (auto & c_ : cb)
		{
			c_ = char(k++);
		}
	};

	alphabet(cb1);
	DBJ_TEST_ATOM(sizeshow(cb1));

	auto ec_ = copy(cb1, cb2);

	if (ec_)
		DBJ_ATOM_TEST(ec_); // error
	else
		DBJ_ATOM_TEST(cb2.data()); // no error

// tranformations to string, wstring and vector
	{
		DBJ_TEST_ATOM(to_string(cb2));
		DBJ_TEST_ATOM(to_wstring(cb2));
		DBJ_TEST_ATOM(to_vector(cb2));
	}

	/* std::unique_ptr<wchar_t[]> */
	{
		auto[err_code, wsize, wbuf] = wide_copy(cb2);
		DBJ_ATOM_TEST(cb2.size());
		DBJ_ATOM_TEST(wsize);
		// 
		// show what is the real wbuf type
		typename char_buffer::wide_pointer & wide_buffer = wbuf;
		if (!err_code)
			DBJ_ATOM_TEST(wide_buffer.get());
		else
			::dbj::console::print("\nerror code returned: ", err_code);
	}
}

namespace inner {

	//deliberately not constexpr
	inline auto const & buffer_size = BUFSIZ;

	inline void naked_unique_ptr(size_t count_) {
		auto alloc_return = [&]() {
			return std::make_unique<char[]>(count_);
		};
		auto dumsy = alloc_return();
		dumsy.get()[count_ - 1] = '?';
	}

	inline void dbj_buffer(size_t count_) {
		auto alloc_return = [&]() {
			return char_buffer(count_);
		};
		auto dumsy[[maybe_unused]] = alloc_return();
		dumsy[count_ - 1] = '?';
	}

	inline auto vector_buffer(size_t count_) {
		auto alloc_return = [&]() {
			return std::vector<unsigned char>(count_);
		};
		auto dumsy[[maybe_unused]] = alloc_return();
		dumsy[count_ - 1] = '?';
	}

	inline auto measure(
		void(*fp)(size_t),
		size_t buffer_count,
		size_t max_iteration = 1000)
	{
		auto start_ = std::chrono::system_clock::now();
		for (long i = 0; i < max_iteration; i++) {
			fp(buffer_count);
		}
		auto end_ = std::chrono::system_clock::now();
		return (end_ - start_).count() / 1000.0;
	}
} // inner

DBJ_TEST_UNIT(dbj_light_buffer_measure) {
	/*
	on this machine I have found, for normal buffer sizes
	std::vector<char> is considerably slower
	for very large buffers it is equaly slow as the two others
	*/
	using ::dbj::console::print;
	using namespace inner;
	system("@echo.");
	system("@echo.");
	print("\n will allocate and measure three types of buffers. Buffer size will be ",
		buffer_size, " chars each\n\tEach allocation/deallocation will happen 1000 times");

	print("\n\nMeasuring unique_ptr<char[]> ");
	print("\n\nunique_ptr<char[]> = ", measure(naked_unique_ptr, buffer_size), " ms.\n");
	print("\n\nMeasuring dbj char buffer ");
	print("\n\ndbj char buffer = ", measure(dbj_buffer, buffer_size), " ms.\n");
	print("\n\nMeasuring std::vector ");
	print("\n\nstd::vector = ", measure(vector_buffer, buffer_size), " ms.\n\n");

	system("@echo.");
	system("@echo.");
	system("@pause");
	system("@echo.");
	system("@echo.");
}

DBJ_TEST_SPACE_CLOSE

#endif // DBJ_LIGHT_BUFFER_TESTING

