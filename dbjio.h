
#pragma once
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
#include <iostream>

namespace dbj {

	namespace io {
		/*
		"modern" printf like output in modern c++, without printf type descriptors required.
		c++14 capable compiler should suffice.

		Inspired with : https://msdn.microsoft.com/magazine/dn973010
		Which in turn was inspired with http://en.cppreference.com/w/cpp/language/parameter_pack
		Kenys idea was to implement that, but without std::iostreams.

		Here is MUCH simpler version using iostreams
		*/
		constexpr auto LEFTPAREN = '(', RIGHTPAREN = ')';

		inline auto & TARGET () { return std::cout; };
		/*
		bellow we add anonimous one namespace for each newly discovered complex type required
		to be printed by this lib
		*/
		namespace {
			// generic range printing of anything that has begin() and end()
			template<typename RANGE>
			inline void print_range(const RANGE & r) {

						for (auto e : r) TARGET() << e ;
			}
		}
		namespace
		{
			/*
			http://cpplove.blogspot.rs/2012/07/printing-tuples.html
			std::tuple to std::ostream
			*/
			/* Define a type which holds an unsigned integer value */
			template<std::size_t> struct int_ {};

			template <class Tuple, size_t Pos>
			inline
				std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<Pos>) {
				out << std::get< std::tuple_size<Tuple>::value - Pos >(t) << ',';
				return print_tuple(out, t, int_<Pos - 1>());
			}

			template <class Tuple>
			inline
				std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<1>) {
				return out << std::get<std::tuple_size<Tuple>::value - 1>(t);
			}

			template <class... Args>
			inline
				std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& t) {
				out << LEFTPAREN;
				print_tuple(out, t, int_<sizeof...(Args)>());
				return out << RIGHTPAREN;
			}
		} // eof namespace

		namespace {
			/* operator which takes a right - hand operand of type 'std::initializer_list<_Ty>'*/
			template <class T>
			inline
				std::ostream& operator<<(std::ostream& out, const std::initializer_list<T>& il) {
				out << LEFTPAREN;
				print_range(il);
				return out << RIGHTPAREN;
			}
		}

		namespace {

			template<typename T>
			inline unsigned sizer(T & t) { return t.size(); }
			/* operator which takes a right - hand operand of type 'std::array<_Ty, N>'*/
			template <class T, unsigned N >
			inline
				std::ostream& operator<<(std::ostream& out, const std::array<T, N>& arr) {
				out << LEFTPAREN;
				print_range(arr);
				return out << RIGHTPAREN;
			}

			inline  std::ostream& 
				operator<<(std::ostream& out, const std::wstring & ws_ ) {
				return out << ws_.data() ;
			}

			inline  std::ostream&
				operator<<(std::ostream& out, const std::string & ns_) {
				return out << ns_.data();
			}
		}


		inline void print(const char* format) // base functions
		{
			std::cout << format;
		}

		inline void print(const wchar_t * format) // base functions
		{
			std::cout << format;
		}

		inline void print(const std::string & std_string_) // base functions
		{
			std::cout << std_string_.data();
		}


		template<typename T, typename... Targs>
		inline
			void print(const char* format, T value, Targs... Fargs) // recursive variadic function
		{
			for (; *format != '\0'; format++) {
				if (*format == '%') {
					std::cout << value;
					print(format + 1, Fargs...); // recursive call
					return;
				}
				std::cout << *format;
			}
		}

		template<typename... Targs>
		inline
		void printex(Targs... args) 
		{
			if constexpr (sizeof...(Targs) > 0 ) {
				// since initializer lists guarantee sequencing, this can be used to
				// call a function on each element of a pack, in order:
				char dummy[sizeof...(Targs)] = { (std::cout << args, 0)... };
			}
		}
	}
}
#define DBJVERSION __DATE__ __TIME__
// #pragma message("-------------------------------------------------------------")
#pragma message( "============> Compiled: " __FILE__ ", Version: " DBJVERSION)
// #pragma message("-------------------------------------------------------------")
#pragma comment( user, "(c) 2017 by dbj@dbj.org | Version: " DBJVERSION ) 
#undef DBJVERSION
