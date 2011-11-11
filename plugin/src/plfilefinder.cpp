#include "plfilefinder.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <omp.h>
#include <set>

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

std::vector<STRING> g_dir;
std::vector<STRING> g_filter;
std::vector<STRING> g_find;
STRING g_finding_dir;
STRING g_finding_name;
s32 g_finding_num;

typedef boost::mutex MUTEX;
typedef boost::mutex::scoped_lock LOCK;
MUTEX g_get_mutex;

u32 g_sleep_time = 0;
u32 g_thread_num = 2;

std::wstring Utf8ToUtf16(const std::string& utf8text)
{
	const int textLen = MultiByteToWideChar(CP_UTF8, 0, utf8text.c_str(),
		utf8text.size(), 0, 0);

	if (textLen == 0)
		return L"";

	std::wstring wideStr(textLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8text.c_str(), utf8text.size(),
		&wideStr[0], wideStr.size());
	return wideStr;
}

extern "C" FILE_FINDER_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	
	STRING dir = config.Get(PLUGIN_FILE_FINDER_DIR_CONFIG_NAME);
	STRING filter = config.Get(PLUGIN_FILE_FINDER_FILTER_CONFIG_NAME);

	boost::char_separator<ZCHAR> sep(L";");
	typedef boost::tokenizer<boost::char_separator<ZCHAR>, STRING::const_iterator, STRING> Token;
	Token dir_tok(dir, sep);
	Token filter_tok(filter, sep);
	for (Token::iterator d_it = dir_tok.begin(); 
		d_it != dir_tok.end();
		d_it++)
	{
		g_dir.push_back(*d_it);
	}
	for (Token::iterator f_it = filter_tok.begin(); 
		f_it != filter_tok.end();
		f_it++)
	{
		g_filter.push_back(*f_it);
	}

	STRING sleep_time = config.Get(PLUGIN_FILE_FINDER_SLEEP_TIME_CONFIG_NAME);
	g_sleep_time = boost::lexical_cast<u32>(sleep_time.c_str());

	STRING thread_num = config.Get(PLUGIN_FILE_FINDER_THREAD_NUM_CONFIG_NAME);
	g_thread_num = boost::lexical_cast<u32>(thread_num.c_str());
	omp_set_num_threads(g_thread_num);

	return true;
}
extern "C" FILE_FINDER_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	return true;
}

extern "C" FILE_FINDER_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInInputType t = (PluginInInputType)(s32)type;
	switch (t)
	{
	case PI_I_FILE_FINDER_CLEAR_RESULT_LIST:
		{
			g_find.clear();
			return PLUGIN_HANDLE_INPUT_END;
		}
		break;
	}
	return PLUGIN_HANDLE_INPUT_CONTINUE;
}
extern "C" FILE_FINDER_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	LOCK lock(g_get_mutex);
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_FILE_FINDER_RESULT_LIST:
		*((std::vector<STRING>**)param) = &g_find;
		return true;
	case PI_GS_FILE_FINDER_FINDING_DIR:
		*((c8 **)param) = (c8*)g_finding_dir.c_str();
		return true;
	case PI_GS_FILE_FINDER_FIND_NUM:
		*((s32 *)param) = g_finding_num;
		return true;
	case PI_GS_FILE_FINDER_FINDING_NAME:
		*((c8 **)param) = (c8*)g_finding_name.c_str();
		return true;
	}
	return false;
}
extern "C" FILE_FINDER_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return false;
}

extern "C" FILE_FINDER_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	namespace fs = boost::filesystem;

	g_find.clear();

	std::vector<fs::path> dirvec;

	// 加载所有根界面
	for (size_t i = 0 ; i < g_dir.size(); i++)
	{
		STRING strPath = g_dir[i];

		fs::path rootPath(fs::initial_path());        // 初始化为本项目路径
		rootPath = fs::system_complete(fs::path(strPath, fs::native));    //将相对路径转换为绝对路径

		if (!fs::exists(rootPath))            //路径是否存在
		{
			continue;
		}
		if (!fs::is_directory(rootPath))        // 是否是目录
		{
			continue;
		}

		dirvec.push_back(rootPath);
	}

	std::set<int> id_set;

#pragma omp parallel
	id_set.insert(omp_get_thread_num());
	u32 size = id_set.size();
	// 开始查找
#pragma omp parallel shared(id_set)
	while (id_set.size() > 0)
	{
		fs::path dir;
#pragma omp critical(DIR_VEC)
		if (!dirvec.empty())
		{
			dir = dirvec.back();
			dirvec.pop_back();
			id_set.insert(omp_get_thread_num());
		}
		else
		{
			id_set.erase(omp_get_thread_num());
		}

		// empty...wait
		if (dir.empty())
		{
			continue;
		}
		
		{
			LOCK lock(g_get_mutex);
			g_finding_dir = dir.c_str();
		}

		fs::directory_iterator end_iter;
		for (fs::directory_iterator file_itr(dir); file_itr != end_iter; ++file_itr)
		{
			if (fs::is_directory(*file_itr))
			{
#pragma omp critical(DIR_VEC)
				dirvec.push_back(file_itr->path());
			}
			else
			{
				std::string ex = fs::extension(*file_itr);
				boost::algorithm::to_lower(ex);
#ifdef UNICODE				
				STRING last = Utf8ToUtf16(ex);
#else
				STRING last = ex;
#endif

				if (std::find(g_filter.begin(), g_filter.end(), last) != g_filter.end())
				{
#pragma omp critical(PUSH_FIND_LIST)
					// find it
					g_find.push_back(file_itr->path().c_str());
					g_finding_num = g_find.size();
				}
				{
					LOCK lock(g_get_mutex);
					g_finding_name = file_itr->path().filename().c_str();
				}
			}
			if (g_sleep_time > 0)
			{
				SLEEP(g_sleep_time);
			}
		}
	} 
	

	return true;
}