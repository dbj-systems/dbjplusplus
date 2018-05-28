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
	for e.g. win32 functions that are 
	leaving the result in the buffer
	and returning 0 aka NULL if they go wrong
	*/
	namespace buf {
		constexpr auto DBJ_BUFSIZ = 1024u;
		using DBJ_BUFREF = const char(&)[DBJ_BUFSIZ];
		constexpr DBJ_BUFREF DBJ_BUF{'\x0'};

		void clear(DBJ_BUFREF buffy) {
			memset((void*)buffy, '\x0', DBJ_BUFSIZ);
		}
	}

	template<class F, class... Pack>
	constexpr 
	  inline 
		dbj::DBJ_BUFREF	
	    	call_with_buff
	(F&& fun, Pack&&... args) {
		buf::clear(buf::DBJ_BUF);
			if (0 == std::invoke(fun, (args)...))
				throw dbj::Exception(typeid(F).name());
		return (buf::DBJ_BUF);
	}

} // dbj::unused
