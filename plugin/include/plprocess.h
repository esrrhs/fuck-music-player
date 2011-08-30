#ifndef __PLUG_IN_PROCESS__
#define __PLUG_IN_PROCESS__

#include "plheader.h"
#include "plmsgheader.h"

#ifdef PROCESS_EXPORTS
#define PROCESS_API __declspec(dllexport)
#else
#define PROCESS_API __declspec(dllimport)
#endif

#endif
