#include "mainframe.h"
#include "wx/wxprec.h"

#include "globledefine.h"
#include <boost/pool/detail/singleton.hpp>
#include "mainthread.h"
#include "globle.h"
#include "uimsg.pb.h"

#include "zmq.h"
#include "zmq_utils.h"
#include "platform.hpp"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

	GOOGLE_PROTOBUF_VERIFY_VERSION;

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

MyApp::~MyApp()
{
	google::protobuf::ShutdownProtobufLibrary();
}

MainFrame::MainFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
       : wxFrame(parent, id, title, pos, size,
                          style | wxNO_FULL_REPAINT_ON_RESIZE), 
						  m_zmq_socket(0)
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
	
	// 初始化zeromq
	ini_zmq();
}

void MainFrame::ini_zmq()
{
	void * ctx = zmq_init(1);
	if (!ctx) 
	{
		return;
	}

	void * s = zmq_socket(ctx, ZMQ_PUSH);
	if (!s) 
	{
		return;
	}

	s32 rc = zmq_bind(s, UI_ZMQ_NAME);
	if (rc) 
	{
		return;
	}

	m_zmq_socket = s;
	SINGLETON(Globle).m_zmq_ui_ctx = ctx;
}
void MainFrame::send_zmq_msg(const ui::uimsg & msg)
{
	std::string str;
	msg.SerializeToString(&str);

	zmq_msg_t zmsg;
	s32 rc = zmq_msg_init_data(&zmsg, (void*)str.c_str(), str.length(), 0, 0);
	if (rc)
	{
		return;
	}
	rc = zmq_send (m_zmq_socket, &zmsg, ZMQ_NOBLOCK);
	if (rc)
	{
		if (errno != EAGAIN)
		{
			return;
		}
	}
}
MainFrame::~MainFrame()
{
	zmq_close(m_zmq_socket);
	m_zmq_socket = 0;
	zmq_term(SINGLETON(Globle).m_zmq_ui_ctx);
	SINGLETON(Globle).m_zmq_ui_ctx = 0;
}

void MainFrame::OnLeftDown(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_left_down);
	msg.set_optional_x(event.GetPosition().x);
	msg.set_optional_y(event.GetPosition().y);
	
	send_zmq_msg(msg);
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