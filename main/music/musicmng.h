#ifndef __MUSIC_MNG__
#define __MUSIC_MNG__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

#include <vector>

namespace CEGUI
{
	class String;
}
class MusicMng : public boost::noncopyable
{
public:
	MusicMng();
	virtual ~MusicMng();
	void ini();
	void heartbeat(double elapsed);
	const c8 * get_list_item_name(s32 pos);
private:
	void load_file_finder();
	void start_load_music_list();
	void end_load_music_list();
private:
	std::vector<CEGUI::String *> m_list;
	void * m_filefinder;
};

#endif
