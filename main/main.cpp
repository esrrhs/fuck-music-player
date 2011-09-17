#include "Logger.h"

#include "main.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/lexical_cast.hpp>
#include "god/god.h"
#include "localheader.h"
#include <boost/pool/singleton_pool.hpp>

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

#ifdef WIN32
HWND		g_hwnd = NULL;
#else
#error "unknown HWND implementation"
#endif

PluginSys::PluginContainer * g_pc = NULL;
PluginSys::Plugin * g_Plugin = NULL;

extern "C" MAIN_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);

	STRING fl = config.Get(PLUGIN_CONTAINER_CONFIG_FILE_LIST_KEY);
	STRING nl = config.Get(PLUGIN_CONTAINER_CONFIG_NAME_LIST_KEY);
	g_pc = new PluginSys::PluginContainer(fl, nl);
	g_pc->AddFather(p);

	// load fps
	STRING fps = config.Get(PLUGIN_MAIN_FPS_CONFIG_NAME);
	u32 ufps = boost::lexical_cast<u32>(fps.c_str());
	SINGLETON(God).SetFps(ufps);

	g_pc->Ini();

	g_Plugin = p;

	return true;
}
extern "C" MAIN_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	g_pc->Quit();
	delete g_pc;
	g_pc = 0;
	return true;
}

extern "C" MAIN_API bool PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return g_hwnd != 0;
	}
	return false;
}
extern "C" MAIN_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return g_hwnd != 0;
	}
	return false;
}
extern "C" MAIN_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		g_hwnd = *((HWND*)param);
		return true;
	}
	return false;
}
void WaitGetHwnd()
{
	LOG_ENTER
	while (!g_hwnd)
	{
		g_Plugin->GetOther((void*)PI_GS_UI_WIN_HANDLE, (void*)&g_hwnd);
		boost::system_time t = boost::get_system_time();
		t += boost::posix_time::microseconds((boost::int64_t)1000);
		boost::thread::sleep(t);
	}
	SINGLETON(God).SetHwnd((void*)g_hwnd);
	LOG_LEAVE
}
extern "C" MAIN_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	LOG_ENTER
	WaitGetHwnd();
	SINGLETON(God).Ini();
	SINGLETON(God).Loop();
	SINGLETON(God).Quit();
	LOG_LEAVE
	return true;
}