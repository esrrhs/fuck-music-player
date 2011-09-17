#ifndef __PLUG_IN_MAIN__
#define __PLUG_IN_MAIN__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef MAIN_EXPORTS
#define MAIN_API __declspec(dllexport)
#else
#define MAIN_API __declspec(dllimport)
#endif

#define PLUGIN_MAIN_FPS_CONFIG_NAME L"fps"

#endif
