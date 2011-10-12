#include "uiwheel.h"
#include "Logger.h"
#include <boost/pool/singleton_pool.hpp>
#include "globle.h"
#include "CEGUI.h"
#include "../localheader.h"
#include "../render/render.h"
#include "../god/god.h"
#include "uimng.h"

UIWheel::UIWheel()
{
}
UIWheel::~UIWheel()
{
}
void UIWheel::ini()
{
	LOG_ENTER;
	
/*	CEGUI::Window * root = CEGUI::System::getSingleton().getGUISheet();
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();

	CEGUI::ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	CEGUI::Window* background = winMgr.createWindow ("TaharezLook/StaticImage");

	background->setArea (CEGUI::URect (cegui_reldim (0), cegui_reldim (0),
		cegui_reldim (1), cegui_reldim (1)));
	background->setProperty ("FrameEnabled", "false");
	background->setProperty ("BackgroundEnabled", "false");
	background->setProperty ("Image", "set:BackgroundImage image:full_image");

	CEGUI::Window * wnd = (CEGUI::FrameWindow*)winMgr.createWindow("TaharezLook/StaticText", "w1");
	background->addChildWindow(wnd);
	wnd->setPosition(CEGUI::UVector2(cegui_reldim(0.f), cegui_reldim( 0.f)));
	wnd->setSize(CEGUI::UVector2(cegui_reldim(0.2f), cegui_reldim( 0.1f)));
	wnd->setMaxSize(CEGUI::UVector2(cegui_reldim(1.f), cegui_reldim( 1.f)));
	wnd->setProperty ("HorzFormatting", "WordWrapCentred");
	wnd->setText("Hello World!");

	root->addChildWindow(background);*/

	LOG_LEAVE;
}
void UIWheel::heartbeat(double elapsed)
{
}
