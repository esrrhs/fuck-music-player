#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>

#pragma comment(lib, "boost.lib")

MainThread()
{
}
~MainThread()
{
}
void MainThread::ini()
{
	LOG_TRACE("MainThread", "ini Main Thread...");
	LOG_TRACE("MainThread", "ini gui...");

	LOG_TRACE("MainThread", "ini gui ok");
	boost::thread thrd(boost::bind(&MainThread::run, this));
	LOG_TRACE("MainThread", "ini Main Thread ok");
}
void MainThread::run()
{
	LOG_TRACE("MainThread", "MainThread start...");
	LOG_TRACE("MainThread", "MainThread start ok");
}

