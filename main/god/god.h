#ifndef __GOD__
#define __GOD__

#include <map>
#include "globledefine.h"

class God
{
public:
	bool Ini();
	bool Loop();
	bool Quit();
	void * GetHwnd()
	{
		return m_hwnd;
	}
	void SetHwnd(void * hwnd)
	{
		m_hwnd = hwnd;
	}
	void * GetZeromqCtx()
	{
		return m_zeromq_ctx;
	}
	void SetZeromqCtx(void * zeromq_ctx)
	{
		m_zeromq_ctx = zeromq_ctx;
	}
	u32 GetFps()
	{
		return m_fps;
	}
	void SetFps(u32 fps)
	{
		m_fps = fps;
	}
	void * GetZeromqSocket()
	{
		return m_zeromq_socket;
	}
	void SetZeromqSocket(void * zeromq_socket)
	{
		m_zeromq_socket = zeromq_socket;
	}
	void GetPluginName(const STRING & plugintype, STRING & filename, STRING & name)
	{
		filename = m_plugin_name[plugintype].first;
		name = m_plugin_name[plugintype].second;
	}
	void SetPluginName(const STRING & plugintype, const STRING & filename, const STRING & name)
	{
		m_plugin_name[plugintype] = std::make_pair(filename, name);
	}
private:
	bool Hearbeat(double elapsed);
private:
	void * m_hwnd;
	void * m_zeromq_ctx;
	void * m_zeromq_socket;
	std::map<STRING, std::pair<STRING, STRING> > m_plugin_name;
	u32 m_fps;
};

#endif	