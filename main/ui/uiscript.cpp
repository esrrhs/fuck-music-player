#include "uiscript.h"
#include "Logger.h"
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "CEGUI.h"
#include "../localheader.h"
#include "../render/render.h"
#include "../god/god.h"
#include "uimng.h"
#include "../lua_tinker/lua_tinker.h"
#include "../helper/helper.h"
#include "../music/musicmng.h"

#include "ScriptingModules/LuaScriptModule/CEGUILua.h"

UIScript::UIScript()
{
}
UIScript::~UIScript()
{
}

const c8 * lua_GetWheelName(s32 pos)
{
	return SINGLETON(MusicMng).get_list_item_name(pos);
}
void UIScript::ini()
{
	LOG_ENTER;
	
	CEGUI::LuaScriptModule * ls = (CEGUI::LuaScriptModule*)CEGUI::System::getSingleton().getScriptingModule();
	lua_State* L = ls->getLuaState();
	lua_tinker::def(L, "GetWheelName", lua_GetWheelName);

	LOG_LEAVE;
}
void UIScript::heartbeat(double elapsed)
{
}
