#pragma once

/*
2018-04-30	dbj@dbj.org		created
*/

#include <windows.h>
#include <wchar.h>
#include <crtdbg.h>

extern "C" {
	/*
	*Entry:
	*       const char *_string1 = pointer to beginning of the first string
	*       const char *s1e = pointer past end of the first string
	*       const char *_string2 = pointer to beginning of the second string
	*       const char *_end2 = pointer past end of the second string
	*
	*Exit:
	*       Less than 0    = first string less than second string
	*       0              = strings are equal
	*       Greater than 0 = first string greater than second string
	*/
	static int __cdecl dbj_ordinal_compareA(
		const char *_string1,
		const char *_end1,
		const char *_string2,
		const char *_end2)
	{
		int n1 = (int)((_end1 - _string1));
		int n2 = (int)((_end2 - _string2));

		int ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
		int ret = (ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1);

		return ret;
	}

	static int __cdecl dbj_ordinal_compareW(
		const wchar_t *_string1,
		const wchar_t *_end1,
		const wchar_t *_string2,
		const wchar_t *_end2)
	{
		int n1 = (int)((_end1 - _string1));
		int n2 = (int)((_end2 - _string2));

		int ans = wmemcmp(_string1, _string2, n1 < n2 ? n1 : n2);
		int ret = (ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1);

		return ret;
	}

	static char * winfile_lowerize_stringA(char * the_str_) {
		_ASSERTE(the_str_);
		errno_t err_ = _strlwr_s(the_str_, strlen(the_str_) + (1 * sizeof(*the_str_)));
		return the_str_;
	}

	static wchar_t * winfile_lowerize_stringW(wchar_t * the_str_) {
		_ASSERTE(the_str_);
		errno_t err_ = _wcslwr_s(the_str_, wcslen(the_str_) + (1 * sizeof(*the_str_)));
		return the_str_;
	}

	/// <summary>
	/// ordinal comparison of two ascii null terminated strings
	/// </summary>
	static int winfile_ordinal_string_compareA(const char * str1, const char * str2, unsigned char ignore_case) {

		if (ignore_case) {
			winfile_lowerize_stringA((char *)str1);
			winfile_lowerize_stringA((char *)str2);
		}

		return dbj_ordinal_compareA(
			str1, str1 + strlen(str1), str2, str2 + strlen(str2)
		);
	}

	/// <summary>
	/// ordinal comparions of two unicode null terminated strings
	/// </summary>
	static int winfile_ordinal_string_compareW(const wchar_t * str1, const wchar_t * str2, unsigned char ignore_case) {

		if (ignore_case) {
			winfile_lowerize_stringW((wchar_t *)str1);
			winfile_lowerize_stringW((wchar_t *)str2);
		}
		return dbj_ordinal_compareW(
			str1, str1 + wcslen(str1), str2, str2 + wcslen(str2)
		);
	}

	/// <summary>
	/// This is to be used for "UI sting comparisons" or sorting and a such
	/// this is locale sensitive, legacy aka "pre vista" solution 
	/// when locales had ID's not names
	/// and when CompareStringEx was not
	/// </summary>
	int winfile_ui_string_compare(LPCTSTR str1, LPCTSTR str2, unsigned char ignore_case)
	{
		/// <summary>
		/// it is highly unlikely the user will change the
		/// locale between the calls, so we could have this
		/// as static 
		/// </summary>
		/* LCID aka DWORD */ unsigned long
			current_locale = GetUserDefaultLCID(); // not LOCALE_INVARIANT

		return CompareString(
			/* _In_ LCID    */ current_locale,
			/* _In_ DWORD   */ ignore_case == 1 ? LINGUISTIC_IGNORECASE : NORM_LINGUISTIC_CASING,
			/* _In_ LPCTSTR */ str1,
			/* _In_ int     */ -1,
			/* _In_ LPCTSTR */ str2,
			/* _In_ int     */ -1
		);
	}

} // extern "C"

  /* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017-2018 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
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
