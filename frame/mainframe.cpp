#include "mainframe.h"
#include "wx/wxprec.h"

#include "globledefine.h"
#include <boost/pool/detail/singleton.hpp>
#include "mainthread.h"
#include "globle.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main frame window

   m_mainFrame = new MainFrame((wxFrame *)NULL, wxID_ANY, wxT("FuckMPlayer"),
                                 wxDefaultPosition, wxSize(500, 400),
                                 wxDEFAULT_FRAME_STYLE);
    m_mainFrame->Show(true);
    SetTopWindow(m_mainFrame);

	SINGLETON(Globle).m_hwnd = (s32)m_mainFrame->GetHWND();

	// 启动主线程
	SINGLETON(MainThread).ini();

    return true;
}

MainFrame::MainFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxFrame(parent, id, title, pos, size,
                          style | wxNO_FULL_REPAINT_ON_RESIZE)
{

}

MainFrame::~MainFrame()
{
}