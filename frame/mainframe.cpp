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
	this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MainFrame::OnLeftDown));
	this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MainFrame::OnLeftUp));
	this->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MainFrame::OnRightDown));
	this->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(MainFrame::OnRightUp));
	this->Connect(wxEVT_MOTION, wxMouseEventHandler(MainFrame::OnMouseMove));

	this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MainFrame::OnKeyDown));
	this->Connect(wxEVT_KEY_UP, wxKeyEventHandler(MainFrame::OnKeyUp));
	this->Connect(wxEVT_CHAR, wxCharEventHandler(MainFrame::OnKeyChar));
	
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnCloseWindow));
	
}

MainFrame::~MainFrame()
{
}

void MainFrame::OnLeftDown(wxMouseEvent& event)
{
	int i = 0;
}
void MainFrame::OnLeftUp(wxMouseEvent& event)
{
	int i = 0;
}
void MainFrame::OnRightDown(wxMouseEvent& event)
{
	int i = 0;
}
void MainFrame::OnRightUp(wxMouseEvent& event)
{
	int i = 0;
}
void MainFrame::OnMouseMove(wxMouseEvent& event)
{
	int i = 0;
}
void MainFrame::OnKeyDown(wxKeyEvent& event)
{
	int i = 0;
	event.Skip(true);
}
void MainFrame::OnKeyUp(wxKeyEvent& event)
{
	int i = 0;
}
void MainFrame::OnKeyChar(wxKeyEvent& event)
{
	int i = 0;
}
void MainFrame::OnCloseWindow(wxCloseEvent& event)
{
	int i = 0;
}