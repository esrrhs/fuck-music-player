#include "uiwheel.h"
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

#include "ScriptingModules/LuaScriptModule/CEGUILua.h"

UIWheel::UIWheel()
{
}
UIWheel::~UIWheel()
{
}

const c8 * lua_GetWheelName(s32 pos)
{
	return SINGLETON(UIWheel).get_list_item_name(pos);
}
void UIWheel::ini()
{
	LOG_ENTER;

	ini_wheel_list();

	CEGUI::LuaScriptModule * ls = (CEGUI::LuaScriptModule*)CEGUI::System::getSingleton().getScriptingModule();
	lua_State* L = ls->getLuaState();
	lua_tinker::def(L, "GetWheelName", lua_GetWheelName);

	LOG_LEAVE;
}
void UIWheel::heartbeat(double elapsed)
{
}
void UIWheel::ini_wheel_list()
{
	// TODO
	m_list.push_back(new CEGUI::String(Helper::Utf16ToString(L"¹þ¹þ")));
	m_list.push_back(new CEGUI::String("2"));
	m_list.push_back(new CEGUI::String("3"));
	m_list.push_back(new CEGUI::String("4"));
	m_list.push_back(new CEGUI::String("5"));
	m_list.push_back(new CEGUI::String("6"));
	m_list.push_back(new CEGUI::String("7"));
	m_list.push_back(new CEGUI::String("8"));
	m_list.push_back(new CEGUI::String("9"));
}
const c8 * UIWheel::get_list_item_name(s32 pos)
{
	if (pos >= 0 && pos < (s32)m_list.size())
	{
		return m_list[pos]->c_str();
	}
	return "empty";
}
