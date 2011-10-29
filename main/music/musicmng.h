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
	const c8 * get_find_dir_name();
	const c8 * get_find_name();
	const c8 * get_find_num();
	void * GetFileFinder() { return m_filefinder; }
	void parse_from_find_list();
	void end_load_music_list();
private:
	void load_file_finder();
	void start_load_music_list();
private:
	std::vector<STRING> m_list;
	void * m_filefinder;
};

#endif
