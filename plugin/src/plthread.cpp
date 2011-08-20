#include "plthread.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>

#ifdef WIN32

// 包括 SDKDDKVer.h 将定义可用的最高版本的 Windows 平台。

// 如果要为以前的 Windows 平台生成应用程序，请包括 WinSDKVer.h，并将
// WIN32_WINNT 宏设置为要支持的平台，然后再包括 SDKDDKVer.h。

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

PluginSys::PluginContainer * g_pc = NULL;

// TODO
void heartbeat(double elapsed)
{
	g_pc->Run();
}
// TODO
void Run()
{
	boost::timer tm;
	while (1)
	{
		double elapsed = tm.elapsed();
		if (elapsed < 1.f / 50)
		{
			boost::system_time t = boost::get_system_time();
			t += boost::posix_time::microseconds((boost::int64_t)(1.f / 50 - elapsed) * 1000);
			boost::thread::sleep(t);
		}
		else
		{
			tm.restart();
			heartbeat(elapsed);
		}
	}
}

extern "C" THREAD_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	STRING fl = config.Get(PLUGIN_CONTAINER_CONFIG_FILE_LIST_KEY);
	STRING nl = config.Get(PLUGIN_CONTAINER_CONFIG_NAME_LIST_KEY);
	g_pc = new PluginSys::PluginContainer(fl, nl);

	g_pc->Ini();

	// 启动thread
	boost::thread td(boost::bind(Run));

	return true;
}
extern "C" THREAD_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	g_pc->Quit();
	delete g_pc;
	g_pc = 0;
	return true;
}

extern "C" THREAD_API bool PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Input(type, param);
}
extern "C" THREAD_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Get(type, param);
}
extern "C" THREAD_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Set(type, param);
}
