#pragma once
/*
from previous console implementation
*/
#pragma region not in use
#if 0
namespace internal {
	constexpr char space = ' ', prefix = '{', suffix = '}', delim = ',';

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
			out__(val_);
			if ((arg_count++) < (argsize - 1)) out__(delim);
		};

		out__(prefix); out__(space);
		for (auto item : range) {
			delimited_out(item);
		}
		out__(space); out__(suffix);
	};

	/* also called from void out__(...) functions for compound types. e.g. void out__(tuple&) */
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
			out__(val_);
			if (arg_count < pack_size) {
				out__(delim);
			}
			arg_count += 1;
		};

		out__(prefix); out__(space);

		delimited_out(first);

		if constexpr (pack_size > 0) {
			(delimited_out(args), ...);
		}

		out__(space); out__(suffix);
	};

} // internal nspace
  /*
    console.out__(...) is the only method to output to a console
  this is the special out__ that does not use the console output class
  but painter commander

  Thus we achieved a decoupling of console and painter
  */
	  template<
		  typename PC ,
		  typename std::enable_if_t<
		  std::is_same_v< std::decay_t<PC>, painter_command>
		  , int> = 0
	  >
	  inline void out__	( const PC & cmd_ )
	  {
		  painter_commander().execute(cmd_);
	  }
		  template<
			  typename N ,
			  typename std::enable_if_t<std::is_arithmetic_v<std::decay_t<N>>, int > = 0
		  >
		  inline void out__	( const N & number_	)
		  {
			  // static_assert( std::is_arithmetic<N>::value, "type N is not a number");
			  console_.out__(std::to_wstring(number_));
		  }

		  template< typename B,
			  typename std::enable_if_t< dbj::is_bool_v<B>, int > = 0 >
		  inline void out__
		  ( const B & val_ )
		  {
			  console_.out__((true == val_ ? L"true" : L"false"));
		  }
		  /*
		   output the standard string
		   enable only if it is made out__
		   of standard chars
		   */
		  template< typename T >
		  inline void out__ ( const std::basic_string<T> & s_ ) {

			  static_assert(dbj::str::is_std_char_v<T>);

			  if (!s_.empty())
				  console_.out__(s_);
		  }

		  /*
		  output the standard string view
		  enable only if it is made out__
		  of standard chars
		  */
		  template<typename T	>
		  inline void out__ ( std::basic_string_view<T>  s_ )
		  {
			  static_assert( dbj::str::is_std_char_v<T> );

			  if (!s_.empty())
				  console_.out__(s_);
		  }

		  inline void out__(const std::string & s_) {
			  if (!s_.empty())
				  console_.out__(s_);
		  }

		  inline void out__(const std::u16string  & s_) {
			  if (!s_.empty())
				  console_.out__(s_);
		  }

		  inline void out__(const std::u32string  & s_) {
			  if (!s_.empty())
				  console_.out__(s_);
		  }
		  inline void out__(const std::string_view sv_) {
			  if (sv_.empty()) return;
			  console_.out__(sv_);
		  }

		  inline void out__(const std::wstring_view sv_) {
			  if (sv_.empty()) return;
			  console_.out__(sv_);
		  }

		  inline void out__(const std::u16string_view sv_) {
			  if (sv_.empty()) return;
			  console_.out__(sv_);
		  }

		  inline void out__(const std::u32string_view sv_) {
			  if (sv_.empty()) return;
			  console_.out__(sv_);
		  }

		  implement for these when required

	  template<typename T, size_t N>
	  inline void out__(const T(*arp_)[N]) {
		  using arf_type = T(&)[N];
		  arf_type arf = (arf_type)arp_;
		  internal::print_range(arf);
	  }


	  template<size_t N>
	  inline void out__(const char(&car_)[N]) {
		  console_.out__(
			  std::string_view(car_, car_ + N)
		  );
	  }

	  template<size_t N>
	  inline void out__(const wchar_t(&wp_)[N]) {
		  console_.out__(std::wstring(wp_, wp_ + N));
	  }

	  inline void out__(const char * cp) {
		  _ASSERTE(cp != nullptr);
		  console_.out__(std::string(cp));
	  }

	  inline void out__(const wchar_t * cp) {
		  _ASSERTE(cp != nullptr);
		  console_.out__(std::wstring(cp));
	  }

	  inline void out__(const char16_t * cp) {
		  _ASSERTE(cp != nullptr);
		  console_.out__(std::u16string(cp));
	  }

	  inline void out__(const char32_t * cp) {
		  _ASSERTE(cp != nullptr);
		  console_.out__(std::u32string(cp));
	  }

	  inline void out__(const wchar_t wp_) {
		  console_.out__(std::wstring(1, wp_));
	  }

	  inline void out__(const char c_) {
		  char str[] = { c_ , '\0' };
		  console_.out__(std::wstring(std::begin(str), std::end(str)));
	  }

	  inline void out__(const char16_t wp_) {
		  console_.out__(std::u16string{ 1, wp_ });
	  }

	  inline void out__(const char32_t wp_) {
		  console_.out__(std::u32string{ 1, wp_ });
	  }
	  /*
	  now we will deliver out__() overloads for "compound" types using the ones above
	  made for intrinsic types
	  ------------------------------------------------------------------------
	  output the exceptions
	  */

		  output array of T
		  template <typename T, size_t N >
			  inline void out__( const T (&carr) [N], typename std::enable_if_t<
				  std::is_array_v< std::remove_cv_t<T>[N] >
				  , int > = 0 )
		  {
			  static_assert(N > 1);
			  internal::print_range((T(&)[N])carr);
		  }

		  output array of T *
		  template <typename T, size_t N,
			  typename actual_type = std::remove_cv_t< std::remove_pointer_t<T> >
		  >
			  inline void out__(const T(*carr)[N], typename std::enable_if_t<
				  std::is_array_v< std::remove_cv_t<T>[N] >
				  , int > = 0 )
		  {
			  static_assert(N > 1);
			  internal::print_range((T(&)[N])carr);
		  }

		  output std char type
		  template < typename T >
			  inline void out__(const T & chr,
				  typename std::enable_if_t< dbj::is_std_char_v<T>
				  , int > = 0)
		  {
			  using actual_type = std::remove_cv_t< T >;
			  actual_type car[]{chr};
				  console_.out__(chr);
		  }
		  output pointer to std char type
		  template < typename T,
			  typename actual_type = std::remove_cv_t< std::remove_pointer_t<T> >,
			  typename std::enable_if_t<
			  dbj::is_std_char_v<actual_type>
			  , int > = 0
		  >
		  inline void out__(const T * ptr)
		  {
			  _ASSERTE(ptr != nullptr);
			  using actual_type = std::remove_cv_t< std::remove_pointer_t<T> >;
				  console_.out__(std::basic_string<T>{ptr});
		  }

			  inline auto print = [] ( const auto & first_param, auto && ... params)
			  {
				   out(first_param);

				  // if there are  more params
				  if constexpr (sizeof...(params) > 0) {
					  // recurse
					  print(params...);
				  }
				  return print;
			  };
#endif
#pragma endregion