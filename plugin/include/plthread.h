#ifndef __PLUG_IN_THREAD__
#define __PLUG_IN_THREAD__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef THREAD_EXPORTS
#define THREAD_API __declspec(dllexport)
#else
#define THREAD_API __declspec(dllimport)
#endif

#define PLUGIN_THREAD_FPS_CONFIG_NAME L"fps"

#endif
