#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>
#include "uimng.h"

MainThread::MainThread()
{
}
MainThread::~MainThread()
{
}
void MainThread::ini()
{
	LOG_TRACE("MainThread", "ini...");
	SINGLETON(UIMng).ini();
	boost::thread thrd(boost::bind(&MainThread::run, this));
	LOG_TRACE("MainThread", "ini ok");
}
void MainThread::run()
{
	LOG_TRACE("MainThread", "start...");
	LOG_TRACE("MainThread", "start ok");
}

