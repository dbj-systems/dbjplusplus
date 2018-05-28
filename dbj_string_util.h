#pragma once

#include "dbj_crt.h"
/*


#include <string>
#include <map>
#include <vector>


#include <locale>
#include <iterator>
*/

#include <sstream> // wstringstream

namespace dbj::str {

	/// <summary>
	/// argument is a reference 
	/// and is "lowerized" in place
	/// BIG NOTE: it is not clear if this is locale friendly ?
	/// </summary>
	inline void lowerize(std::wstring & key) {
		transform(key.begin(), key.end(), key.begin(), std::towlower);
	};

	inline void lowerize(std::string & key) {
		transform(key.begin(), key.end(), key.begin(),
			// this stunt is required as tolower returns 'int' 
			// and modern c++ has none of that, it wants char
			[](char chrter) -> char { return std::tolower(chrter); }
		);
	};

	/// <summary>
	/// ui compare means local friendly compare
	/// </summary>
	template<typename CT>
	inline int ui_string_compare(
		const CT * p1, const CT * p2, bool ignore_case
	)
	{
		_ASSERTE(p1 != nullptr);
		_ASSERTE(p2 != nullptr);
		std::basic_string<CT> s1{ p1 }, s2{ p2 };

		if (false == ignore_case) {
			lowerize(s1);
			lowerize(s2);
		}
		// the "C" locale is default 
		std::locale loc{};
		// get collate facet:
		auto & coll = std::use_facet<std::collate<CT> >{ loc };
		//
		return coll.compare(s1.begin(), s1.end(), s2.begin(), s2.end());
	}


	/// <summary>
	/// is Lhs prefix to Rhs
	/// L must be shorter than R
	/// </summary>
	inline  bool  is_prefix(
		const std::wstring_view & lhs, const std::wstring_view & rhs
	)
	{
		_ASSERTE(lhs.size() > 0);
		_ASSERTE(rhs.size() > 0);

		// "predefined" can not be a prefix to "pre" 
		// opposite is true
		if (lhs.size() > rhs.size()) return false;

		return dbj::equal_(
			lhs.begin(),
			lhs.end(),
			rhs.begin());
	}

	extern "C" inline auto
		add_backslash(wstring & path_)
	{
		constexpr auto char_backslash{ L'\\' };
		if (path_.back() != char_backslash) {
			path_.append(wstring{ char_backslash });
		}
		return path_.size();
	}


	dbj::wstring_vector tokenize (const wchar_t * szText, wchar_t token = L' ')
	{
		dbj::wstring_vector words{};
		std::wstringstream ss;
		ss.str(szText);
		std::wstring item;
		while (getline(ss, item, token))
		{
			if (item.size() != 0)
				words.push_back(item);
		}

		return words;
	}

}
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

