# Curently unused

Reason: dbj++ console dealing are all done with dbj::console part, which in turn is based 
on the need to outpuit UTF-8 normalu and safely. That in turn introduces self imposed limitation of not using iostreams, and 
not using global printf, wprintf familly. 

All of this is/was due to UCRT dramatic changes in and arround stdio.h and especially UCRT 
not handling clearly the daner of using __setmode() and then accidenrly or not tryhing to use printf 
which leads to crash.

To cut the long story short always use dbj::console::print() and dbj::console::prinf()
To overload dbj::console::out() for your classes and a such best course of action is to
implement them as inline friend

```c
struct my_type {
  friend void out ( const & my_type mt_) {
  }
}
```
