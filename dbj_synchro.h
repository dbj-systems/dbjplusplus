#pragma once

// does not require any include before
#include <mutex>

namespace dbj {

	/// <summary>
	/// dbj synchronisation primitives
	/// see the licence blurb at eof
	/// </summary>
	namespace sync {

		/// <summary>
		/// automatic use of std::mutex 
		/// example of making a function
		/// thread safe
		/// <code>
		/// void safe_fun ( ) {
		/// dbj::sync::lock_unlock auto_lock_ ;
		/// }
		/// </code>
		/// </summary>
		struct lock_unlock final {
			std::mutex mux_;

			lock_unlock() noexcept { mux_.lock(); }
			~lock_unlock() { mux_.unlock(); }
		};

// Multi Threaded Build 
#ifdef DBJ_MT
#define DBJ_AUTO_LOCK dbj::sync::lock_unlock __dbj_auto_lock__ 
#else
#define DBJ_AUTO_LOCK
#endif


		/// <summary>
		/// in presence of multiple threads
		/// guard value of type T
		/// example
		/// <code>
		/// static inline guardian<bool> signal_ ;
		/// </code>
		/// default bool is false
		/// <code>
		/// auto false_ = signal_.load() ;
		/// </code>
		/// switch to true
		/// <code>
		/// auto true_ = signal_.load(true) ;
		/// </code>
		/// </summary>
		template <typename T>
		class guardian final {
		public:
			typedef T value_type;
			value_type load() const {
				lock_unlock locker_;
				return treasure_;
			}
			value_type store(value_type new_value) const {
				lock_unlock locker_;
				return treasure_ = new_value;
			}
		private:
			mutable value_type treasure_{};
		};


	} // sync
} // dbj

  /*
  Copyright 2017,2018 by dbj@dbj.org

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

