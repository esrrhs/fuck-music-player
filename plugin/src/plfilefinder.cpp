#include "plfilefinder.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

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

	fs::path rootPath(fs::initial_path());        // ��ʼ��Ϊ����Ŀ·��
	rootPath = fs::system_complete(fs::path(strPath, fs::native));    //�����·��ת��Ϊ����·��
	
	if (!fs::exists(rootPath))            //·���Ƿ����
	{
		return false;
	}
	if (!fs::is_directory(rootPath))        // �Ƿ���Ŀ¼
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
			else if (fs::extension(*file_itr)==".mp3")        // �ļ���׺
			{
	
			}
		}
	} 
	while (!dirvec.empty());

	return true;
}