#pragma once

namespace dbj {
	namespace fmt {
		/*
		somewhat inspired with
		https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
		*/
		template <typename T>
		T & frm_arg(const T & value) noexcept
		{
			return value;
		}
		// no pass by value allowed
		template <typename T> T & frm_arg(T && value) = delete;

		template <typename T>
		T const * frm_arg(std::basic_string<T> const & value) noexcept
		{
			return value.c_str();
		}

		template <typename T>
		T const * frm_arg(std::basic_string_view<T> const & value) noexcept
		{
			return value.data();
		}
		/*
		inspired by
		https://stackoverflow.com/a/39972671/10870835
		*/
		template<typename ... Args>
		inline std::string
			string_print(std::string_view format_, Args const & ... args)
			noexcept
		{
			static_assert(sizeof...(args) < 255, "\n\nmax 255 arguments allowed\n");
			const auto fmt = format_.data();
			// 1: what is he size required
			const size_t size = std::snprintf(nullptr, 0, fmt, frm_arg(args) ...) + 1;
			// 2: use it at runtime
			auto buf = std::make_unique<char[]>(size);
			// each arg becomes arg to the frm_arg() overload found
			std::snprintf(buf.get(), size, fmt, frm_arg(args) ...);

			// dbj: why do we use std::string here?
			// why not dbj char_buffer ?
			auto res = std::string(buf.get(), buf.get() + size - 1);
			return res;
		}

	} // fmt

	template<typename ... Args>
	inline void
		dbj_print(std::string_view format_, Args const & ... args)
		noexcept
	{
		std::wprintf(L"%S", fmt::string_print(format_, args...).c_str());
	}
} // dbj

#include "../dbj_gpl_license.h"

#pragma comment( user, DBJ_BUILD_STAMP ) 
