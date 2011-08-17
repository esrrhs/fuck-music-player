#ifndef __PLUG_IN_FRAME_ADAPTER__
#define __PLUG_IN_FRAME_ADAPTER__

#include "plheader.h"

#ifndef FRAME_ADAPTER_EXPORTS
#define FRAME_ADAPTER_API __declspec(dllexport)
#else
#define FRAME_ADAPTER_API __declspec(dllimport)
#endif

#endif
