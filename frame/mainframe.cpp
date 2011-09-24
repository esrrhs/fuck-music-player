#include "mainframe.h"
#include "wx/wxprec.h"

#include "globledefine.h"
#include "uimsg.pb.h"

#include "Config.h"

#include "plugincontainer.h"

#include "plframeadapter.h"

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

	m_root = new PluginSys::Plugin(L"", L"");

    m_mainFrame->Show(true);
    SetTopWindow(m_mainFrame);

	// load plugins
	Config config(L"fuck.cfg");
	STRING fl = config.Get(PLUGIN_CONTAINER_CONFIG_FILE_LIST_KEY);
	STRING nl = config.Get(PLUGIN_CONTAINER_CONFIG_NAME_LIST_KEY);
	m_pc = new PluginSys::PluginContainer(fl, nl);
	m_pc->AddFather(m_root);
	m_pc->Ini();

	WXHWND hwnd = m_mainFrame->GetHWND();
	m_pc->Set((void*)PI_GS_UI_WIN_HANDLE, (void*)&hwnd);

    return true;
}
int MyApp::OnExit()
{
	if ( !wxApp::OnExit() )
		return false;

	m_pc->Quit();
	delete m_pc;
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

MainFrame::~MainFrame()
{
}

void MainFrame::OnLeftDown(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_left_down);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnLeftUp(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_left_up);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnRightDown(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_right_down);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnRightUp(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_right_up);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnMouseMove(wxMouseEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_mouse_move);
	msg.set_optional_x(event.GetPosition().x);
	msg.set_optional_y(event.GetPosition().y);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnKeyDown(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_down);
	msg.set_optional_key(event.GetKeyCode());

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnKeyUp(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_up);
	msg.set_optional_key(event.GetKeyCode());

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnKeyChar(wxKeyEvent& event)
{
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_key_char);
	msg.set_optional_key(event.GetKeyCode());

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
void MainFrame::OnCloseWindow(wxCloseEvent& event)
{
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnCloseWindow));
	ui::uimsg msg;
	msg.set_required_type(ui::uimsg_type_close_window);

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
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

	wxGetApp().GetPluginContainer()->Input((void*)PI_I_UI_MSG, (void*)&msg);
}
