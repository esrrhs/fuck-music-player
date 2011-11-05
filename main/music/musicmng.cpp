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
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>

MusicMng::MusicMng() : m_filefinder(NULL), m_musicengine(NULL)
{
}
MusicMng::~MusicMng()
{
}
void MusicMng::ini()
{
	LOG_ENTER;

	ini_music_engine();

	load_file_finder();
	start_load_music_list();

	LOG_LEAVE;
}
void MusicMng::ini_music_engine()
{
	LOG_ENTER;

	STRING filename, name;
	SINGLETON(God).GetPluginName(MUSIC_ENGINE_PLUGIN_TYPE, filename, name);

	PluginSys::Plugin * musicengine = new PluginSys::Plugin(filename, name);

	if (!musicengine->IsOK())
	{
		LOG_ERROR("m_musicengine Is not OK");
		SAFE_DELETE(musicengine);
	}

	musicengine->Ini();

	m_musicengine = musicengine;

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
	((PluginSys::Plugin*)m_musicengine)->Run();
}
void MusicMng::play_music(s32 pos)
{
	if (pos >= 0 && pos < (s32)m_list.size())
	{
#ifdef UNICODE
		std::string str = Helper::Utf16ToACP(m_list[pos]);
		const c8 * name = str.c_str();
#else
		const c8 * name = m_list[pos].c_str();
#endif
		play_music(name);
	}
}
void MusicMng::play_music(const c8 * name)
{
	((PluginSys::Plugin*)m_musicengine)->Input((void*)PI_I_MUSIC_ENGINE_PLAY_MUSIC, (void*)name);
}
void RunFileFinder()
{
	PluginSys::Plugin * m_filefinder = (PluginSys::Plugin*)SINGLETON(MusicMng).GetFileFinder();
	m_filefinder->Run();

	SINGLETON(MusicMng).end_load_music_list();
}
void MusicMng::start_load_music_list()
{
	LOG_ENTER;

	LOG_TRACE("start find thread");

	SINGLETON(God).SetStauts(God::GS_FIND);

	// 启动thread
	boost::thread fd(boost::bind(RunFileFinder));

	LOG_LEAVE;
}
void MusicMng::parse_from_find_list()
{
	LOG_ENTER;

	std::vector<STRING> * pvec;
	((PluginSys::Plugin*)m_filefinder)->Get((void*)PI_GS_FILE_FINDER_RESULT_LIST, (void*)&pvec);

	for (size_t i = 0; i < pvec->size(); i++)
	{
		STRING tmp = (*pvec)[i];
		m_list.push_back(tmp);
	}

	LOG_TRACE("clear tmp");
	((PluginSys::Plugin*)m_filefinder)->Input((void*)PI_I_FILE_FINDER_CLEAR_RESULT_LIST, NULL);

	LOG_LEAVE;
}
void MusicMng::end_load_music_list()
{
	SINGLETON(God).SetStauts(God::GS_FIND_OK);
}
const c8 * MusicMng::get_list_item_name(s32 pos)
{
	if (pos >= 0 && pos < (s32)m_list.size())
	{
		namespace fs = boost::filesystem;

#ifdef UNICODE
		STRING strPath = m_list[pos];
#else
		std::wstring strPath = Helper::Utf8ToUtf16(m_list[pos].c_str());
#endif
		fs::path rootPath(fs::initial_path());        // 初始化为本项目路径
		rootPath = fs::system_complete(fs::path(strPath, fs::native));    //将相对路径转换为绝对路径

#ifdef UNICODE
		static std::string tmp;
		tmp = Helper::Utf16ToUtf8(rootPath.leaf().c_str());
		return tmp.c_str();
#else
		static std::string tmp;
		tmp = rootPath.leaf().c_str();
		return tmp.c_str();
#endif
	}
	return "empty";
}

const c8 * MusicMng::get_find_dir_name()
{
	const c8 * p = NULL;
	((PluginSys::Plugin*)m_filefinder)->Get((void*)PI_GS_FILE_FINDER_FINDING_DIR, (void*)&p);
#ifdef UNICODE
	static std::string tmp;
	tmp = Helper::Utf16ToUtf8((wchar_t*)p);
	return tmp.c_str();
#else
	return p;
#endif
}
const c8 * MusicMng::get_find_name()
{
	const c8 * p = NULL;
	((PluginSys::Plugin*)m_filefinder)->Get((void*)PI_GS_FILE_FINDER_FINDING_NAME, (void*)&p);
#ifdef UNICODE
	static std::string tmp;
	tmp = Helper::Utf16ToUtf8((wchar_t*)p);
	return tmp.c_str();
#else
	return p;
#endif
}
const c8 * MusicMng::get_find_num()
{
	s32 num = 0;
	((PluginSys::Plugin*)m_filefinder)->Get((void*)PI_GS_FILE_FINDER_FIND_NUM, (void*)&num);

	static std::string tmp;
	tmp = boost::lexical_cast<std::string>(num);
	return tmp.c_str();
}
s32 MusicMng::get_list_item_num()
{
	return (s32)m_list.size();
}
void MusicMng::random_shuffle()
{
	std::random_shuffle(m_list.begin(), m_list.end());
}
void MusicMng::set_music_volume(f32 volume)
{
	((PluginSys::Plugin*)m_musicengine)->Input((void*)PI_I_MUSIC_ENGINE_SET_VOLUME, (void*)&volume);
}