#ifndef __UI_SCRIPT__
#define __UI_SCRIPT__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class UIScript : public boost::noncopyable
{
public:
	UIScript();
	virtual ~UIScript();
	void ini();
	void heartbeat(double elapsed);
private:
private:
};

#endif
