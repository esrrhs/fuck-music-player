#ifndef __MAIN_THREAD__
#define __MAIN_THREAD__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class MainThread : public boost::noncopyable
{
public:
	MainThread();
	virtual ~MainThread();
	void ini();
	void run();
	void heartbeat(double elapsed);
private:
};

#endif	
