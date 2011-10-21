#ifndef __UI_WHEEL__
#define __UI_WHEEL__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

#include <vector>

namespace CEGUI
{
	class String;
}
class UIWheel : public boost::noncopyable
{
public:
	UIWheel();
	virtual ~UIWheel();
	void ini();
	void heartbeat(double elapsed);
	const c8 * get_list_item_name(s32 pos);
private:
	void ini_wheel_list();
private:
	std::vector<CEGUI::String *> m_list;
};

#endif
