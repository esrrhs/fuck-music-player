#include "uimng.h"
#include "Logger.h"
#include <d3d9.h>
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h"
#include "CEGUI.h"
#include "globle.h"
#include "../localheader.h"
#include "../render/render.h"
#include "../god/god.h"
#include "uiwheel.h"
#include "ScriptingModules/LuaScriptModule/CEGUILua.h"

#include "uimsg.pb.h"

#include "zmq.h"
#include "zmq_utils.h"
#include "platform.hpp"

UIMng::UIMng() : m_zmq_socket(0)
{
}
UIMng::~UIMng()
{
}
void UIMng::ini()
{
	LOG_ENTER;

	create_cegui_system();
	ini_cegui_system();

	LOG_LEAVE;
}
void UIMng::heartbeat(double elapsed)
{
	zmq_heartbeat();
	CEGUI::System::getSingleton().injectTimePulse((float)elapsed);
}
void UIMng::render()
{
	CEGUI::System & guiSystem = CEGUI::System::getSingleton();
	guiSystem.renderGUI();
}
void UIMng::create_cegui_system()
{
	LOG_ENTER;

	void * pD3DDevice = SINGLETON(Render).GetDevice();

	// 创建cegui
	CEGUI::Direct3D9Renderer& renderer(CEGUI::Direct3D9Renderer::create((LPDIRECT3DDEVICE9)pD3DDevice));
	LOG_TRACE("Create Direct3D9Renderer ok");

	CEGUI::DefaultResourceProvider* rp = new CEGUI::DefaultResourceProvider();
	CEGUI::System::create(renderer, rp);
	LOG_TRACE("Create System ok");

	// 设置资源
	rp->setResourceGroupDirectory("schemes", "ui/schemes/");
	rp->setResourceGroupDirectory("imagesets", "ui/imagesets/");
	rp->setResourceGroupDirectory("fonts", "ui/fonts/");
	rp->setResourceGroupDirectory("layouts", "ui/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "ui/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "ui/lua_scripts/");
	rp->setResourceGroupDirectory("schemas", "ui/xml_schemas/");   
	rp->setResourceGroupDirectory("animations", "ui/animations/"); 

	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
	LOG_TRACE("setDefaultResourceGroup ok");

	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	LOG_LEAVE;
}
void UIMng::ini_cegui_system()
{
	LOG_ENTER;

	CEGUI::LuaScriptModule& scriptmod(CEGUI::LuaScriptModule::create());

	CEGUI::System::getSingleton().setScriptingModule(&scriptmod);

	CEGUI::System::getSingleton().executeScriptFile("ini_cegui_system.lua");

	SINGLETON(UIWheel).ini();
	
	LOG_LEAVE;
}
void UIMng::zmq_heartbeat()
{
	void * zmq_socket = SINGLETON(God).GetZeromqSocket();

	zmq_msg_t zmsg;
	zmq_msg_init(&zmsg);
	s32 rc;
	ui::uimsg msg;
	while(1)
	{
		rc = zmq_recv(zmq_socket, &zmsg, ZMQ_NOBLOCK);
		if (rc < 0) 
		{
			break;
		}
		msg.ParseFromArray(zmq_msg_data(&zmsg), zmq_msg_size(&zmsg));
		handle_zmq_msg(msg);
	}
	zmq_msg_close(&zmsg);
}
void UIMng::handle_zmq_msg(const ui::uimsg & msg)
{
	switch(msg.required_type())
	{
	case ui::uimsg_type_left_down:
		{
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
		}
		break;
	case ui::uimsg_type_left_up:
		{
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
		}
		break;
	case ui::uimsg_type_right_down:
		{
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
		}
		break;
	case ui::uimsg_type_right_up:
		{
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
		}
		break;
	case ui::uimsg_type_mouse_move:
		{
			CEGUI::System::getSingleton().injectMousePosition(msg.optional_x(), msg.optional_y());
		}
		break;
	case ui::uimsg_type_key_down:
		{
			CEGUI::System::getSingleton().injectKeyDown(msg.optional_key());
		}
		break;
	case ui::uimsg_type_key_up:
		{
			CEGUI::System::getSingleton().injectKeyUp(msg.optional_key());
		}
		break;
	case ui::uimsg_type_key_char:
		{
			CEGUI::System::getSingleton().injectChar(msg.optional_key());
		}
		break;
	case ui::uimsg_type_close_window:
		{
			::PostMessage((HWND)SINGLETON(God).GetHwnd(), WM_CLOSE, 0, 0);
		}
		break;
	case ui::uimsg_type_mouse_wheel:
		{
			CEGUI::System::getSingleton().injectMouseWheelChange((float)msg.optional_wheel());
		}
		break;
	}
}