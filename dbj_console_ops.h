#pragma once

#include "dbj_traits.h"
#include "dbj_console_fwd.h"

/// <summary>
/// out overloads for outputing to IConsole instance
/// all funamnetal types
/// all compound types
/// all ranges that have begin() and end() methods
/// </summary>
namespace dbj::console {

	inline Printer printer{ &console_ };

#ifdef DBJ_TYPE_INSTRUMENTS
#define DBJ_TYPE_REPORT_FUNCSIG	dbj::console::printer.printf("\n\n%-20s : %s\n%-20s :-> ", "Function", __FUNCSIG__, " ")
#else
#define DBJ_TYPE_REPORT_FUNCSIG __noop
#endif
	template<typename T>
	auto  dbj_type_report = []()
	{
		using namespace dbj::tt;
		printer.printf("\n%s", T::to_string<T>().c_str());
	};

	/// <summary>
	/// in here we essentially mimic the standard C++ 
	/// type hierarchy as described in here
	/// https://en.cppreference.com/w/cpp/language/type
	/// 
	/// the one and only, only the lonely ...etc ...
	/// </summary>
	template <typename T>
	void out(T specimen)
	{
		using actual = dbj::tt::actual<T>;
		// using namespace dbj::tt;
		// using argument_instrument = dbj::tt::instrument<T> ;
#ifdef DBJ_TYPE_INSTRUMENTS
		printer.printf("\n%-20s", "Master dispatcher");
		DBJ_TYPE_REPORT_FUNCSIG;
#endif
		// dbj_type_report<argument_type>( &console_ );

		if constexpr(std::is_fundamental_v< actual::unqualified_type >) {

			if constexpr(std::is_arithmetic_v< actual::unqualified_type >) {
				if constexpr(std::is_floating_point_v< actual::unqualified_type >) {
					printer.printf("\nunhandled floating point type of argument %s", DBJ_VALTYPENAME(specimen));
				}
				else if constexpr(std::is_integral_v< actual::unqualified_type >) {
					printer.printf("\nunhandled integral type of argument: %s", DBJ_VALTYPENAME(specimen));
				}
			}
			else {
				printer.printf("\nunhandled fundamental type of argument: %s ", DBJ_VALTYPENAME(specimen));
			}
		}
		else if constexpr (std::is_compound_v< actual::unqualified_type >) {

			if constexpr (std::is_pointer_v< actual::unqualified_type >) {

				if constexpr (dbj::is_std_char_v<actual::not_ptr_type>) {
					// pointer to std char, aka string literal
					if constexpr (dbj::is_same_v<actual::not_ptr_type, char >) { printer.printf("%s", specimen); }
					else
						if constexpr (dbj::is_same_v<actual::not_ptr_type, wchar_t >) { printer.printf(L"%s", specimen); }
						else { printer.printf("unhandled string literal character type: %s", DBJ_VALTYPENAME(specimen)); }
				}
				else {
					// just a pointer, so print it's address
					printer.printf("%p", specimen);
				}
			}
			else if constexpr (std::is_member_pointer_v< actual::unqualified_type >) {
				// std::is_member_object_pointer
				// std::is_member_function_pointer
				printer.printf("%p", specimen);
			}
			else if constexpr (std::is_array_v< actual::unqualified_type >) {
				printer.printf("\nunhandled array type %s", DBJ_VALTYPENAME(specimen));
			}
			else if constexpr (std::is_function_v< actual::unqualified_type >) {
				printer.printf("\nunhandled function type: %s", DBJ_VALTYPENAME(specimen));
			}
			else if constexpr (std::is_enum_v< actual::unqualified_type >) {
				printer.printf("\nunhandled enum type %s", DBJ_VALTYPENAME(specimen));
			}
			else if constexpr (std::is_class_v< actual::unqualified_type >) {
				printer.printf("\nunhandled class type: %s", DBJ_VALTYPENAME(specimen));
			}
			else if constexpr (std::is_union_v< actual::unqualified_type >) {
				printer.printf("\nunhandled union type: %s", DBJ_VALTYPENAME(specimen));
			}
			else {
				printer.printf("\nunhandled compound type : %s", DBJ_VALTYPENAME(specimen));
			}
		}
		else {
			printer.printf("\nunhandled argument type: %s", DBJ_VALTYPENAME(specimen));
		}
	}

	// template <typename T> void out( const T * specimen){	DBJ_TYPE_REPORT(&console_, decltype(specimen)); }

	template <typename T, size_t N>
	void out(T(&native_array)[N])
	{
		DBJ_TYPE_REPORT_FUNCSIG;

		if constexpr (std::is_same_v<std::remove_cv_t<T>, char >) {
			printer.printf("%s", native_array);
		}
		else
			if constexpr (std::is_same_v<std::remove_cv_t<T>, wchar_t >) {
				printer.printf(L"%s", native_array);
			}
			else
				if constexpr (std::is_integral_v<T>) {
					out(" {");
					for (auto && elem : native_array) {
						out(" ");
						out(elem);
						out(" ");
					}
					out("} ");
				}
	}

	template <typename T, size_t N>
	inline void out(T(*native_array)[N])
	{
		DBJ_TYPE_REPORT_FUNCSIG;
		out<T, N>(*native_array);
	}

	// fundamental types ********************************************************************************
	template<> inline void out<nullptr_t>(nullptr_t) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("null"); }
	// fundamental - floating point types
	template<> inline void out<float>(float fv) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%f", fv); }
	template<> inline void out<double>(double fv) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%f", fv); }
	template<> inline void out<long double>(long double fv) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%f", fv); }
	// fundamental - integral types
	template<> inline void out<bool>(bool bv) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%s", (bv ? "true" : "false")); }
	// char types are integral types too
	template<> inline void out<char>(char val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }
	template<> inline void out<signed char>(signed char val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }
	template<> inline void out<unsigned char>(unsigned char val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }
	template<> inline void out<wchar_t>(wchar_t val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }
	template<> inline void out<char16_t>(char16_t val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }
	template<> inline void out<char32_t>(char32_t val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%c", val); }

	/// <summary>
	/// signed integer types (short int, int, long int, long long int);
	/// </summary>
	template<> inline void out<int>(int val) { DBJ_TYPE_REPORT_FUNCSIG;	printer.printf("%d", val); }
	template<> inline void out<short int>(short int val) { DBJ_TYPE_REPORT_FUNCSIG;	printer.printf("%d", val); }
	template<> inline void out<long int>(long int val) { DBJ_TYPE_REPORT_FUNCSIG;	printer.printf("%d", val); }
	template<> inline void out<long long int>(long long int val) { DBJ_TYPE_REPORT_FUNCSIG;	printer.printf("%d", val); }

	/// <summary>
	/// unsigned integer types (unsigned short int, unsigned int, unsigned long int, unsigned long long int);
	/// </summary>
	template<> inline void out<unsigned short int>(unsigned short int val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%X", val); }
	template<> inline void out<unsigned int>(unsigned int val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%X", val); }
	template<> inline void out<unsigned long int>(unsigned long int val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%X", val); }
	template<> inline void out<unsigned long long int>(unsigned long long int val) { DBJ_TYPE_REPORT_FUNCSIG; printer.printf("%X", val); }

}