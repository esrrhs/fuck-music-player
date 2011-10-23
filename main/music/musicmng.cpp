#include "musicmng.h"
#include "Logger.h"
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "../localheader.h"
#include "../helper/helper.h"

#include <boost/filesystem.hpp>

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

	namespace fs = boost::filesystem;

	STRING strPath = L"Z:\\Mymusic";

	fs::path rootPath(fs::initial_path());        // 初始化为本项目路径
	rootPath = fs::system_complete(fs::path(strPath, fs::native));    //将相对路径转换为绝对路径
	
	if (!fs::exists(rootPath))            //路径是否存在
	{
		LOG_ERROR("fs::exists");
		return;
	}
	if (!fs::is_directory(rootPath))        // 是否是目录
	{
		LOG_ERROR("fs::is_directory");
		return;
	}

	LOG_TRACE("start find...");

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
				m_list.push_back(new CEGUI::String(Helper::Utf16ToString(file_itr->path().leaf().c_str())));		//获取文件名
			}
		}
	} 
	while (!dirvec.empty());

	LOG_TRACE("end find");

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
