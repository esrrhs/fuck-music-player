#ifndef __PLUG_IN_FILE_FINDER__
#define __PLUG_IN_FILE_FINDER__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef FILE_FINDER_EXPORTS
#define FILE_FINDER_API __declspec(dllexport)
#else
#define FILE_FINDER_API __declspec(dllimport)
#endif

#define PLUGIN_FILE_FINDER_DIR_CONFIG_NAME L"dir"
#define PLUGIN_FILE_FINDER_FILTER_CONFIG_NAME L"filter"
#define PLUGIN_FILE_FINDER_SLEEP_TIME_CONFIG_NAME L"sleep_time"
#define PLUGIN_FILE_FINDER_THREAD_NUM_CONFIG_NAME L"thread_num"

#endif
