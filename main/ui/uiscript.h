#ifndef __UI_SCRIPT__
#define __UI_SCRIPT__

#include <boost/noncopyable.hpp>
#include "globledefine.h"
#include <map>

class UIScript : public boost::noncopyable
{
public:
	UIScript();
	virtual ~UIScript();
	void ini();
	void heartbeat(double elapsed);
	typedef double CustomValueType;
	CustomValueType GetCustomValue(const c8 * name);
	void SetCustomValue(const c8 * name, const CustomValueType & value);
private:
	std::map<std::string, CustomValueType> m_CustomValueMap;
private:
};

#endif
