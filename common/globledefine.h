#ifndef __GLOBLE_DEFINE__
#define __GLOBLE_DEFINE__

#ifdef NULL
#undef NULL
#endif
#define NULL 0

typedef int s32;
typedef unsigned int u32;
typedef char c8;
typedef unsigned char u8;
typedef float f32;

#ifdef UNICODE
#define ZCHAR wchar_t
#define STRING std::wstring
#define IFSTREAM std::wifstream
#else
#define ZCHAR char
#define STRING std::string
#define IFSTREAM std::ifstream
#endif

#define SAFE_DELETE(x) if(x) { delete x; x = 0; }

#ifdef WIN32
#define SLEEP(ms) ::Sleep((DWORD)ms)
#else
#error "unknown sleep implementation"
#endif

#endif