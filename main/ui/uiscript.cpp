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
const c8 * lua_GetFindDir()
{
	return SINGLETON(MusicMng).get_find_dir_name();
}
const c8 * lua_GetFindName()
{
	return SINGLETON(MusicMng).get_find_name();
}
const c8 * lua_GetFindNum()
{
	return SINGLETON(MusicMng).get_find_num();
}
s32 lua_GetStatus()
{
	return (s32)SINGLETON(God).GetStauts();
}
void lua_SetStatus(s32 status)
{
	SINGLETON(God).SetStauts((God::GodStauts)status);
}
void lua_OnFindEnd()
{
	SINGLETON(MusicMng).parse_from_find_list();
}
UIScript::CustomValueType lua_GetCustomValue(const c8 * name)
{
	return SINGLETON(UIScript).GetCustomValue(name);
}
void lua_SetCustomValue(const c8 * name, UIScript::CustomValueType value)
{
	SINGLETON(UIScript).SetCustomValue(name, value);
}
UIScript::CustomValueType UIScript::GetCustomValue(const c8 * name)
{
	return m_CustomValueMap[name];
}
void UIScript::SetCustomValue(const c8 * name, const UIScript::CustomValueType & value)
{
	m_CustomValueMap[name] = value;
}
void UIScript::ini()
{
	LOG_ENTER;

	CEGUI::System::getSingleton().executeScriptFile("globlefunc.lua");

	CEGUI::LuaScriptModule * ls = (CEGUI::LuaScriptModule*)CEGUI::System::getSingleton().getScriptingModule();
	lua_State* L = ls->getLuaState();
	lua_tinker::def(L, "GetWheelName", lua_GetWheelName);
	lua_tinker::def(L, "GetStatus", lua_GetStatus);
	lua_tinker::def(L, "SetStatus", lua_SetStatus);
	lua_tinker::def(L, "OnFindEnd", lua_OnFindEnd);
	lua_tinker::def(L, "GetCustomValue", lua_GetCustomValue);
	lua_tinker::def(L, "SetCustomValue", lua_SetCustomValue);
	lua_tinker::def(L, "GetFindDir", lua_GetFindDir);
	lua_tinker::def(L, "GetFindName", lua_GetFindName);
	lua_tinker::def(L, "GetFindNum", lua_GetFindNum);
	
	LOG_LEAVE;
}
void UIScript::heartbeat(double elapsed)
{
	CEGUI::LuaScriptModule * ls = (CEGUI::LuaScriptModule*)CEGUI::System::getSingleton().getScriptingModule();
	lua_State* L = ls->getLuaState();

	lua_tinker::call<double>(L, "UIScript_Heartbeat", elapsed);
}
