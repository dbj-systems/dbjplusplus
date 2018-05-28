#pragma once
#include <string>

namespace dbj::unused {

	using namespace std;

	inline string  filename(const string  &  file_path) {
		auto pos = file_path.find_last_of('\\');
		return
			(string::npos != pos
				? file_path.substr(pos, file_path.size())
				: file_path
				);
	}
	inline string fileline(const string & file_path, unsigned line_, const string & suffix = 0) {
		return filename(file_path) + "(" + to_string(line_) + ")"
			+ (suffix.empty() ? "" : suffix);
	}

	/*
	for win32 functions that are leaving result in the buffer
	and returning 0 aka NULL if they go wrong
	*/
	namespace {
		constexpr auto DBJ_BUFSIZ = 1024u;
		using DBJ_BUFREF = const char(&)[DBJ_BUFSIZ];
		constexpr DBJ_BUFREF DBJ_BUF{'\x0'};
		constexpr std::string && infoBuf{ BUF };

		void clear(DBJ_BUFREF buffy) {
			memset((void*)buffy, '\x0', DBJ_BUFSIZ);
		}
	}

	template<class F, class... Pack>
	constexpr __forceinline 
		DBJ_BUFREF	call_with_buff
	(F&& fun, Pack&&... args) {
		clear(DBJ_BUF);
			if (0 == std::invoke(fun, (args)...))
				throw dbj::Exception(typeid(F).name());
		return (DBJ_BUF);
	}

} // dbj::unused
