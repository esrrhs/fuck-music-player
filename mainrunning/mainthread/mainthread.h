#ifndef __MAIN_THREAD__
#define __MAIN_THREAD__

#include <boost/noncopyable.hpp>

class MainThread : public boost::noncopyable
{
public:
	void ini();
	void run();
};

#endif	