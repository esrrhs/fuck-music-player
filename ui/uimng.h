#ifndef __UI_MNG__
#define __UI_MNG__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class UIMng : public boost::noncopyable
{
public:
	UIMng();
	virtual ~UIMng();
	void ini();
	void heartbeat(float elapsed);
private:
	void create_cegui_system();
	void ini_cegui_system();
private:
};

#endif
