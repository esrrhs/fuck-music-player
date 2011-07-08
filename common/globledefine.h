#ifndef __GLOBLE_DEFINE__
#define __GLOBLE_DEFINE__

#define SINGLETON(t) boost::details::pool::singleton_default<t>::instance()

#endif