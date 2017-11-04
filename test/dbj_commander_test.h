#pragma once
/*
  commander testing is using testing mechanism so we move it here 
  so that commander itself does not depened on testing

  NOTE: we do not include testing headers into the dbj++.h
*/
#ifdef DBJ_TESTING_EXISTS
namespace dbj {
	namespace cmd {
		namespace /* test the Commander*/ {
			/*
			Define comand id's first
			*/
			enum class CMD : unsigned {
				white = 0, red, green, blue, cyan,
				yellow, grey, bright_red, text_color_reset,
				nop = (unsigned)-1
			};

			DBJ_TEST_CASE("dbj cmd Commander<>") {
				/*
				observe CAREFULY!
				declaration of a function type, NOT function pointer type
				*/
				using cmd_fun_t = bool(void);
				/*
				BIG NOTE! [&] scoping operator '&' MUST exist
				*/
				auto paint_it_red = [&]() {
					dbj::printex("\n", __FUNCSIG__, " called!");
					return true;
				};
				Commander<CMD, cmd_fun_t > commander_;
				try {
					commander_.reg(CMD::red, paint_it_red);
					bool r = commander_.execute(CMD::red);
				}
				catch (dbj::Exception & x) {
					dbj::print(x);
				}
			}
		}
	} // cmd
} // dbj
#endif