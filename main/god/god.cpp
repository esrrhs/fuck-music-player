#include "Logger.h"
#include "god.h"
#include "../localheader.h"
#include <boost/pool/singleton_pool.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

#include "../ui/uimng.h"
#include "../render/render.h"
#include "../music/musicmng.h"

God::God() : m_status(GS_NONE), m_hwnd(NULL), m_zeromq_ctx(NULL), m_zeromq_socket(NULL), m_fps(50)
{

}
bool God::Ini()
{
	LOG_ENTER
	SetStauts(God::GS_START);
	SINGLETON(Render).Ini();
	SINGLETON(MusicMng).ini();
	SINGLETON(UIMng).ini();
	LOG_LEAVE
	return true;
}
bool God::Loop()
{
	LOG_ENTER
	boost::timer tm;
	double tps = 1.f / m_fps;
	while (1)
	{
		double elapsed = tm.elapsed();
		if (elapsed < tps)
		{
			SLEEP(1);
		}
		else
		{
			tm.restart();
			Hearbeat(elapsed);
		}
	}
	LOG_LEAVE
	return true;
}
bool God::Hearbeat(double elapsed)
{
	SINGLETON(Render).Begin();

	SINGLETON(MusicMng).heartbeat(elapsed);

	SINGLETON(UIMng).heartbeat(elapsed);
	SINGLETON(UIMng).render();

	SINGLETON(Render).End();
	return true;
}
bool God::Quit()
{
	LOG_ENTER
	LOG_LEAVE
	return true;
}