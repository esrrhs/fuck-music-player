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

	CEGUI::utf8* buff = new CEGUI::utf8[len];
	WideCharToMultiByte(CP_UTF8, 0, utf16text, -1,
		reinterpret_cast<char*>(buff), len, 0, 0);
	const CEGUI::String result(buff);
	delete[] buff;

	return result;
}
std::wstring Utf8ToUtf16(const std::string& utf8text)
{
	const int textLen = MultiByteToWideChar(CP_UTF8, 0, utf8text.c_str(),
		utf8text.size(), 0, 0);

	if (textLen == 0)
		CEGUI_THROW(CEGUI::InvalidRequestException(
		"Utf8ToUtf16 - MultiByteToWideChar failed"));

	std::wstring wideStr(textLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8text.c_str(), utf8text.size(),
		&wideStr[0], wideStr.size());
	return wideStr;
}
std::string Utf16ToUtf8(const std::wstring& utf16text)
{
	const int len = WideCharToMultiByte(CP_UTF8, 0, utf16text.c_str(), -1,
		0, 0, 0, 0);
	if (!len)
		CEGUI_THROW(CEGUI::InvalidRequestException(
		"Utf16ToUtf8 - WideCharToMultiByte failed"));

	char* buff = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, utf16text.c_str(), -1,
		buff, len, 0, 0);
	const std::string result(buff);
	delete[] buff;

	return result;
}
std::string Utf16ToACP(const std::wstring& utf16text)
{
	const int len = WideCharToMultiByte(CP_ACP, 0, utf16text.c_str(), -1,
		0, 0, 0, 0);
	if (!len)
		CEGUI_THROW(CEGUI::InvalidRequestException(
		"Utf16ToUtf8 - WideCharToMultiByte failed"));

	char* buff = new char[len];
	WideCharToMultiByte(CP_ACP, 0, utf16text.c_str(), -1,
		buff, len, 0, 0);
	const std::string result(buff);
	delete[] buff;

	return result;
}
}