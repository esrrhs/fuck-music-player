#include "musicmng.h"
#include "Logger.h"
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "../localheader.h"
#include "../helper/helper.h"

MusicMng::MusicMng()
{
}
MusicMng::~MusicMng()
{
}
void MusicMng::ini()
{
	LOG_ENTER;

	start_load_music_list();

	LOG_LEAVE;
}
void MusicMng::heartbeat(double elapsed)
{
}
void MusicMng::start_load_music_list()
{
	LOG_ENTER;

	// TODO
	m_list.push_back(new CEGUI::String(Helper::Utf16ToString(L"¹þ¹þ")));
	m_list.push_back(new CEGUI::String("2"));
	m_list.push_back(new CEGUI::String("3"));
	m_list.push_back(new CEGUI::String("4"));
	m_list.push_back(new CEGUI::String("5"));
	m_list.push_back(new CEGUI::String("6"));
	m_list.push_back(new CEGUI::String("7"));
	m_list.push_back(new CEGUI::String("8"));
	m_list.push_back(new CEGUI::String("9"));

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
		return m_list[pos]->c_str();
	}
	return "empty";
}
