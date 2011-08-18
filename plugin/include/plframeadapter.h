#ifndef __PLUG_IN_FRAME_ADAPTER__
#define __PLUG_IN_FRAME_ADAPTER__

#include "plheader.h"

#ifdef FRAMEADAPTER_EXPORTS
#define FRAMEADAPTER_API __declspec(dllexport)
#else
#define FRAMEADAPTER_API __declspec(dllimport)
#endif

#endif
