#pragma once

namespace dbj {
	
	// if the argument of the DBJ_STRLITERALLEN macro is a pointer, 
	// code won't compile 
	template <typename T, size_t N>
	char(&ArraySizeHelper(T(&array)[N]))[N];

	#define DBJ_STRLITERALLEN(str) (sizeof(ArraySizeHelper(str)) - 1)

	/// <summary>
	/// The string literal is used only once. 
	/// The string literal length is evaluated during the compilation phase. 
	/// You cannot accidentally pass a pointer to the function and 
	/// incorrectly evaluate the string length.
	/// </summary>
	template<typename T, size_t N>
	int mystrncmp(const T *a, const T(&b)[N])
	{
		return _tcsnccmp(a, b, N - 1);
	}
}
/*
2018-04-30	dbj@dbj.org		
created	basically a C code and thus not in the namespace
*/

extern "C" {

/*
bellow is WIN32 API 
for standard C++ use 
dbj::str::ui_string_compare()
*/
#ifdef DBJ_WIN
/// <summary>
/// This is to be used for "UI string comparisons" or sorting and a such
/// </summary>
inline int dbj_win32_string_compare(LPCTSTR str1, LPCTSTR str2, unsigned char ignore_case)
{
	int rez = CompareStringEx(
		/* locale name  */ LOCALE_NAME_USER_DEFAULT,
		/* _In_ DWORD   */ ignore_case == 1 ? LINGUISTIC_IGNORECASE : NORM_LINGUISTIC_CASING,
		/* _In_ LPCTSTR */ str1,
		/* _In_ int     */ -1,
		/* _In_ LPCTSTR */ str2,
		/* _In_ int     */ -1,
		NULL, NULL, 0
	);
	switch (rez) {
	case CSTR_LESS_THAN: rez = -1; break;
	case CSTR_EQUAL: rez = 0; break;
	case CSTR_GREATER_THAN: rez = 1; break;
	}
	return rez;
}
#endif // DBJ_WIN

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
	inline int __cdecl dbj_ordinal_compareA(
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

	inline int __cdecl dbj_ordinal_compareW(
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

	/* must free the result */
	inline char * dbj_lowerize_stringA(const char * the_str_) {
		_ASSERTE(the_str_);
		char * dup = _strdup(the_str_);
		_ASSERTE(dup);
		const size_t sze_ = strlen(dup);
		size_t cnt_ = 0;
		do  {
			dup[cnt_] = tolower(dup[cnt_]);
		} while (cnt_++ < sze_);
		return dup;
	}

	/* must free the result */
	inline wchar_t * dbj_lowerize_stringW(const wchar_t * the_str_) {
		_ASSERTE(the_str_);
		wchar_t * dup = _wcsdup(the_str_);
		_ASSERTE(dup);
		const size_t sze_ = wcslen(dup);
		size_t cnt_ = 0;
		 do {
			dup[cnt_] = towlower(dup[cnt_]);
		 } while (cnt_++ < sze_);
		return dup;
	}

	/// <summary>
	/// ordinal comparison of two ascii null terminated strings
	/// </summary>
	inline int dbj_ordinal_string_compareA(const char * str1, const char * str2, unsigned char ignore_case) {

		if (ignore_case) {
			char * cp1 = dbj_lowerize_stringA(str1);
			char * cp2 = dbj_lowerize_stringA(str2);
			int rez = dbj_ordinal_compareA(
				cp1, cp1 + strlen(cp1), cp2, cp2 + strlen(cp2)
			);
			free(cp1);
			free(cp2);
			return rez;
		}
		else {
			return dbj_ordinal_compareA(
				str1, str1 + strlen(str1), str2, str2 + strlen(str2)
			);
		}
	}

	/// <summary>
	/// ordinal comparions of two unicode null terminated strings
	/// </summary>
	inline int dbj_ordinal_string_compareW(const wchar_t * str1, const wchar_t * str2, unsigned char ignore_case) {

		if (ignore_case) {
			wchar_t * cp1 = dbj_lowerize_stringW(str1);
			wchar_t * cp2 = dbj_lowerize_stringW(str2);
			int rez = dbj_ordinal_compareW(
				cp1, cp1 + wcslen(cp1), cp2, cp2 + wcslen(cp2)
			);
			free(cp1);
			free(cp2);
			return rez;
		}
		else {
			return dbj_ordinal_compareW(
				str1, str1 + wcslen(str1), str2, str2 + wcslen(str2)
			);
		}
	}


} // extern "C"

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
