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

void * g_zmq_socket = NULL;
void * g_zmq_ui_ctx = NULL;
c8 g_zeromq_ok = 0; 

#include "zmq.h"
#include "zmq_utils.h"
#include "platform.hpp"

void ini_zmq()
{
	LOG_ENTER;

	void * ctx_ = g_zmq_ui_ctx;
	if (!ctx_)
	{
		LOG_ERROR("ctx_");
		return;
	}

	void * s = zmq_socket(ctx_, ZMQ_PULL);
	if (!s) 
	{
		LOG_ERROR("zmq_socket");
		return;
	}

	s32 rc = zmq_connect (s, UI_ZMQ_NAME);
	if (rc != 0) 
	{
		LOG_ERROR("zmq_connect");
		return;
	}

	g_zmq_socket = s;

	g_zeromq_ok = 1;

	LOG_LEAVE;
}
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

	// load filefinder
	STRING filefinder_filename = config.Get(PLUGIN_MAIN_FILEFINDER_FILENAME_CONFIG_NAME);
	STRING filefinder_name = config.Get(PLUGIN_MAIN_FILEFINDER_NAME_CONFIG_NAME);
	SINGLETON(God).SetPluginName(FILE_FINDER_PLUGIN_TYPE, filefinder_filename, filefinder_name);

	// load musicengine
	STRING musicengine_filename = config.Get(PLUGIN_MAIN_MUSICENGINE_FILENAME_CONFIG_NAME);
	STRING musicengine_name = config.Get(PLUGIN_MAIN_MUSICENGINE_NAME_CONFIG_NAME);
	SINGLETON(God).SetPluginName(MUSIC_ENGINE_PLUGIN_TYPE, musicengine_filename, musicengine_name);

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

extern "C" MAIN_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return PLUGIN_HANDLE_INPUT_END;
}
extern "C" MAIN_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return g_hwnd != 0;
	case PI_GS_ZEROMQ_CTX:
		*((void**)param) = g_zmq_ui_ctx;
		return g_zmq_ui_ctx != 0;
	case PI_GS_ZEROMQ_OK_FLG:
		*((c8*)param) = g_zeromq_ok;
		return true;
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
	case PI_GS_ZEROMQ_CTX:
		g_zmq_ui_ctx = *((void**)param);
		return true;
	case PI_GS_ZEROMQ_OK_FLG:
		g_zeromq_ok = *((c8*)param);
		return true;
	}
	return false;
}
void WaitGetHwnd()
{
	LOG_ENTER
	while (!g_hwnd || !g_zmq_ui_ctx)
	{
		g_Plugin->GetOther((void*)PI_GS_UI_WIN_HANDLE, (void*)&g_hwnd);
		g_Plugin->GetOther((void*)PI_GS_ZEROMQ_CTX, (void*)&g_zmq_ui_ctx);
		SLEEP(1);
	}
	SINGLETON(God).SetHwnd((void*)g_hwnd);
	SINGLETON(God).SetZeromqCtx((void*)g_zmq_ui_ctx);
	SINGLETON(God).SetZeromqSocket((void*)g_zmq_socket);
	LOG_LEAVE
}
extern "C" MAIN_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	LOG_ENTER
	ini_zmq();
	WaitGetHwnd();
	SINGLETON(God).Ini();
	SINGLETON(God).Loop();
	SINGLETON(God).Quit();
	LOG_LEAVE
	return true;
}