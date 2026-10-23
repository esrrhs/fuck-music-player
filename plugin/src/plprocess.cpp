#include "plprocess.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <stdio.h>

PluginSys::PluginContainer * g_pc = NULL;

extern "C" PROCESS_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
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
extern "C" PROCESS_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	//g_pc->Quit();
	delete g_pc;
	g_pc = 0;
	return true;
}

extern "C" PROCESS_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Input(type, param);
}
extern "C" PROCESS_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Get(type, param);
}
extern "C" PROCESS_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return g_pc->Set(type, param);
}

extern "C" PROCESS_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	while (g_pc->size() > 0)
	{
		PluginSys::PluginContainer::ConstIter it = g_pc->begin();
		if (!(*it)->IsOK())
		{
			// LoadLibrary failed (missing DLL / deps) — do not pretend Run succeeded.
			FILE * bf = fopen("d3d-init.txt", "a");
			if (bf) { fprintf(bf, "process: plugin LoadLibrary FAILED (IsOK=false)\n"); fclose(bf); }
			return false;
		}
		if(!(*it)->Run())
		{
			return false;
		}
		if(!(*it)->Quit())
		{
			return false;
		}
		delete (*it);
		g_pc->erase(it);
	}
	return false;
}