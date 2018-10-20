#pragma once

#pragma warning( push)
#pragma warning( disable: 4307 )

namespace dbj::util {

	extern "C" {
		constexpr int dbj_atoi(const char *str)
		{
			int res = 0; // Initialize result 
			// Iterate through all characters of input string and 
			// update result 
			for (int i = 0; str[i] != '\0'; ++i)
				res = res * 10 + str[i] - '0';
			// return result. 
			return res;
		}

		constexpr unsigned long dbj_get_seed()
		{
			int hour = dbj_atoi(__TIME__);
			int min = dbj_atoi(__TIME__ + 3);
			int sec = dbj_atoi(__TIME__ + 6);
			return 10000 * hour + 100 * min + sec;
		}

		// as an example, one can call bellow like this
		// constexpr inline unsigned long hash_code =
		//	dbj::util::hash(__FILE__);
		constexpr inline unsigned long hash(const char *str)
		{
			unsigned long hash = 5381;
			int c = 0;
			while ((c = *str++))
				hash = ((hash << 5) + hash) + c; // hash * 33 + c 
			return hash;
		}
	} // extern "C" linkage
	
	template<typename T, size_t N>
	constexpr inline unsigned long hashit(const T(&str)[N]) {
		return dbj::util::hash(str);
	}

	namespace inner
	{
		constexpr const char timestamp[]{ __TIMESTAMP__ };
		constexpr auto timestamp_hash_{ dbj::util::dbj_atoi(__TIMESTAMP__) };
	}
		// inspired by 
		// https://stackoverflow.com/questions/5355317/generate-a-random-number-using-the-c-preprocessor/17420032#17420032

	/*
	This is QUAZY RANDOM + it is the same on every call
	*/
		constexpr inline long compile_time_random() 
		{
			auto z = 362436069 * inner::timestamp_hash_;
			auto w = 521288629 * inner::timestamp_hash_,
				jsr = 123456789 * inner::timestamp_hash_,
				jcong = 380116160 * inner::timestamp_hash_;

			z = (36969 * (z & 65535) + (z >> 16)) << 16;
			w = 18000 * (w & 65535) + (w >> 16) & 65535;
			auto mwc = (z + w);
			jsr = (jsr = (jsr = jsr ^ (jsr << 17)) ^ (jsr >> 13)) ^ (jsr << 5);
			auto shr3 = jsr;
			jcong = 69069 * jcong + 1234567;
			auto cong = jcong;
			auto kiss = (mwc^cong) + shr3;
			return (kiss < 0 ? -1 * kiss : kiss);
		}

} //dbj::util

#pragma warning( pop )

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

