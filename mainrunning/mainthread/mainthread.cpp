#include "mainthread.h"
#include <boost/thread/thread.hpp>
#include "Logger.h"
#include <boost/pool/detail/singleton.hpp>
#include "uimng.h"

MainThread::MainThread() : d_lastFrameTime(GetTickCount())	// TODO ¡Ÿ ±
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
	while (1)
	{
		DWORD thisTime = GetTickCount();
		float elapsed = static_cast<float>(thisTime - d_lastFrameTime) / 1000.0f;
		d_lastFrameTime = thisTime;

		heartbeat(elapsed);

		Sleep(50);
	}
	LOG_LEAVE;
}
void MainThread::heartbeat(float elapsed)
{
	SINGLETON(UIMng).heartbeat(elapsed);
	SINGLETON(UIMng).render();
}

