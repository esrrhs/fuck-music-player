#ifndef __GLOBLE_DEFINE__
#define __GLOBLE_DEFINE__

#define SINGLETON(t) boost::details::pool::singleton_default<t>::instance()

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define LOG_TRACE(x) SINGLETON(Logger).log("trace.txt", __FUNCTION__, x);
#define LOG_ERROR(x) SINGLETON(Logger).log("error.txt", __FUNCTION__, x);
#define LOG_ENTER LOG_TRACE("begin...")
#define LOG_LEAVE LOG_TRACE("end")

typedef int s32;
typedef unsigned u32;
typedef char c8;

#define UI_ZMQ_NAME "inproc://ui_msg"

#define FPS 50
#define TPS (1.f / 50)

#ifdef UNICODE
#define ZCHAR wchar_t
#define STRING std::wstring
#define IFSTREAM std::wifstream
#else
#define ZCHAR char
#define STRING std::string
#define IFSTREAM std::ifstream
#endif

#endif