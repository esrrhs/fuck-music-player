#ifndef __UI_WHEEL__
#define __UI_WHEEL__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class UIWheel : public boost::noncopyable
{
public:
	UIWheel();
	virtual ~UIWheel();
	void ini();
	void heartbeat(double elapsed);
private:
private:
};

#endif
