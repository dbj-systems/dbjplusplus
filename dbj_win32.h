#pragma once
//
#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOSERVICE
#define NOMINMAX
#include <windows.h>
#endif

#ifdef DBJ_GDI_LINE
#ifndef _GDIPLUS_H
// because GDI+ 
// can not cope with
// NOMINMAX
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")
#endif // _GDIPLUS_H
#endif
//
#include "dbj_defval.h"
//
namespace dbj {
	namespace win32 {
#ifdef DBJ_GDI_LINE
		namespace gdi {
			static struct dflt {
				dbj::holder<REAL>			widh{ 10 };
				dbj::holder<SmoothingMode>	smoothnes{ SmoothingMode::SmoothingModeAntiAlias };
				dbj::holder<LineCap>		linecap{ LineCap::LineCapRound };
			} DFLT_;
		}
		/*
		The WIN32 DrawLine function wrapper
		*/
		class LINE final {
		public:
			using REAL = Gdiplus::REAL;
			using SmoothingMode = Gdiplus::SmoothingMode;
			using LineCap = Gdiplus::LineCap;
			using GraphicP = std::unique_ptr<Gdiplus::Graphics>;
			using PenP = std::unique_ptr<Gdiplus::Pen>;
		private:
			GraphicP				gfx_ = nullptr;
			PenP					pen_ = nullptr;

		public:
			~LINE() {	}
			LINE(HDC hDC, Gdiplus::ARGB clr, Gdiplus::REAL width = DFLT_.widh())
			{
				gfx_ = ::std::make_unique<Gdiplus::Graphics>(hDC);
				if (width < 1) width = DFLT_.widh();
				pen_ = ::std::make_unique< Gdiplus::Pen>(Gdiplus::Color::Color(clr), width);
			}

			/* change the pen color and return the ARGB of a new color */
			Gdiplus::ARGB color(Gdiplus::ARGB clr) {

				Gdiplus::Color color_(clr);
				// Status SetColor(IN const Color& color)
				Gdiplus::Status status_ = pen_->SetColor(color_);
				DBJ_ASSERT(status_ == Gdiplus::Status::Ok);
				// Status GetColor(OUT Color* color) const
				status_ = pen_->GetColor(&color_);
				DBJ_ASSERT(status_ == Gdiplus::Status::Ok);
				return color_.MakeARGB(color_.GetAlpha(), color_.GetRed(), color_.GetGreen(), color_.GetBlue());
			}
			/* draw the line */
			void operator () (int sx, int sy, int ex, int ey, Gdiplus::REAL width = 0) {
				gfx_->SetSmoothingMode(
					DFLT_.smoothnes()
				);
				pen_->SetEndCap(
					DFLT_.linecap()
				);

				if (width > 0)
					pen_->SetWidth(width);
				else
					throw TEXT("Pen width less than 1?");

				gfx_->DrawLine(pen_.get(), sx, sy, ex, ey);
			}
		};

#endif // DBJ_GDI_LINE

#pragma region "dbj win32 string types"
		using CHAR_T = wchar_t;
		using STRING = std::wstring;
		using long_string_pointer = CHAR_T *; // LPWSTR;
#pragma endregion "dbj win32 string types"

		namespace inner {
			struct last final
			{
				mutable int error{};
				last() noexcept : error(::GetLastError()) {}
				~last() { ::SetLastError(0); }
			};
		}

		inline int last_error()
		{
			return inner::last{}.error;
		};

		//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
		inline auto getLastErrorMessage(
			const STRING & prompt = STRING{}, DWORD errorMessageID = last_error()
		)
		{
			//Get the error message, if any.
			if (errorMessageID == 0)
				return STRING{}; //No error message has been recorded

			long_string_pointer messageBuffer = nullptr;

			size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (long_string_pointer)&messageBuffer, 0, NULL);

			STRING message(messageBuffer, size);

			//Free the buffer.
			::LocalFree(messageBuffer);

			if (!prompt.empty())
				return STRING(prompt).append(message);

			return message;
		}

		inline auto getLastErrorMessage(
			const char * prompt, DWORD errorMessageID = last_error())
		{
			STRING wprompt = dbj::range_to_wstring(prompt);
			return getLastErrorMessage(wprompt, errorMessageID);
		}

		namespace sysinfo {
			using std::wstring;
			namespace {
				DWORD	INFO_BUFFER_SIZE = 1024;

				static wstring  infoBuf(INFO_BUFFER_SIZE, (wchar_t)0);
				//
				template<class F, class... Pack>
				constexpr inline auto
					call
					(F&& fun, Pack&&... args) {
					infoBuf.clear();
					infoBuf.resize(INFO_BUFFER_SIZE);
					if (0 == std::invoke(fun, (args)...))
						throw getLastErrorMessage(typeid(F).name());
					return (infoBuf);
				}
			}
			// 
			inline STRING computer_name() {
				static STRING computer_name_ =
					call(GetComputerName, infoBuf.data(), &INFO_BUFFER_SIZE);
				return computer_name_;
			}

			inline STRING user_name() {
				static STRING user_name_ =
					call(GetUserName, infoBuf.data(), &INFO_BUFFER_SIZE);
				return user_name_;
			}

			inline STRING system_directory() {
				static STRING system_directory_ =
					call(GetSystemDirectory, infoBuf.data(), INFO_BUFFER_SIZE);
				return system_directory_;
			}

			// return e.g. L"C:\windows\"
			inline STRING windows_directory() {
				static STRING windows_directory_ =
					call(GetWindowsDirectory, infoBuf.data(), INFO_BUFFER_SIZE);
				return windows_directory_;
			}

			// DBJ: what happens if there is no drive C: ?
			// we get the system drive letter
			// the first 3 chars that is, e.g. L"C:\\"
			inline const STRING windows_drive()
			{
				// it is unlikely system drive letter 
				// will change between calls
				static STRING result = windows_directory().substr(0, 3);
				_ASSERTE(result.size() == 3);
				return result;
			}

		} // sysinfo

		/* geo info api using mechanism 

		location two letter codes --> https://www.iso.org/obp/ui/#search/code/

		https://docs.microsoft.com/en-us/windows/desktop/api/winnls/ne-winnls-sysgeotype
		for SYSGEOTYPE
		*/
		// PWSTR is WCHAR *
		// thus for us it is wchar_t *

		using geo_info_map_type = std::map<const wchar_t *, std::wstring >;

		inline auto geo_info = [](PWSTR location) {

			auto info_query = [](PWSTR location, SYSGEOTYPE query)
				-> std::wstring
			{
// WINVER is set to	_WIN32_WINNT
// but GetGeoInfoEx requires REDSTONE 3 or above
#if (NTDDI_VERSION >= NTDDI_WIN10_RS3)
				/*https://docs.microsoft.com/en-us/windows/desktop/api/winnls/nf-winnls-getgeoinfoex*/

				auto use_geo_info = [&](PWSTR geoData, int geoDataCount) {
					return ::GetGeoInfoEx(
						(PWSTR)location,
						(GEOTYPE)query,
						(PWSTR)geoData,
						(int)geoDataCount
					);
				};

				int size = use_geo_info(NULL, 0);
				_ASSERTE(size > 0);

				std::wstring geoData(size, 0);
				int rezult = use_geo_info(geoData.data(), (int)geoData.size());

				if (0 == rezult) {
					const auto err = dbj::win32::last_error();
					if (err == ERROR_INSUFFICIENT_BUFFER)
						return { L"The supplied buffer size was not large enough, or it was incorrectly set to NULL." };
					if (err == ERROR_INVALID_PARAMETER)
						return { L"A parameter value was not valid." };
					if (err == ERROR_INVALID_FLAGS)
						return { L"The values supplied for flags were not valid" };
				}
				DBJ_ASSERT(rezult != 0);
					return geoData;
#else
				return std::wstring{ L"GetGeoInfoEx requires systems above REDSTONE 3" };
#endif
			};

			geo_info_map_type geo_info_map{};

			geo_info_map[L"GEO_LATITUDE"] = info_query(location, GEO_LATITUDE);
			geo_info_map[L"GEO_LONGITUDE"] = (info_query(location, GEO_LONGITUDE));
			geo_info_map[L"GEO_ISO2"] = (info_query(location, GEO_ISO2));
			geo_info_map[L"GEO_ISO3"] = (info_query(location, GEO_ISO3));
			geo_info_map[L"GEO_FRIENDLYNAME"] = (info_query(location, GEO_FRIENDLYNAME));
			geo_info_map[L"GEO_OFFICIALNAME"] = (info_query(location, GEO_OFFICIALNAME));
			geo_info_map[L"GEO_ISO_UN_NUMBER"] = (info_query(location, GEO_ISO_UN_NUMBER));
			geo_info_map[L"GEO_PARENT"] = (info_query(location, GEO_PARENT));
			geo_info_map[L"GEO_DIALINGCODE"] = (info_query(location, GEO_DIALINGCODE));
			geo_info_map[L"GEO_CURRENCYCODE"] = (info_query(location, GEO_CURRENCYCODE));
			geo_info_map[L"GEO_CURRENCYSYMBOL"] = (info_query(location, GEO_CURRENCYSYMBOL));
			geo_info_map[L"GEO_NAME"] = (info_query(location, GEO_NAME));

			return geo_info_map;

		}; // geo_info
	} // win32
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
