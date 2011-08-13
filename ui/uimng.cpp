#include "uimng.h"
#include "Logger.h"
#include <d3d9.h>
#include <boost/pool/detail/singleton.hpp>
#include "globle.h"
#include "RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h"
#include "CEGUI.h"
#include "globle.h"

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
	ini_zmq();

	LOG_LEAVE;
}
void UIMng::heartbeat(float elapsed)
{
	CEGUI::System & guiSystem = CEGUI::System::getSingleton();

	guiSystem.injectTimePulse(elapsed);

	zmq_heartbeat();
}
void UIMng::render()
{
	CEGUI::System & guiSystem = CEGUI::System::getSingleton();

	((LPDIRECT3DDEVICE9)SINGLETON(Globle).m_pD3DDevice)->BeginScene();
	((LPDIRECT3DDEVICE9)SINGLETON(Globle).m_pD3DDevice)->Clear(0, 0, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	guiSystem.renderGUI();
	((LPDIRECT3DDEVICE9)SINGLETON(Globle).m_pD3DDevice)->EndScene();
	((LPDIRECT3DDEVICE9)SINGLETON(Globle).m_pD3DDevice)->Present(0, 0, 0, 0);
}
void UIMng::create_cegui_system()
{
	LOG_ENTER;

	// 创建d3d
	LPDIRECT3D9 d_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d_D3D)
	{
		LOG_ERROR("Direct3DCreate9");
		return;
	}
	LOG_TRACE("Direct3DCreate9 ok");

	D3DDISPLAYMODE d3ddm;
	if (FAILED(d_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		LOG_ERROR("GetAdapterDisplayMode");
		return;
	}

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) ); 
	d3dpp.Windowed = TRUE; // 指明是窗口模式，而非全局独占模式 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // 无需交换链 
	d3dpp.BackBufferFormat = d3ddm.Format; // 后备缓冲区颜色格式 
	d3dpp.EnableAutoDepthStencil = TRUE; // 使深度缓冲区有效 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // 深度缓冲区颜色格式 
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//使用屏幕刷新率
	d3dpp.hDeviceWindow = (HWND)SINGLETON(Globle).m_hwnd;

	LPDIRECT3DDEVICE9 d_3DDevice;
	if (FAILED(d_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)SINGLETON(Globle).m_hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d_3DDevice)))
	{
		LOG_ERROR("CreateDevice");
		return;
	}
	SINGLETON(Globle).m_pD3DDevice = d_3DDevice;
	LOG_TRACE("CreateDevice ok");

	// 创建cegui
	CEGUI::Direct3D9Renderer& renderer(CEGUI::Direct3D9Renderer::create((LPDIRECT3DDEVICE9)SINGLETON(Globle).m_pD3DDevice));
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

	// TODO 先临时用
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	LOG_TRACE("create Scheme ok");

	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::DefaultWindow* root = (CEGUI::DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
	CEGUI::System::getSingleton().setGUISheet(root);
	CEGUI::FrameWindow* wnd = (CEGUI::FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo Window");
	root->addChildWindow(wnd);
	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
	wnd->setSize(CEGUI::UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
	wnd->setMaxSize(CEGUI::UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
	wnd->setMinSize(CEGUI::UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));
	wnd->setText("Hello World!");

	LOG_LEAVE;
}
void UIMng::ini_zmq()
{
	LOG_ENTER;

	void * ctx = zmq_init(1);
	if (!ctx) 
	{
		LOG_ERROR("zmq_init");
		return;
	}

	void * s = zmq_socket(ctx, ZMQ_PULL);
	if (!s) 
	{
		LOG_ERROR("zmq_socket");
		return;
	}

	s32 rc = zmq_bind(s, UI_ZMQ_NAME);
	if (rc) 
	{
		LOG_ERROR("zmq_bind");
		return;
	}

	m_zmq_socket = s;
	SINGLETON(Globle).m_zmq_ui_ctx = ctx;

	LOG_LEAVE;
}
void UIMng::zmq_heartbeat()
{
	zmq_msg_t zmsg;
	zmq_msg_init(&zmsg);
	s32 rc;
	ui::uimsg msg;
	while(1)
	{
		rc = zmq_recv(m_zmq_socket, &zmsg, ZMQ_NOBLOCK);
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

		}
		break;
	case ui::uimsg_type_close_window:
		{

		}
		break;
	}
}