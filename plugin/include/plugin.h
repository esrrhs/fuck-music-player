#ifndef __PLUG_IN__
#define __PLUG_IN__

#include "globledefine.h"
#include <string>
#include "plheader.h"
#include <vector>
#include <algorithm>

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
		typedef bool (*IniFunc) (Plugin*);
		typedef bool (*QuitFunc) (void);
		typedef bool (*RunFunc) (void);
		typedef PLUGIN_HANDLE_INPUT_STATUS (*InputFunc) (void*, void*);
		typedef bool (*GetFunc) (void*, void*);
		typedef bool (*SetFunc) (void*, void*);

		Plugin(const STRING & filename, const STRING & name) : m_filename(filename), 
			m_name(name), m_handle(0)
		{
			m_handle = PLUGIN_LOAD(filename.c_str());
			if (m_handle)
			{
#if defined UNICODE && defined WIN32
				const s32 maxProcName = PLUGIN_MAX_FUNC_NAME_LENGTH;
				c8 temp[maxProcName];

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_INI_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0); 
				m_inifunc = (IniFunc)PLUGIN_GETSYM(m_handle, temp);

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_QUIT_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0); 
				m_quitfunc = (QuitFunc)PLUGIN_GETSYM(m_handle, temp);

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_RUN_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0);
				m_runfunc = (RunFunc)PLUGIN_GETSYM(m_handle, temp); 

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_INPUT_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0); 
				m_inputfunc = (InputFunc)PLUGIN_GETSYM(m_handle, temp);

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_GET_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0); 
				m_getfunc = (GetFunc)PLUGIN_GETSYM(m_handle, temp);

				memset(temp, 0, sizeof(c8) * maxProcName);
				WideCharToMultiByte(CP_ACP, 0, PLUGIN_SET_FUNC_DEFAAULT_NAME_STR, -1, temp, maxProcName, NULL, 0); 
				m_setfunc = (SetFunc)PLUGIN_GETSYM(m_handle, temp);
#endif
			}
		}
		~Plugin()
		{
			PLUGIN_UNLOAD(m_handle);
		}
		const ZCHAR * name() const
		{
			return m_name.c_str();
		}
		bool IsOK() const
		{
			return m_handle != 0;
		}
		bool Ini()
		{
			if (m_inifunc)
			{
				return m_inifunc(this);
			}
			return true;
		}
		bool Quit()
		{
			if (m_quitfunc)
			{
				return m_quitfunc();
			}
			return true;
		}
		bool Run()
		{
			if (m_runfunc)
			{
				return m_runfunc();
			}
			return true;
		}
		PLUGIN_HANDLE_INPUT_STATUS Input(void * type, void * param)
		{
			if (m_inputfunc)
			{
				return m_inputfunc(type, param);
			}
			return PLUGIN_HANDLE_INPUT_CONTINUE;
		}
		bool Get(void * type, void * param)
		{
			if (m_getfunc)
			{
				return m_getfunc(type, param);
			}
			return true;
		}
		bool Set(void * type, void * param)
		{
			if (m_setfunc)
			{
				return m_setfunc(type, param);
			}
			return true;
		}
		bool AddSon(Plugin* p)
		{
			m_son.push_back(p);
		}
		bool DelSon(Plugin* p)
		{
			SonContainer::iterator it = std::find(m_son.begin(), m_son.end(), p);
			if (it != m_son.end())
			{
				m_son.erase(it);
			}
		}
		// TODO 遍历其他节点
		// GetOther
	private:
		STRING m_filename, m_name;
		PLUGIN_HANDLE m_handle;
		IniFunc m_inifunc;
		QuitFunc m_quitfunc;
		RunFunc m_runfunc;
		InputFunc m_inputfunc;
		GetFunc m_getfunc;
		SetFunc m_setfunc;
		// 儿子节点
		typedef std::vector<Plugin*> SonContainer;
		SonContainer m_son;
	};

}
#endif
