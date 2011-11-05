#include "plmusicengine.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

FMOD::System * g_system = NULL;
FMOD::Sound * g_playing_sound = NULL;
FMOD::Channel * g_channel = NULL;
FMOD_RESULT g_result;
f32 g_volume = 0.f;
u8 g_is_playing = 0;

typedef boost::mutex MUTEX;
typedef boost::mutex::scoped_lock LOCK;
MUTEX g_play_music_mutex;
std::vector<std::string> g_temp_play_music_list;

void ini_fmod_system()
{
    int               key = 0;
    unsigned int      version;

    /*
        Create a System object and initialize.
    */
    g_result = FMOD::System_Create(&g_system);

    g_result = g_system->getVersion(&version);

	if (version < FMOD_VERSION)
	{
		return;
	}

	g_result = g_system->init(32, FMOD_INIT_NORMAL, 0);

}
FMOD_RESULT F_CALLBACK play_call_back(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
	if(type == FMOD_CHANNEL_CALLBACKTYPE_END)
	{
		g_is_playing = 0;
	}

	return FMOD_OK;
}
void play_thread()
{
	std::string name;
	do
	{
		if (g_channel)
		{
			g_channel->setVolume(g_volume);
		}
		g_play_music_mutex.lock();
		if (g_temp_play_music_list.size() == 0)
		{
			g_play_music_mutex.unlock();
			SLEEP(10);
			continue;
		}
		name = g_temp_play_music_list.back();
		g_temp_play_music_list.clear();
		g_play_music_mutex.unlock();

		// 开始播放
		if (g_playing_sound)
		{
			g_playing_sound->release();
			g_playing_sound = NULL;
		}
		g_result = g_system->createSound(name.c_str(), FMOD_HARDWARE, 0, &g_playing_sound);
		g_result = g_playing_sound->setMode(FMOD_LOOP_OFF);
		g_result = g_system->playSound(FMOD_CHANNEL_FREE, g_playing_sound, false, &g_channel);
		g_result = g_channel->setVolume(g_volume);
		g_channel->setCallback(play_call_back);
	}
	while (1);
}
extern "C" MUSIC_ENGINE_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	
	STRING volume = config.Get(PLUGIN_MUSIC_ENGINE_VOLUME_CONFIG_NAME);
	g_volume = boost::lexical_cast<f32>(volume.c_str());

	ini_fmod_system();

	// 启动thread
	boost::thread td(boost::bind(play_thread));

	return true;
}
extern "C" MUSIC_ENGINE_API bool PLUGIN_QUIT_FUNC_DEFAAULT_NAME()
{
	g_result = g_system->close();
	g_result = g_system->release();
	return true;
}

extern "C" MUSIC_ENGINE_API PLUGIN_HANDLE_INPUT_STATUS PLUGIN_INPUT_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInInputType t = (PluginInInputType)(s32)type;
	switch (t)
	{
	case PI_I_MUSIC_ENGINE_PLAY_MUSIC:
		{
			const char * name = (const char *)param;
			LOCK lock(g_play_music_mutex);
			g_is_playing = 1;
			g_temp_play_music_list.push_back(name);
			return PLUGIN_HANDLE_INPUT_END;
		}
		break;
	case PI_I_MUSIC_ENGINE_SET_VOLUME:
		{
			const f32 * volume = (const f32 *)param;
			g_volume = *volume;
			return PLUGIN_HANDLE_INPUT_END;
		}
		break;
	}
	return PLUGIN_HANDLE_INPUT_CONTINUE;
}
extern "C" MUSIC_ENGINE_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	PluginInGetSetType t = (PluginInGetSetType)(s32)type;
	switch (t)
	{
	case PI_GS_MUSIC_ENGINE_IS_PLAYING:
		*((u8*)param) = (u8)g_is_playing;
		return true;
	}
	return false;
}
extern "C" MUSIC_ENGINE_API bool PLUGIN_SET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
	return false;
}

extern "C" MUSIC_ENGINE_API bool PLUGIN_RUN_FUNC_DEFAAULT_NAME()
{
	g_result = g_system->update();
	return true;
}