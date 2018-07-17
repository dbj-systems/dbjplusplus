#pragma once

#include <vector>
#include <string>

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

namespace dbj::log {

#ifdef _UNICODE

	using outstream_type = std::wostringstream; //  std::wostream;
	using stringbuf_type = std::wstringbuf;

	constexpr auto unicode = true;
	//	inline auto && COUT = std::wcout;
	constexpr wchar_t LEFT_ANGLED = L'[';
	constexpr wchar_t RGHT_ANGLED = L']';
	constexpr wchar_t SPACE = L' ';
	constexpr wchar_t COMMA = L',';
#else
#error "Just UNICODE builds please :)"
#endif

	constexpr auto bufsiz = BUFSIZ * 2;

	using vector_wstrings_type = std::vector<std::wstring>;
	using vector_strings_type = std::vector<std::string >;
}