#include "musicmng.h"
#include "Logger.h"
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "../localheader.h"
#include "../helper/helper.h"
#include "../god/god.h"
#include "plugin.h"
#include "plmsgheader.h"

#include <boost/filesystem.hpp>

MusicMng::MusicMng() : m_filefinder(NULL)
{
}
MusicMng::~MusicMng()
{
}
void MusicMng::ini()
{
	LOG_ENTER;

	load_file_finder();
	start_load_music_list();

	LOG_LEAVE;
}
void MusicMng::load_file_finder()
{
	LOG_ENTER;

	STRING filename, name;
	SINGLETON(God).GetPluginName(FILE_FINDER_PLUGIN_TYPE, filename, name);

	PluginSys::Plugin * filefinder = new PluginSys::Plugin(filename, name);

	if (!filefinder->IsOK())
	{
		LOG_ERROR("m_filefinder Is not OK");
		SAFE_DELETE(filefinder);
	}

	filefinder->Ini();

	m_filefinder = filefinder;

	LOG_LEAVE;
}
void MusicMng::heartbeat(double elapsed)
{
}
void MusicMng::start_load_music_list()
{
	LOG_ENTER;

	LOG_TRACE("begin find");
	((PluginSys::Plugin*)m_filefinder)->Run();
	LOG_TRACE("end find");

	LOG_TRACE("start parse");
	std::vector<STRING> * pvec;
	((PluginSys::Plugin*)m_filefinder)->Get((void*)PI_GS_FILE_FINDER_RESULT_LIST, (void*)&pvec);

	for (size_t i = 0; i < pvec->size(); i++)
	{
		STRING tmp = (*pvec)[i];
#ifdef UNICODE
		m_list.push_back(new CEGUI::String(Helper::Utf16ToString(tmp.c_str())));
#else
		m_list.push_back(new CEGUI::String(tmp.c_str()));
#endif
	}

	LOG_TRACE("clear tmp");
	((PluginSys::Plugin*)m_filefinder)->Input((void*)PI_I_FILE_FINDER_CLEAR_RESULT_LIST, NULL);

	LOG_LEAVE;
}
void MusicMng::end_load_music_list()
{
	LOG_ENTER;

	LOG_LEAVE;
}
const c8 * MusicMng::get_list_item_name(s32 pos)
{
	if (pos >= 0 && pos < (s32)m_list.size())
	{
		namespace fs = boost::filesystem;
#ifdef UNICODE
		STRING strPath = Helper::Utf8ToUtf16(m_list[pos]->c_str());
#else
		STRING strPath = m_list[pos]->c_str();
#endif
		fs::path rootPath(fs::initial_path());        // 初始化为本项目路径
		rootPath = fs::system_complete(fs::path(strPath, fs::native));    //将相对路径转换为绝对路径

#ifdef UNICODE
		return Helper::Utf16ToString(rootPath.leaf().c_str()).c_str();
#else
		return p.leaf().c_str();
#endif
	}
	return "empty";
}
