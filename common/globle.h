#ifndef __GLOBLE__
#define __GLOBLE__

#include <boost/noncopyable.hpp>
#include "globledefine.h"

class Globle : public boost::noncopyable
{
public:
	Globle() : m_hwnd(0), m_pD3DDevice(0), m_zmq_ui_ctx(0) {};
	virtual ~Globle(){};
	s32 m_hwnd;
	void * m_pD3DDevice;
	void * m_zmq_ui_ctx;
};

#endif	