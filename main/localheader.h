#ifndef __LOCAL_MAIN_HEADER__
#define __LOCAL_MAIN_HEADER__

#define SINGLETON(t) boost::details::pool::singleton_default<t>::instance()

#define LOG_TRACE(x) SINGLETON(Logger).log("trace.txt", __FUNCTION__, x);
#define LOG_ERROR(x) SINGLETON(Logger).log("error.txt", __FUNCTION__, x);
#define LOG_ENTER LOG_TRACE("begin...")
#define LOG_LEAVE LOG_TRACE("end")

#define FILE_FINDER_PLUGIN_TYPE L"filefinder"
#define MUSIC_ENGINE_PLUGIN_TYPE L"musicengine"

#endif
