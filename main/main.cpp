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

// ���� SDKDDKVer.h ��������õ���߰汾�� Windows ƽ̨��

// ���ҪΪ��ǰ�� Windows ƽ̨����Ӧ�ó�������� WinSDKVer.h������
// WIN32_WINNT ������ΪҪ֧�ֵ�ƽ̨��Ȼ���ٰ��� SDKDDKVer.h��

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
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

extern "C" MAIN_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);

	STRING fl = config.Get(PLUGIN_CONTAINER_CONFIG_FILE_LIST_KEY);
	STRING nl = config.Get(PLUGIN_CONTAINER_CONFIG_NAME_LIST_KEY);
	g_pc = new PluginSys::PluginContainer(fl, nl);
	g_pc->AddFather(p);

	g_pc->Ini();

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
	return g_pc->Input(type, param);
}
extern "C" MAIN_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Get(type, param);
}
extern "C" MAIN_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Set(type, param);
}
extern "C" MAIN_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	LOG_ENTER
	God god;
	god.Ini();
	god.Loop();
	god.Quit();
	LOG_LEAVE
	return true;
}