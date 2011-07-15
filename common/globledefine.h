#ifndef __GLOBLE_DEFINE__
#define __GLOBLE_DEFINE__

#define SINGLETON(t) boost::details::pool::singleton_default<t>::instance()

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define LOG_TRACE(x) SINGLETON(Logger).log("trace.txt", "trace", x);

#endif