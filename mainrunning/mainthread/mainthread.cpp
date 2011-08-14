#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>
#include "uimng.h"
#include "globle.h"
#include <boost/timer.hpp>

MainThread::MainThread()
{
}
MainThread::~MainThread()
{
}
void MainThread::ini()
{
	LOG_ENTER;
	SINGLETON(UIMng).ini();
	boost::thread thrd(boost::bind(&MainThread::run, this));
	LOG_LEAVE;
}
void MainThread::run()
{
	LOG_ENTER;
	boost::timer tm;
	while (!SINGLETON(Globle).m_quit)
	{
		double elapsed = tm.elapsed();
		if (elapsed < TPS)
		{
			boost::system_time t = boost::get_system_time();
			t += boost::posix_time::microseconds((TPS - elapsed) * 1000);
			boost::thread::sleep(t);
		}
		else
		{
			tm.restart();
			heartbeat(elapsed);
		}
	}
	LOG_LEAVE;
}
void MainThread::heartbeat(double elapsed)
{
	SINGLETON(UIMng).heartbeat(elapsed);
	SINGLETON(UIMng).render();
}

