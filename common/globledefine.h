#ifndef __GLOBLE_DEFINE__
#define __GLOBLE_DEFINE__

#define SINGLETON(t) boost::details::pool::singleton_default<t>::instance()

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define LOG_TRACE(tag, x) SINGLETON(Logger).log("trace.txt", tag, x);
#define LOG_ERROR(tag, x) SINGLETON(Logger).log("error.txt", tag, x);

typedef int s32;

#endif