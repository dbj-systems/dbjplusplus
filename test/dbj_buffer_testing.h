#pragma once

#define DBJ_LIGHT_BUFFER_TESTING
#ifdef DBJ_LIGHT_BUFFER_TESTING

#include "../console/dbj_console_ops.h"


DBJ_TEST_SPACE_OPEN(dbj_buffer)

using namespace ::dbj::buf;

auto alphabet = [&](char_buffer::reference_type cbr ) {
	char k = 65; // 'A'
	for (auto & c_ : cbr )
	{
		c_ = char(k++);
	}
};

extern "C"	inline void	my_memset(void *s, size_t n, char val_ = 0) noexcept
{
	volatile char *p = (char *)s;
	while (n--) *p++ = val_;
}

DBJ_TEST_UNIT(dbj_light_buffer)
{
	const auto bufsiz_ = BUFSIZ;
	char_buffer cb1(BUFSIZ);

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

	auto sizeshow = [&](char_buffer::reference_type cbr) noexcept -> void
	{
		auto show = [](auto && obj_, auto filler ) {
			filler(obj_);
			dbj::console::prinf("\ntype: %s\n\tsize of type: %zu\n\tsize of instance: %zu",
				typeid(obj_).name(), sizeof(decltype(obj_)), sizeof(obj_)
			);
		};

		show(std::vector<char>(BUFSIZ), [](auto & obj_) { std::fill(obj_.begin(), obj_.end(), '\0'); });
		show( std::string(BUFSIZ,'\0'), [](auto & obj_) { std::fill(obj_.begin(), obj_.end(), '\0'); });
		show(cbr, [](auto & obj_) { obj_.fill(); });
	};

	DBJ_TEST_ATOM(cb1.address());

	alphabet(cb1);	sizeshow(cb1);
	
	// show the copying
	{
		char_buffer cb2(BUFSIZ);
		DBJ_ATOM_TEST(copy_to(cb1, cb2)); // error_code display
		DBJ_ATOM_TEST(cb2); //rezult
		DBJ_TEST_ATOM(cb1 = cb2); // assignment
	}
// tranformations to string, wstring and vector
	{
		cb1.fill('X'); // assignment
		DBJ_ATOM_TEST(cb1);
		DBJ_ATOM_TEST(to_string(cb1));
		DBJ_ATOM_TEST(to_wstring(cb1));
		DBJ_ATOM_TEST(to_vector(cb1));
	}

	{
		auto[err_code, wsize, smart_wide_charr] = wide_copy(cb1);
		DBJ_ATOM_TEST(err_code);
		DBJ_ATOM_TEST(wsize);
		DBJ_ATOM_TEST(smart_wide_charr);
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
	print("\nWill allocate and measure three types of buffers. Buffer size will be ",
		buffer_size, " chars each\n\tEach allocation/deallocation will happen 1000 times");

	print("\n\nMeasuring unique_ptr<char[]> ");
	print("\n\nunique_ptr<char[]> = ", measure(naked_unique_ptr, buffer_size), " ms.\n");
	print("\n\nMeasuring dbj char buffer ");
	print("\n\ndbj char buffer = ", measure(dbj_buffer, buffer_size), " ms.\n");
	print("\n\nMeasuring std::vector ");
	print("\n\nstd::vector = ", measure(vector_buffer, buffer_size), " ms.\n\n");

	system("@pause");
	system("@echo.");
}

DBJ_TEST_SPACE_CLOSE

#endif // DBJ_LIGHT_BUFFER_TESTING

