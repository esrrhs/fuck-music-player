#include "helper.h"

#if defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#endif

#include <string>
#include "CEGUIExceptions.h"

namespace Helper
{
CEGUI::String Utf16ToString(const wchar_t* const utf16text)
{
	const int len = WideCharToMultiByte(CP_UTF8, 0, utf16text, -1,
		0, 0, 0, 0);
	if (!len)
		CEGUI_THROW(CEGUI::InvalidRequestException(
		"Utf16ToUtf8 - WideCharToMultiByte failed"));

	CEGUI::utf8* buff = new CEGUI::utf8[len + 1];
	WideCharToMultiByte(CP_UTF8, 0, utf16text, -1,
		reinterpret_cast<char*>(buff), len, 0, 0);
	const CEGUI::String result(buff);
	delete[] buff;

	return result;
}
}