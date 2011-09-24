#ifndef __UI_MNG__
#define __UI_MNG__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

namespace ui
{
	class uimsg;
}
class UIMng : public boost::noncopyable
{
public:
	UIMng();
	virtual ~UIMng();
	void ini();
	void heartbeat(double elapsed);
	void render();
private:
	void create_cegui_system();
	void ini_cegui_system();
	void zmq_heartbeat();
	void handle_zmq_msg(const ui::uimsg & msg);
private:
	void * m_zmq_socket;
};

#endif
