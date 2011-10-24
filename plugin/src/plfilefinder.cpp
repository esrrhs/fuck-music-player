#include "plfilefinder.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

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
	return true;
}
extern "C" FILE_FINDER_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	return true;
}

extern "C" FILE_FINDER_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return PLUGIN_HANDLE_INPUT_CONTINUE;
}
extern "C" FILE_FINDER_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return false;
}
extern "C" FILE_FINDER_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return false;
}

extern "C" FILE_FINDER_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	namespace fs = boost::filesystem;

	STRING strPath = L"Z:\\Mymusic";

	fs::path rootPath(fs::initial_path());        // 初始化为本项目路径
	rootPath = fs::system_complete(fs::path(strPath, fs::native));    //将相对路径转换为绝对路径
	
	if (!fs::exists(rootPath))            //路径是否存在
	{
		return false;
	}
	if (!fs::is_directory(rootPath))        // 是否是目录
	{
		return false;
	}
	
	std::vector<fs::path> dirvec;
	dirvec.push_back(rootPath);
	do 
	{
		fs::path dir = dirvec.back();
		dirvec.pop_back();

		fs::directory_iterator end_iter;
		for (fs::directory_iterator file_itr(dir); file_itr != end_iter; ++file_itr)
		{
			if (fs::is_directory(*file_itr))
			{
				dirvec.push_back(file_itr->path());
			}
			else if (fs::extension(*file_itr)==".mp3")        // 文件后缀
			{
	
			}
		}
	} 
	while (!dirvec.empty());

	return true;
}