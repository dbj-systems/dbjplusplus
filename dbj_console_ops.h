#pragma once

#include "dbj_traits.h"
#include "dbj_console_fwd.h"

// #define DBJ_TYPE_INSTRUMENTS

/// <summary>
/// out overloads for outputing to IConsole instance
/// all funamnetal types
/// all compound types
/// all ranges that have begin() and end() methods
/// </summary>
namespace dbj::console {

	 // inline Printer PRN{ &console_ };
	 inline const auto & PRN = printer_instance();


	 namespace inner {

		 /*
		 anything that has begin and end
		 NOTE: that includes references to native arrays
		 */
		 inline auto print_range = [](const auto & range) {

			 // not requiring  range.size();
			 // thus can do native arrays
			 std::size_t argsize =
				 static_cast<std::size_t>(
					 std::distance(
						 std::begin(range), std::end(range)
					 )
					 );

			 if (argsize < 1) return;

			 std::size_t arg_count{ 0 };

			 auto delimited_out = [&](auto && val_) {
				 // try to find the required out()
				 out(val_);
				 if ((arg_count++) < (argsize - 1)) PRN.wchar_to_console(wdelim_str);
			 };

			 PRN.wchar_to_console(wprefix_str); PRN.wchar_to_console(wspace_str);
			 for (auto item : range) {
				 delimited_out(item);
			 }
			 PRN.wchar_to_console(wspace_str); PRN.wchar_to_console(wsuffix_str);
		 };

		 /* also called from void out(...) functions for compound types. e.g. void out(tuple&) */
		 // template<typename... Args >
		 inline	auto print_varargs = [](
			 auto && first,
			 auto && ... args
			 )
		 {
			 constexpr std::size_t pack_size = sizeof...(args);

			 std::size_t arg_count = 0;

			 auto delimited_out = [&](auto && val_)
			 {
				 out(val_);
				 if (arg_count < pack_size) PRN.wchar_to_console(wdelim_str);
				 arg_count += 1;
			 };

			 PRN.wchar_to_console(wprefix_str); PRN.wchar_to_console(wspace_str);

			 delimited_out(first);

			 if constexpr (pack_size > 0) {
				 (delimited_out(args), ...);
			 }

			 PRN.wchar_to_console(wspace_str); PRN.wchar_to_console(wsuffix_str);
		 };

	 } // inner nspace



#ifdef DBJ_TYPE_INSTRUMENTS
#define DBJ_TYPE_REPORT_FUNCSIG	dbj::console::PRN.printf("\n\n%-20s : %s\n%-20s :-> ", "Function", __FUNCSIG__, " ")
	 template<typename T>
	 auto  dbj_type_report = []()  { using namespace dbj::tt; PRN.printf("\n%s", T::to_string<T>().c_str()); };
#else
#define DBJ_TYPE_REPORT_FUNCSIG __noop
	 template<typename T>
	 auto  dbj_type_report = []() { __noop;   };
#endif

#define DBJ_UNHANDLED_(P)\
	 PRN.printf(\
		 "\nunhandled " P " argument: %s\nunqualified_type: %s\nnot a pointer type: %s",\
		 DBJ_TYPENAME(T),\
		 DBJ_TYPENAME(actual::unqualified_type),\
		 DBJ_TYPENAME(actual::not_ptr_type)\
	 )

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

#ifdef DBJ_TYPE_INSTRUMENTS
		using argument_instrument = dbj::tt::instrument<T> ;
		PRN.printf("\n%-20s", "Master dispatcher");
		DBJ_TYPE_REPORT_FUNCSIG;
#endif
		// dbj_type_report<argument_type>( &console_ );

		if constexpr(std::is_fundamental_v< actual::unqualified_type >) {

			if constexpr(std::is_arithmetic_v< actual::unqualified_type >) {
				if constexpr(std::is_floating_point_v< actual::unqualified_type >) {
					DBJ_UNHANDLED_("floating point");
				}
				else if constexpr(std::is_integral_v< actual::unqualified_type >) {
					DBJ_UNHANDLED_("integral");
				}
			}
			else {
				DBJ_UNHANDLED_("fundamental");
			}
		}
		else if constexpr (std::is_compound_v< actual::unqualified_type >) {

			if constexpr (std::is_pointer_v< actual::unqualified_type >) {

				if constexpr (dbj::is_std_char_v<actual::not_ptr_type>) 
				{
					DBJ_UNHANDLED_("string literal");
				}
				else {
					// just a pointer, so print it's address
					PRN.printf("%p", specimen);
				}
			}
			else if constexpr (std::is_member_pointer_v< actual::unqualified_type >) {
				// std::is_member_object_pointer
				// std::is_member_function_pointer
				PRN.printf("%p", specimen);
			}
			else if constexpr (std::is_array_v< actual::unqualified_type >) {
				DBJ_UNHANDLED_("array");
			}
			else if constexpr (std::is_function_v< actual::unqualified_type >) {
				DBJ_UNHANDLED_("function");
			}
			else if constexpr (std::is_enum_v< actual::unqualified_type >) {
				DBJ_UNHANDLED_("enum");
			}
			else if constexpr (std::is_class_v< actual::unqualified_type >) {
				DBJ_UNHANDLED_("class");
			}
			else if constexpr (std::is_union_v< actual::unqualified_type >) {
				DBJ_UNHANDLED_("union");
			}
			else {
				DBJ_UNHANDLED_("compound");
			}
		}
		else {
			DBJ_UNHANDLED_(" ");
		}
	}

    // the one out for the native array references
	template <typename T, size_t N>
	void out(T(&native_array)[N])
	{
		DBJ_TYPE_REPORT_FUNCSIG;

		if constexpr (
			dbj::SameTypes<T,char>
			/* std::is_same_v<std::remove_cv_t<T>, char > */
			) {
			PRN.char_to_console(native_array);
		}
		else
			if constexpr (
				dbj::SameTypes<T, wchar_t>
			) 
			{
				PRN.wchar_to_console(native_array);
			}
			else 
			{
				// try any other type
				PRN.wchar_to_console( wprefix_str );
					for (auto && elem : native_array) {
						PRN.wchar_to_console(wspace_str);
						out(elem);
					}
				PRN.wchar_to_console(wsuffix_str);
			}
	}

	template <typename T, size_t N>
	inline void out(T(*native_array)[N])
	{
		DBJ_TYPE_REPORT_FUNCSIG;
		out<T, N>(*native_array);
	}

	// stings and string literals are different by design ***********************************************
	template<> inline void out<const char     *>(const char     * str) { DBJ_TYPE_REPORT_FUNCSIG;  _ASSERTE(str != nullptr); PRN.char_to_console(str); }
	template<> inline void out<const wchar_t  *>(const wchar_t  * str) { DBJ_TYPE_REPORT_FUNCSIG; _ASSERTE(str != nullptr); PRN.wchar_to_console(str); }
	template<> inline void out<const char16_t *>(const char16_t * str) { DBJ_TYPE_REPORT_FUNCSIG; _ASSERTE(str != nullptr); PRN.wchar_to_console( dbj::range_to_wstring(str).c_str() ); }
	template<> inline void out<const char32_t *>(const char32_t * str) { DBJ_TYPE_REPORT_FUNCSIG; _ASSERTE(str != nullptr); PRN.wchar_to_console( dbj::range_to_wstring(str).c_str() ); }

	template<> inline void out<std::string>(std::string str) 
	{ DBJ_TYPE_REPORT_FUNCSIG; if (! str.empty()) PRN.char_to_console(str.c_str()); }
	template<> inline void out<std::wstring>(std::wstring str) 
	{ DBJ_TYPE_REPORT_FUNCSIG; if (!str.empty()) PRN.cons()->out(str.data(), str.data() + str.size()); }
	template<> inline void out<std::u16string>(std::u16string str) 
	{ DBJ_TYPE_REPORT_FUNCSIG; if (!str.empty()) PRN.wchar_to_console(dbj::range_to_wstring(str).c_str()); }
	template<> inline void out<std::u32string>(std::u32string str) 
	{ DBJ_TYPE_REPORT_FUNCSIG; if (!str.empty()) PRN.wchar_to_console(dbj::range_to_wstring(str).c_str()); }

	// fundamental types ********************************************************************************
	template<> inline void out<nullptr_t>(nullptr_t) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("null"); }
	// fundamental - floating point types
	template<> inline void out<float>(float fv) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%f", fv); }
	template<> inline void out<double>(double fv) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%f", fv); }
	template<> inline void out<long double>(long double fv) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%f", fv); }
	// fundamental - integral types
	template<> inline void out<bool>(bool bv) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%s", (bv ? "true" : "false")); }
	// char types are integral types too
	template<> inline void out<char>(char val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }
	template<> inline void out<signed char>(signed char val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }
	template<> inline void out<unsigned char>(unsigned char val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }
	template<> inline void out<wchar_t>(wchar_t val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }
	template<> inline void out<char16_t>(char16_t val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }
	template<> inline void out<char32_t>(char32_t val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%c", val); }

	/// <summary>
	/// signed integer types (short int, int, long int, long long int);
	/// </summary>
	template<> inline void out<int>(int val) { DBJ_TYPE_REPORT_FUNCSIG;	PRN.printf("%d", val); }
	template<> inline void out<short int>(short int val) { DBJ_TYPE_REPORT_FUNCSIG;	PRN.printf("%d", val); }
	template<> inline void out<long int>(long int val) { DBJ_TYPE_REPORT_FUNCSIG;	PRN.printf("%d", val); }
	template<> inline void out<long long int>(long long int val) { DBJ_TYPE_REPORT_FUNCSIG;	PRN.printf("%d", val); }

	/// <summary>
	/// unsigned integer types (unsigned short int, unsigned int, unsigned long int, unsigned long long int);
	/// </summary>
	template<> inline void out<unsigned short int>(unsigned short int val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%X", val); }
	template<> inline void out<unsigned int>(unsigned int val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%X", val); }
	template<> inline void out<unsigned long int>(unsigned long int val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%X", val); }
	template<> inline void out<unsigned long long int>(unsigned long long int val) { DBJ_TYPE_REPORT_FUNCSIG; PRN.printf("%X", val); }

	// std classes

	template<typename T, size_t N> 
	inline void out(const std::array<T, N> & arr)
	{ DBJ_TYPE_REPORT_FUNCSIG; 
			inner::print_range(arr);
	}

}