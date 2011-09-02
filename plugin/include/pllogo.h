#ifndef __PLUG_IN_LOGO__
#define __PLUG_IN_LOGO__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef LOGO_EXPORTS
#define LOGO_API __declspec(dllexport)
#else
#define LOGO_API __declspec(dllimport)
#endif

#define PLUGIN_LOG_FILE_CONFIG_NAME L"logo"

#endif
