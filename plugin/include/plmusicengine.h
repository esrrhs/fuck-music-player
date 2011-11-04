#ifndef __PLUG_IN_MUSIC_ENGINE__
#define __PLUG_IN_MUSIC_ENGINE__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef MUSIC_ENGINE_EXPORTS
#define MUSIC_ENGINE_API __declspec(dllexport)
#else
#define MUSIC_ENGINE_API __declspec(dllimport)
#endif

#define PLUGIN_MUSIC_ENGINE_VOLUME_CONFIG_NAME L"volume"

#endif
