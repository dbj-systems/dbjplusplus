#pragma once
/*
2018 06 18	DBJ		Added
A micro log indeed. Although completely separated from dbj win con 
and this has to be rectified with a common engine for console/logging
which is a future extensions
(c) 2018 by dbj.org license at eof
*/
#pragma once

#include <ctime>
#include <array>
#include <chrono>
#include <vector>
#include <sstream>
// #include <iostream>

// this will do      std::ios::sync_with_stdio(true);
#define DBJLOG_SYNC_WITH_STDIO

#include "dbj_micro_log_fwd.h"
#include "dbj_micro_log_ops.h"

namespace dbj {
	namespace god_of_time {

		using namespace std;

		inline constexpr auto BUFSIZE = 1024u;
		typedef array<char, BUFSIZE>  char_buffer;
		typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;


		inline std::time_t systemtime_now()
		{
			system_time_point system_now = std::chrono::system_clock::now();
			return std::chrono::system_clock::to_time_t(system_now);
		}

		/// <summary>
		/// Apparently this is thread safe
		/// </summary>
		/// <param name="time">our own std::time_t buffer</param>
		/// <returns>std::tm snapshot</returns>
		inline tm localtime(std::time_t time = 0)
		{
			std::tm tm_snapshot;
			if (time == 0) time = systemtime_now();
			localtime_s(&tm_snapshot, (&time));
			return tm_snapshot;
		}

		inline char_buffer timestamp()
		{
			// locale::global(locale(nullptr));
			// time_t t = time(nullptr);
			// std::time_t tt_{};
			const auto lt_ = &localtime();
			char_buffer mbstr;
			if (strftime(mbstr.data(), mbstr.size(), "%c", lt_)) {
				return mbstr;
			}
			throw runtime_error("timestamp() -- strftime has failed");
		}
	} // god_of_time

	namespace log {

		class DBJLog final {

			static constexpr bool PIPE_OUT{ false };
			static constexpr bool TIMESTAMP{ false };

			static void output_and_reset(std::wstring & buffer_) {

				if (buffer_.empty()) return;

					// if (DBJLog::PIPE_OUT) {
						// if one wants to pipe/redirect the console output
						// auto rez[[maybe_unused]]
						// = ::_putws(string_trans.data());
						// _ASSERTE(EOF != rez);
						dbj::console::console_.out(buffer_);
					// }
#ifdef _DEBUG
					_RPT0(_CRT_WARN, buffer_.c_str());
#endif
					// clear the buffer afterwards
					buffer_.clear();
			}
/*
usage: MyBuf buff;
std::ostream stream(&buf);
stream << 1 << true << L"X" << std::flush ;
*/
/*
			mutable struct DBJBuf final 
				: public dbj::log::stringbuf_type
			{
				// called on stream flush
				// overload from the string stream buffer
				virtual int sync() {
					
					// use this->str() here
					// it contains whatever was streamed 
					// into this instance
					// until flush() was called
					auto string_trans = this->str();

					if (!string_trans.empty()) {

						if (DBJLog::PIPE_OUT) {
							// if one wants to pipe/redirect the console output
							// auto rez[[maybe_unused]]
							// = ::_putws(string_trans.data());
							// _ASSERTE(EOF != rez);
							dbj::win::con::out(string_trans.data());
						}
#ifdef _DEBUG
						_RPT0(_CRT_WARN, string_trans.data());
#endif
						// clear the buffer afterwards
						this->str(L"");
					}
					return 1 ;
				}
			} buffer_{};
			// mutable  DBJBuf buffer_{};
*/
			std::wstring buffer_{ };

			DBJLog() {

				this->buffer_.resize( dbj::log::bufsiz );
#ifdef DBJLOG_SYNC_WITH_STDIO
				std::ios::sync_with_stdio(true);
#else
				std::ios::sync_with_stdio(false);
#endif
				// this->flush();
			}


			mutable bool flushed{};
			// false is default bool value

		public:

			typedef dbj::log::outstream_type   
				stream_type;

			// DBJLog made *only* here
			friend DBJLog & instance();


			// streaming target for clients to use
			stream_type & stream() noexcept {
				static stream_type log_stream_  { this->buffer_ };
				return log_stream_;
			}

// manual flush() 
void flush() {
	stream_type & stream_ = this->stream();
		_ASSERTE(stream_.good());
		stream_.flush();
	auto * str_buf_object = stream_.rdbuf();
	_ASSERTE(str_buf_object != nullptr );
		str_buf_object->pubsync();
			auto str_device_ = str_buf_object->str();
		output_and_reset(str_device_);
	str_buf_object->str(L"");
}

	~DBJLog() {
		try {
			if (!flushed) {
				this->flush();
				flushed = true;
			}
			this->buffer_.clear();
#ifdef DBJLOG_SYNC_WITH_STDIO
			std::ios::sync_with_stdio(false);
#endif
		}
		catch (...) {
#if _DEBUG
			DBJ::TRACE("\n\nUnknown exception in %s", __func__);
#endif
		}
	}

};

		inline DBJLog & instance() {
			static DBJLog single_instance_
				= [&]() -> DBJLog {
				// this is anonymous lambda 
				// called only once, if required 
				// do some more complex 
				// initialization here
				return {};
			}(); // call immediately
			return single_instance_;
		};


		namespace inner {
			inline auto & stream_ 
				= (dbj::log::instance()).stream();
		}

		// for customers
		inline void flush() {
			(dbj::log::instance()).flush();
		}

/*
just print to the stream as ever
*/
		inline auto print = [](auto && ... param)
		{
			constexpr auto no_of_args = sizeof...(param);

			if constexpr (no_of_args > 0) {

				if (DBJLog::TIMESTAMP) {
					inner::stream_ << std::endl
						<< dbj::god_of_time::timestamp().data() << " : ";
				}
					(inner::stream_ << ... << param);
			}
#ifndef __clang__
			return print;
#endif
		};

	} // log
} // dbj

  /*
  Copyright 2018 by dbj@dbj.org

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http ://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
  */
/*
DBJ_TEST_SPACE_OPEN( micro_log )

DBJ_TEST_UNIT(" dbj micro log test") {

	dbj::log::print(L"\nHello, are you ok", L"with two, or", " three arguments?");
	dbj::log::instance().flush();
	dbj::log::print("\nDBJ")(" micro")(" log!");
	dbj::log::instance().flush();

	std::byte dumzy[[maybe_unused]]{};

}

DBJ_TEST_SPACE_CLOSE
*/

