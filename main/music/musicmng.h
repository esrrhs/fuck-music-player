#ifndef __MUSIC_MNG__
#define __MUSIC_MNG__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

#include <vector>

class MusicMng : public boost::noncopyable
{
public:
	MusicMng();
	virtual ~MusicMng();
	void ini();
	void heartbeat(double elapsed);
	const c8 * get_list_item_name(s32 pos);
	s32 get_list_item_num();
	const c8 * get_find_dir_name();
	const c8 * get_find_name();
	const c8 * get_find_num();
	void * GetFileFinder() { return m_filefinder; }
	void parse_from_find_list();
	void end_load_music_list();
	void play_music(const c8 * name);
	void play_music(s32 pos);
	void random_shuffle();
	void set_music_volume(f32 volume);
private:
	void ini_music_engine();
	void load_file_finder();
	void start_load_music_list();
private:
	std::vector<STRING> m_list;
	void * m_filefinder;
	void * m_musicengine;
};

#endif
