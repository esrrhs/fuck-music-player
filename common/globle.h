#ifndef __GLOBLE__
#define __GLOBLE__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class Globle : public boost::noncopyable
{
public:
	Globle() : m_hwnd(0) {};
	virtual ~Globle(){};
	s32 m_hwnd;
};

#endif	