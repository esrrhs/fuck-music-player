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

	m_mainFrame->ini_zmq();

	// 启动主线程
	SINGLETON(MainThread).ini();
	
    return true;
}
int MyApp::OnExit()
{
	if ( !wxApp::OnExit() )
		return false;

	google::protobuf::ShutdownProtobufLibrary();

	return true;
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

	this->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(MainFrame::OnEnterWindow));
	this->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(MainFrame::OnLeaveWindow));

	this->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(MainFrame::OnMouseWheel));
}
void MainFrame::show_cursor(bool enable)
{
#ifdef WIN32
	::ShowCursor(enable);
#else
#error "unknown show_cursor implementation"
#endif
}

void MainFrame::ini_zmq()
{	void * ctx = zmq_init(1);
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
	s32 size = msg.ByteSize();
	zmq_msg_t zmsg;
	s32 rc = zmq_msg_init_size(&zmsg, size);
	if (rc)
	{
		return;
	}
	void * buffer = zmq_msg_data(&zmsg);
	msg.SerializeToArray(buffer, size);

	rc = zmq_send (m_zmq_socket, &zmsg, 0);
	if (rc)
	{
		return;
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

	send_zmq_msg(msg);
}
void MainFrame::OnLeftUp(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_left_up);

	send_zmq_msg(msg);
}
void MainFrame::OnRightDown(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_right_down);

	send_zmq_msg(msg);
}
void MainFrame::OnRightUp(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_right_up);

	send_zmq_msg(msg);
}
void MainFrame::OnMouseMove(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_mouse_move);
	msg.set_optional_x(event.GetPosition().x);
	msg.set_optional_y(event.GetPosition().y);

	send_zmq_msg(msg);
}
void MainFrame::OnKeyDown(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_down);
	msg.set_optional_key(event.GetKeyCode());

	send_zmq_msg(msg);
}
void MainFrame::OnKeyUp(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_up);
	msg.set_optional_key(event.GetKeyCode());

	send_zmq_msg(msg);
}
void MainFrame::OnKeyChar(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_char);
	msg.set_optional_key(event.GetKeyCode());

	send_zmq_msg(msg);
}
void MainFrame::OnCloseWindow(wxCloseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_close_window);

	send_zmq_msg(msg);
}
void MainFrame::OnEnterWindow(wxMouseEvent& event)
{
	show_cursor(false);
}
void MainFrame::OnLeaveWindow(wxMouseEvent& event)
{
	show_cursor(true);
}
void MainFrame::OnMouseWheel(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_mouse_wheel);
	msg.set_optional_wheel(event.GetWheelRotation());

	send_zmq_msg(msg);
}
