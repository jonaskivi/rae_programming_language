#include <iostream>
#if __cplusplus >= 201103L //c++11
	#include <mutex>

#include <stdarg.h>

namespace rae
{

std::ostream&
log_one(std::ostream& os)
{
    return os;
}

template <class A0, class ...Args>
std::ostream&
log_one(std::ostream& os, const A0& a0, const Args& ...args)
{
    os << a0;
    return log_one(os, args...);
}

template <class ...Args>
std::ostream&
log(std::ostream& os, const Args& ...args)
{
    return log_one(os, args...);
}

template <class ...Args>
std::ostream&
log(const Args& ...args)
{
	  static std::mutex m;
    std::lock_guard<std::mutex> _(m);
    return log(std::cout, args...);
}

}//end namespace rae

#else

namespace rae
{
void log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == 'd') {
            int i = va_arg(args, int);
            std::cout << i << '\n';
        } else if (*fmt == 's') {
            char * s = va_arg(args, char*);
            std::cout << s << '\n';
        }
        ++fmt;
    }

    va_end(args);
}


}

#endif



