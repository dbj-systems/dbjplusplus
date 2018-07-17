#pragma once

#include "dbj_crt.h"
#include "dbj_micro_log_fwd.h"
#include "dbj_app_env.h"
#include "dbj_string_util.h"

namespace dbj::log {

	using dbj::log::outstream_type; // std::wostringstream this is

	namespace internal {
		constexpr char space = ' ', prefix = '{', suffix = '}', delim = ',';

		/*
		anything that has begin and end
		NOTE: that includes references to native arrays
		*/
		inline auto print_range = []
		(outstream_type & os, const auto & range) {

			// not requiring  range.size();
			// thus can do native arrays
			std::size_t argsize =
				static_cast<std::size_t>(
					std::distance(
						std::begin(range), std::end(range)
					)
					);

			if (argsize < 1) return;

			std::size_t arg_count{ 0 };

			auto delimited_out = [&](auto && val_) {
				os << (val_);
				if ((arg_count++) < (argsize - 1)) os << (delim);
			};

			os << (prefix) << (space);
			for (auto item : range) {
				delimited_out(item);
			}
			os << (space) << (suffix);
		};

		/* also called from void out(...) functions for compound types. e.g. void out(tuple&) */
		// template<typename... Args >
		inline	auto print_varargs = [](
			outstream_type & os, 
			auto && first,
			auto && ... args
			)
		{
			constexpr std::size_t pack_size = sizeof...(args);

			std::size_t arg_count = 0;

			auto delimited_out = [&](auto && val_)
			{
				os << (val_);
				if (arg_count < pack_size) {
					os << (delim);
				}
				arg_count += 1;
			};

			os << (prefix) << (space);

			delimited_out(first);

			if constexpr (pack_size > 0) {
				(delimited_out(args), ...);
			}

			os << (space) << (suffix);
		};

	} // internal nspace

	// fundamental std types

	inline outstream_type & operator<<
		(outstream_type & os, const char * x_)
	{
		static dbj::str::wchar_range_to_string conv_{};

		os << conv_(x_);
		return os;
	}

	inline outstream_type & operator<<
		(outstream_type & os, std::string x_)
	{
		static dbj::str::wchar_range_to_string conv_{};

		os << conv_(x_);
		return os;
	}


	 // ranges
	template<typename T>
	inline outstream_type & operator<<
		(	outstream_type & os,const std::vector<T>& vec	)
	{
		internal::print_range(os, vec);
		return os;
	}

	template<typename K, typename V	>
	inline outstream_type & operator<<
		(outstream_type & os, const std::map<K, V> & map_) {
		if (map_.empty()) return os;
		internal::print_range(os,map_);
		return os;
	}

	template<typename T, std::size_t S	>
	inline outstream_type & operator<<
		(outstream_type & os, const std::array<T, S> & arr_) {
		if (arr_.empty()) return os;
		internal::print_range(os, arr_);
		return os;
	}

	// std stuff not a range 
	template<typename T>
	inline outstream_type & operator<<
		(outstream_type & os, const std::variant<T>& x_)
	{
		os << std::get<0>(x_); return os;
	}

/*
	inline	outstream_type & operator<<
		(	outstream_type & os, std::wstring s_)
	{
		os << s_.c_str() ;
		return os;
	}

	inline outstream_type & operator<<
		(	outstream_type & os, const std::string & s_	)
	{
		os << s_.c_str();
		return os;
	}
*/
	inline outstream_type & operator<<
		(	outstream_type & os, const dbj::console::painter_command & cmd_	)
	{
		dbj::console::paint(cmd_);
		return os;
	}

	inline outstream_type & operator<<
		(outstream_type & os, const dbj::app_env::structure & aes_)
	{
		os << " dbj::app_env::structure output operator has to be implemented";
		return os;
	}

	inline outstream_type & operator<<
		(outstream_type & os, const dbj::Exception & x_)
	{
		static dbj::str::wchar_range_to_string conv_{};

		using dbj::console::painter_command;
		os << painter_command::bright_red
			<< conv_( x_.what() )
			<< painter_command::text_color_reset ;
		return os;
	}

	template <class... Args>
	inline outstream_type & operator<< (outstream_type & os, std::tuple<Args...> tple) {

		if (std::tuple_size< std::tuple<Args...> >::value < 1) return os;

		std::apply(
			[&](auto&&... xs) {
			internal::print_varargs(os, xs...);
		},
			tple);
		return os;
	}

	template <typename T1, typename T2>
	inline outstream_type & operator<< (outstream_type & os, const std::pair<T1, T2>& pair_) {
		std::apply(
			[&](auto&&... xs) {
			internal::print_varargs(os, xs...);
		},
			pair_);
		return os;
	}

	/* output the { ... } aka std::initializer_list<T> */
	template <class... Args>
	inline outstream_type & operator<<(outstream_type & os, const std::initializer_list<Args...> & il_)
	{
		if (il_.size() < 1) return;
		std::apply(
			[&](auto&&... xs) {
			internal::print_varargs(os, xs...);
		},
			il_);
		return os;
	}
}