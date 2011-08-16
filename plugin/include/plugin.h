#ifndef __PLUG_IN__
#define __PLUG_IN__

#include "globledefine.h"
#include <string>
#include "plheader.h"

#ifdef WIN32
	#define PLUGIN_HANDLE HMODULE
	#define PLUGIN_LOAD(a) LoadLibrary(a)
	#define PLUGIN_GETSYM(a, b) GetProcAddress(a, b)
	#define PLUGIN_UNLOAD(a) FreeLibrary(a)
	#include <windows.h>
#else
	#error "unknown Plugin implementation"
#endif

namespace PluginSys
{

	class Plugin
	{
	public:
		typedef bool (*IniFunc) (void);
		typedef bool (*RunFunc) (void);
		typedef bool (*InputFunc) (void*);
		Plugin(const STRING & filename, const STRING & name, 
			const STRING & inistr = PLUGIN_INI_FUNC_DEFAAULT_NAME, 
			const STRING & runstr = PLUGIN_RUN_FUNC_DEFAAULT_NAME, 
			const STRING & inputstr = PLUGIN_INPUT_FUNC_DEFAAULT_NAME) : m_filename(filename), 
			m_name(name), m_handle(0)
		{
			m_handle = PLUGIN_LOAD(filename.c_str());
			if (m_handle)
			{
#if defined UNICODE && defined WIN32
				const s32 maxProcName = 256;
				c8 temp[maxProcName];

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, inistr.c_str(), -1, temp, maxProcName, NULL, 0); 
				m_inifunc = (IniFunc)PLUGIN_GETSYM(m_handle, temp);

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, runstr.c_str(), -1, temp, maxProcName, NULL, 0);
				m_runfunc = (RunFunc)PLUGIN_GETSYM(m_handle, temp); 

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, inputstr.c_str(), -1, temp, maxProcName, NULL, 0); 
				m_inputfunc = (InputFunc)PLUGIN_GETSYM(m_handle, temp);
#endif
			}
		}
		~Plugin()
		{
			PLUGIN_UNLOAD(m_handle);
		}
		bool IsOK() const
		{
			return m_handle != 0;
		}
		bool Ini()
		{
			return m_inifunc();
		}
		bool Run()
		{
			return m_runfunc();
		}
		bool Input(void * param)
		{
			return m_inputfunc(param);
		}
	private:
		STRING m_filename, m_name;
		PLUGIN_HANDLE m_handle;
		IniFunc m_inifunc;
		RunFunc m_runfunc;
		InputFunc m_inputfunc;
	};

}
#endif
