#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>

#pragma comment(lib, "boost.lib")

void MainThread::ini()
{
	LOG_TRACE("ini Main Thread...");
	boost::thread thrd(boost::bind(&MainThread::run, this));
	LOG_TRACE("ini Main Thread ok");
}
void MainThread::run()
{
	LOG_TRACE("MainThread start...");
	LOG_TRACE("MainThread start ok");
}

