#include "mainthread.h"
#include <boost/thread/thread.hpp>

#pragma comment(lib, "boost.lib")

void MainThread::ini()
{
	boost::thread thrd(boost::bind(&MainThread::run, this));
}
void MainThread::run()
{
}

