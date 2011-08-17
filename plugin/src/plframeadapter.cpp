#include "plframeadapter.h"
#include "plugin.h"

#ifdef WIN32
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

extern "C" FRAME_ADAPTER_EXPORTS PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{

}
extern "C" FRAME_ADAPTER_EXPORTS PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{

}