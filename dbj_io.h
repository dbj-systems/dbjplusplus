#pragma once
#ifdef DBJ_IO_PRINT
namespace dbj {
	/*
	"modern" printf like output in modern c++, without printf type descriptors required.
	c++14 capable compiler should suffice.

	Inspired with : https://msdn.microsoft.com/magazine/dn973010
	Which in turn was inspired with http://en.cppreference.com/w/cpp/language/parameter_pack
	Keny's idea was to implement that, but without std::iostreams.
	Here is MUCH simpler version using iostreams
	*/
	namespace io {

		constexpr auto LEFTPAREN = '(', RIGHTPAREN = ')';

		inline auto & TARGET () { return ::std::cout; };
		/*
		bellow we add anonimous one namespace for each newly discovered complex type required
		to be printed by this lib
		*/
		namespace {
			
			// generic range printing of anything that has begin() and end()
			template<typename SEQ>
			inline void print_sequence(const SEQ & r) {

						for (auto e : r) TARGET() << e ;
			}
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

			/* operator which takes a right - hand operand of type 'std::initializer_list<_Ty>'*/
			template <class T>
			inline
				std::ostream& operator<<(std::ostream& out, const std::initializer_list<T>& il) {
				out << LEFTPAREN;
				print_sequence(il);
				return out << RIGHTPAREN;
			}

			template<typename T>
			inline unsigned sizer(T & t) { return t.size(); }
			/* operator which takes a right - hand operand of type 'std::array<_Ty, N>'*/
			template <class T, unsigned N >
			inline
				std::ostream& operator<<(std::ostream& out, const std::array<T, N>& arr) {
				out << LEFTPAREN;
				print_sequence(arr);
				return out << RIGHTPAREN;
			}

			inline  std::ostream& 
				operator<<(std::ostream& out, const std::wstring & ws_ ) {
				return out << ws_.c_str() ;
			}

			inline  std::ostream&
				operator<<(std::ostream& out, const std::string & ns_) {
				return out << ns_.c_str() ;
			}

			template <typename T>
			inline  std::ostream&
				operator<<(std::ostream& out, const std::variant<T> & var_) 
			{
				return out << std::get<0>(var_) ;
			}

			template<typename T, typename std::enable_if_t< std::is_same<bool,T>::value >::type  >
			inline  std::ostream&
				operator<<(std::ostream& out, const bool & tf_) {
					return out << (tf_ ? "true" : "false");
			}
		} // namespace

		namespace {
			/* the single gateway to the cout*/
			template<typename T>
			inline void print(const T & format)
			{
				std::cout << std::boolalpha << format;
			}
#if 0
			inline void print(const dbj::Exception & x)
			{
				std::cout << std::endl << x.what() << std::endl;
			}


			template<typename T, typename... Targs>
			inline
				void print(const char* format, T value, Targs... Fargs) // recursive variadic function
			{
				for (; *format != '\0'; format++) {
					if (*format == '%') {
						print(value);
						print(format + 1, Fargs...); // recursive call
						return;
					}
					print(*format);
				}
			}
#endif
			/* non recursive version
			   also with no format token '%', because it is tedious for
			   when there is a lot of them to remember on the right
			   of the format sentence what values to provide and in which order
			*/
			template<typename... Targs>
			inline	void printex(Targs... args)
			{
				if constexpr (sizeof...(Targs) > 0) {
					// since initializer lists guarantee sequencing, this can be used to
					// call a function on each element of a pack, in order:
					char dummy[sizeof...(Targs)] = { (print(args), 0)... };
				}
			}
		}
//		static constexpr const char * const DBJ_NV_DELIMITER = " : ";
	}
}
#endif // #ifdef DBJ_IO_PRINT


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
