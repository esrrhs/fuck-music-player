#ifndef __PLUG_IN_MS_HEADER__
#define __PLUG_IN_MS_HEADER__

enum PluginInInputType
{
	PI_I_UI_MSG
};

enum PluginInGetSetType
{
	PI_GS_UI_WIN_HANDLE,
	PI_GS_THREAD_ELAPSED,
	PI_GS_THREAD_QUIT_FLG,
	PI_GS_ZEROMQ_CTX,
	PI_GS_ZEROMQ_OK_FLG,
};

#define UI_ZMQ_NAME "inproc://ui_msg"

#endif
