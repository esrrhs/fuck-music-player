#include "plmusicengine.h"
#include "plugin.h"
#include "plugincontainer.h"
#include "config.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include <boost/lexical_cast.hpp>

FMOD::System * g_system = NULL;
FMOD::Sound * g_playing_sound = NULL;
FMOD::Channel * g_channel = NULL;
FMOD_RESULT g_result;
f32 g_volume = 0.f;

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

extern "C" MUSIC_ENGINE_API bool PLUGIN_INI_FUNC_DEFAAULT_NAME(PluginSys::Plugin * p)
{
	STRING name = p->name();
	name += L".cfg";
	Config config(name);
	
	STRING volume = config.Get(PLUGIN_MUSIC_ENGINE_VOLUME_CONFIG_NAME);
	g_volume = boost::lexical_cast<f32>(volume.c_str());

	ini_fmod_system();

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
			if (g_playing_sound)
			{
				g_playing_sound->release();
				g_playing_sound = NULL;
			}
			g_result = g_system->createSound(name, FMOD_HARDWARE, 0, &g_playing_sound);
			g_result = g_playing_sound->setMode(FMOD_LOOP_OFF);
			g_result = g_system->playSound(FMOD_CHANNEL_FREE, g_playing_sound, false, &g_channel);
			g_result = g_channel->setVolume(g_volume);
			return PLUGIN_HANDLE_INPUT_END;
		}
		break;
	}
	return PLUGIN_HANDLE_INPUT_CONTINUE;
}
extern "C" MUSIC_ENGINE_API bool PLUGIN_GET_FUNC_DEFAAULT_NAME(void * type, void * param)
{
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