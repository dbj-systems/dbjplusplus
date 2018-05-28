#pragma once
#ifdef DBJ_TESTING_EXISTS
namespace {
	DBJ_TEST_UNIT(" dbj string comparisons ") {
		auto rez =
			dbj_ordinal_string_compareW(L"A", L"A", true);
		auto zer =
			dbj_ui_string_compare(L"abra babra dabra", L"ABRA babra DABRA", true);
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


#endif