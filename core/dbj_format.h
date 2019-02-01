#pragma once

#include "dbj_buffer.h"

namespace dbj {
	namespace fmt {
		/*
		somewhat inspired with
		https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
		*/
		template <typename T>
		inline T const & frm_arg( T const & value) noexcept
		{
			return value;
		}
		// no pass by value allowed
		template <typename T> T & frm_arg(T && value) = delete;

#pragma region dbj buffer and friends

		// template <typename T>
		inline char * frm_arg(  std::unique_ptr<char[]> const & value) noexcept
		{
			return value.get() ;
		}

		inline wchar_t * frm_arg(  std::unique_ptr<wchar_t[]> const & value) noexcept
		{
			return value.get() ;
		}

		inline char * frm_arg(  ::dbj::buf::char_buffer const & value) noexcept
		{
			return value.data() ;
		}

#pragma endregion 

		template <typename T>
		inline T const * frm_arg(std::basic_string<T> const & value) noexcept
		{
			return value.c_str();
		}

		template <typename T>
		inline T const * frm_arg(std::basic_string_view<T> const & value) noexcept
		{
			return value.data();
		}
		/*
		vaguely inspired by
		https://stackoverflow.com/a/39972671/10870835
		*/
		template<typename ... Args>
		inline dbj::buf::smart_arr
			to_buff(std::string_view format_, Args const & ... args)
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

			return buf;
		}
		// wide version
		template<typename ... Args>
		inline dbj::buf::smart_warr
			to_buff(std::wstring_view format_, Args const & ... args)
			noexcept
		{
			static_assert(sizeof...(args) < 255, "\n\nmax 255 arguments allowed\n");
			const auto fmt = format_.data();
			// 1: what is he size required
			const size_t size = std::swprintf(nullptr, 0, fmt, frm_arg(args) ...) + 1;
			// 2: use it at runtime
			auto buf = std::make_unique<wchar_t[]>(size);
			// each arg becomes arg to the frm_arg() overload found
			std::swprintf(buf.get(), size, fmt, frm_arg(args) ...);

			return buf;
		}

	template<typename ... Args>
	inline void
		print(std::string_view format_, Args const & ... args)
		noexcept
	{
		std::wprintf(L"%S", fmt::to_buff(format_, args...).get());
	}

	} // fmt

} // dbj

namespace dbj {
	namespace core {
#if 0
		template <typename ... Args>
		inline std::wstring printf_to_buffer
		(wchar_t const * const message, Args ... args) noexcept
		{
			auto buf_ = dbj::fmt::to_buff(message, args...);
			return { buf_.get() };
		}

		template <typename ... Args>
		inline std::string printf_to_buffer
		(const char * const message, Args ... args) noexcept
		{
			auto buf_ = dbj::fmt::to_buff(message, args...);
			return { buf_.get() };
		}
#endif
		// DBJ::TRACE exist in release builds too
		template <typename ... Args>
		inline void trace(wchar_t const * const message, Args ... args) noexcept
		{
			auto buf_ = dbj::fmt::to_buff(message, args...);
			::OutputDebugStringW(buf_.get()	);
		}
		template <typename ... Args>
		inline void trace(const char * const message, Args ... args) noexcept
		{
			auto buf_ = dbj::fmt::to_buff(message, args...);
			::OutputDebugStringA(buf_.get()	);
		}

#pragma warning( push )
#pragma warning( disable: 4190 )

		using smart_buf_type = typename dbj::buf::smart_arr;

		extern "C" {

			/*	transform path to filename,	delimiter is '\\' */
			inline	smart_buf_type
				filename(std::string_view file_path, const char delimiter_ = '\\')
				noexcept
			{
				_ASSERTE(!file_path.empty());
				size_t pos = file_path.find_last_of(delimiter_);
				return
					dbj::fmt::to_buff("%s",
					(std::string_view::npos != pos
						? file_path.substr(pos, file_path.size()) 
						: file_path )
				);
			}

			/*
			usual usage :
			DBJ::FILELINE( __FILE__, __LINE__, "some text") ;
			*/
			// inline std::string FILELINE(const std::string & file_path,
			inline 
				smart_buf_type
				fileline (std::string_view file_path,
				          unsigned line_,
				          std::string_view suffix = "")
			{
				_ASSERTE(!file_path.empty());

				return 
					dbj::fmt::to_buff(
						"%s(%u)%s", filename(file_path), line_, (suffix.empty() ? "" : suffix.data())
					);
			}

		} // extern "C"
#pragma warning( pop )
	} //core
} // dbj

#include "../dbj_gpl_license.h"

#pragma comment( user, DBJ_BUILD_STAMP ) 
