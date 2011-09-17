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
