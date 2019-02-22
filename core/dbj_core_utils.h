#pragma once

#include "dbj_buf.h"

#include <string>
#include <string_view>
#include <system_error>
#include <chrono>

namespace dbj {
	// the first time dbj::core namespace was introduced was here
	namespace core {
		namespace util {

			// replaced the use of 
			// std::string with buf_type
			// everywhere in this file
			using buf_type = ::dbj::buf::buff_type;

			// no can do, intelisense goes berserk  --> using namespace::std ;
			using namespace  ::std::literals::string_view_literals;
			namespace h = ::std::chrono;

			constexpr inline char const * TIME_STAMP_FULL_MASK
				= "%Y-%m-%d %H:%M:%S";

			constexpr inline char const * TIME_STAMP_SIMPLE_MASK
				= "%H:%M:%S";

			// time stamp size is max 22 + '\0'
			// updates the ref to std::error_code argument accordingly
			[[nodiscard]]
			inline buf_type::pointer
				make_time_stamp(
					std::error_code & ec_,
					char const * timestamp_mask_ = TIME_STAMP_SIMPLE_MASK
				) noexcept
			{
				const size_t buf_len = 32U;
				auto buffer_	= buf_type::make(buf_len);
				char * buf = buffer_.get();

				ec_.clear();

				// Get the current time
				auto now = h::system_clock::now();
				// Format the date/time
				time_t now_tm_t = h::system_clock::to_time_t(now);
				struct tm  local_time_ {};

				errno_t posix_err_code = ::localtime_s(&local_time_, &now_tm_t);
				// return empty buffer if error
				if (posix_err_code > 0) {
					ec_ = std::make_error_code((std::errc)posix_err_code); return {};
				}

				strftime(buf, buf_len, timestamp_mask_, &local_time_);
				// Get the milliseconds
				int millis = h::time_point_cast<h::milliseconds>(now).time_since_epoch().count() % 100;
				// 
				const auto strlen_buf = std::strlen(buf);
				(void)::sprintf_s(buf + strlen_buf, buf_len - strlen_buf, ".%03d", millis);

				return buffer_;
				// ec_ stays clear
			};

			/*	caller must check std::error_code ref arg	*/
			[[nodiscard]] inline 
				buf_type::pointer
				dbj_get_envvar(std::string_view varname_, std::error_code & ec_ ) noexcept
			{
				_ASSERTE(!varname_.empty());
				size_t buflen_ = 256U;
				auto	bar = buf_type::make(buflen_);
				ec_.clear(); // the OK 
				::SetLastError(0);
				if (1 > ::GetEnvironmentVariableA(varname_.data(), bar.get(), (DWORD)buflen_))
				{
					ec_ = std::error_code(::GetLastError(), std::system_category());
				}
					return bar ;
			}

			/*	caller must check the ec_	*/
			[[nodiscard]] inline buf_type::pointer
				program_data_path( std::error_code & ec_ ) noexcept
			{
					return dbj_get_envvar("ProgramData", ec_ );
			}
		} // util
	}// core
} // dbj