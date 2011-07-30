#ifndef __UI_MNG__
#define __UI_MNG__

#include <boost/noncopyable.hpp>

class UIMng : public boost::noncopyable
{
public:
	UIMng();
	virtual ~UIMng();
	void ini();
	void run();
};

#endif
