#ifndef __HELPER__
#define __HELPER__

#include "CEGUIBase.h"
#include "CEGUIString.h"

namespace Helper
{
	CEGUI::String Utf16ToString(const wchar_t* const utf16text);
	std::wstring Utf8ToUtf16(const std::string& utf8text);
}

#endif
