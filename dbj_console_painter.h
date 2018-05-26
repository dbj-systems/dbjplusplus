#pragma once

#include <algorithm>
#include <variant>

#include "dbj_commander.h"

#ifndef _WINCON_
#ifndef _INC_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STRICT
#define NOMINMAX
#include <windows.h>
#endif

#ifndef _GDIPLUS_H
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

namespace dbj {
namespace win {
namespace con {
#pragma region "colors and painter"
namespace {
				/* modification of catch.h console colour mechanism */
				enum class Colour : unsigned {
					None = 0,		White,	Red,	Green,		Blue,	Cyan,	Yellow,		Grey,
					Bright = 0x10,

					BrightRed = Bright | Red,	BrightGreen = Bright | Green,
					BrightBlue = Bright | Blue,	LightGrey = Bright | Grey,
					BrightWhite = Bright | White,

				};
				/* stop the colour to text attempts*/
				std::ostream& operator << (std::ostream& os, Colour const&); // no op

				class __declspec(novtable) Painter final {
				public:

					Painter(
						HANDLE another_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE)
					) : stdoutHandle(another_handle_)
					{
						CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
						::GetConsoleScreenBufferInfo(stdoutHandle, &csbiInfo);
						originalForegroundAttributes = csbiInfo.wAttributes & ~(BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
						originalBackgroundAttributes = csbiInfo.wAttributes & ~(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					}

	const bool text( Colour _colourCode) const {
		switch (_colourCode) {
			case Colour::None:      return setTextAttribute(originalForegroundAttributes);
			case Colour::White:     return setTextAttribute(FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
			case Colour::Red:       return setTextAttribute(FOREGROUND_RED);
			case Colour::Green:     return setTextAttribute(FOREGROUND_GREEN);
			case Colour::Blue:      return setTextAttribute(FOREGROUND_BLUE);
			case Colour::Cyan:      return setTextAttribute(FOREGROUND_BLUE | FOREGROUND_GREEN);
			case Colour::Yellow:    return setTextAttribute(FOREGROUND_RED | FOREGROUND_GREEN);
			case Colour::Grey:      return setTextAttribute(0);

			case Colour::LightGrey:     return setTextAttribute(FOREGROUND_INTENSITY);
			case Colour::BrightRed:     return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_RED);
			case Colour::BrightGreen:   return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			case Colour::BrightBlue:   return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_BLUE);
			case Colour::BrightWhite:   return setTextAttribute(FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
			default: throw "Exception in "  __FUNCSIG__ " : not a valid colour code?";
		}
	}
	/* modern interface */
	const bool text( std::variant<Colour> _colourCode) const {
		return this->text(std::get<Colour>(_colourCode));
	} 

	const bool text_reset() const { return this->text(Colour::None); }

	private:
		bool setTextAttribute( WORD _textAttribute) const {
			return ::SetConsoleTextAttribute(stdoutHandle, _textAttribute | originalBackgroundAttributes);
		}
		HANDLE stdoutHandle;
		WORD originalForegroundAttributes;
		WORD originalBackgroundAttributes;
};
        // the one and only is hidden in here ----------------------------------------
		Painter painter_{};
		// ---------------------------------------------------------------------------
} // nspace
#pragma endregion "colors and painter"
#pragma region "commander setup"
	/*
	Here we use the dbj::cmd::Commander,  define the comand id's and functions to execute them etc..
	*/
	typedef enum : unsigned {
		white = 0,		red,		green,
		blue,			cyan,		yellow,
		grey,			bright_red, bright_blue, 
		text_color_reset,			nop = (unsigned)-1
	} painter_command ;

	using PainterCommandFunction = bool(void);
	using PainterCommander = dbj::cmd::Commander<painter_command, PainterCommandFunction >;

	namespace {
		auto factory_of_commands = []() -> PainterCommander & {
			// make the unique commander instance
			static PainterCommander commander_;
			// register command/function pairs
	commander_.reg({
		{ painter_command::nop,					[&]() {										return true;  }},
		{ painter_command::text_color_reset,	[&]() {	painter_.text_reset(); 				return true;  }},
		{ painter_command::white,				[&]() { painter_.text(Colour::White);		return true;  }},
		{ painter_command::red,					[&]() { painter_.text(Colour::Red);			return true;  }},
		{ painter_command::green,				[&]() { painter_.text(Colour::Green);		return true;  }},
		{ painter_command::blue,				[&]() { painter_.text(Colour::Blue);		return true;  }},
		{ painter_command::bright_red,			[&]() { painter_.text(Colour::BrightRed);	return true;  }},
		{ painter_command::bright_blue,			[&]() { painter_.text(Colour::BrightBlue);	return true;  }}
	});         // done
				return commander_;
		};
	} // nspace

	inline const PainterCommander  & painter_commander() {
		static 	const PainterCommander & just_call_once = factory_of_commands();
		return  just_call_once ;
	}
#pragma endregion
} // con
} // win
} // dbj

/* standard suffix for every other header here */
#pragma comment( user, __FILE__ "(c) 2017 by dbj@dbj.org | Version: " __DATE__ __TIME__ ) 
/*
Copyright 2017 by dbj@dbj.org

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
The only requirement for this file is not to have any #include's
*/
