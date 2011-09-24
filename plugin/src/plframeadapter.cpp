#include "plframeadapter.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include "uimsg.pb.h"

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
HWND g_hwnd = NULL;
#else
#error "unknown HWND implementation"
#endif

PluginSys::PluginContainer * g_pc = NULL;
void * g_zmq_socket = NULL;
void * g_zmq_ui_ctx = NULL;
c8 g_zeromq_ok = 0; 

#include "zmq.h"
#include "zmq_utils.h"
#include "platform.hpp"
void ini_zmq()
{
	void * ctx = zmq_init(1);
	if (!ctx) 
	{
		return;
	}

	void * s = zmq_socket(ctx, ZMQ_PUSH);
	if (!s) 
	{
		return;
	}

	s32 rc = zmq_bind(s, UI_ZMQ_NAME);
	if (rc) 
	{
		return;
	}

	g_zmq_socket = s;
	g_zmq_ui_ctx = ctx;
}
void send_zmq_msg(const ui::uimsg & msg)
{
	s32 size = msg.ByteSize();
	zmq_msg_t zmsg;
	s32 rc = zmq_msg_init_size(&zmsg, size);
	if (rc)
	{
		return;
	}
	void * buffer = zmq_msg_data(&zmsg);
	msg.SerializeToArray(buffer, size);

	rc = zmq_send (g_zmq_socket, &zmsg, 0);
	if (rc)
	{
		return;
	}
}
extern "C" FRAMEADAPTER_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	STRING fl = config.Get(PLUGIN_CONTAINER_CONFIG_FILE_LIST_KEY);
	STRING nl = config.Get(PLUGIN_CONTAINER_CONFIG_NAME_LIST_KEY);
	g_pc = new PluginSys::PluginContainer(fl, nl);
	g_pc->AddFather(p);

	ini_zmq();
	g_pc->Ini();
	g_pc->Set((void*)PI_GS_ZEROMQ_CTX, (void*)&g_zmq_ui_ctx);

	return true;
}
extern "C" FRAMEADAPTER_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	g_pc->Quit();
	delete g_pc;
	g_pc = 0;
	return true;
}
extern "C" FRAMEADAPTER_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInInputType t = (PluginInInputType)(s32)type;
	switch (t)
	{
	case PI_I_UI_MSG:
		{
			if (!g_zeromq_ok)
			{
				g_pc->Get((void*)PI_GS_ZEROMQ_OK_FLG, (void*)&g_zeromq_ok);
				return PLUGIN_HANDLE_INPUT_END;
			}
			ui::uimsg * msg = (ui::uimsg *)param;
			send_zmq_msg(*msg);
			return PLUGIN_HANDLE_INPUT_END;
		}
		break;
	}
	return PLUGIN_HANDLE_INPUT_CONTINUE;
}
extern "C" FRAMEADAPTER_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return true;
	case PI_GS_ZEROMQ_CTX:
		*((void**)param) = g_zmq_ui_ctx;
		return true;;
	default:
		return g_pc->Get(type, param);
	}
	return false;
}
extern "C" FRAMEADAPTER_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
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
	default:
		return g_pc->Set(type, param);
	}
	return false;
}
