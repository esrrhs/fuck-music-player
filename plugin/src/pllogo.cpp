#include "pllogo.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"

#ifdef WIN32
HWND g_hwnd = NULL;
#else
#error "unknown HWND implementation"
#endif

bool IniLogo(const STRING & logofilename)
{
	return true;
}
extern "C" LOGO_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	
	STRING logo = config.Get(PLUGIN_LOG_FILE_CONFIG_NAME);
	
	return IniLogo(logo);
}
extern "C" LOGO_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	return true;
}

extern "C" LOGO_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_UI_WIN_HANDLE:
		*((HWND*)param) = g_hwnd;
		return true;
	}
	return false;
}
extern "C" LOGO_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
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
extern "C" LOGO_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	return true;
}