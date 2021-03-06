#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__

#include "wx/app.h"
#include "wx/frame.h"

class MainFrame;
namespace ui
{
	class uimsg;
}
namespace PluginSys
{
	class PluginContainer;
	class Plugin;
}
// Define a new application
class MyApp : public wxApp
{
public:
	MyApp() : m_mainFrame(0), m_pc(0) {}
	~MyApp() {}
    bool OnInit();
	int OnExit();
	MainFrame* GetMainFrame() { return m_mainFrame; }
	PluginSys::PluginContainer * GetPluginContainer() { return m_pc; }
private:
	MainFrame* m_mainFrame;
	PluginSys::PluginContainer * m_pc;
	PluginSys::Plugin * m_root;
};

// Define a new frame
class MainFrame : public wxFrame
{
public:
    MainFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    ~MainFrame();

	void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnRightDown(wxMouseEvent& event);
	void OnRightUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnKeyChar(wxKeyEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	void OnEnterWindow(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);

	void OnMouseWheel(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);

	void ini_zmq();
private:
	void send_zmq_msg(const ui::uimsg & msg);
	void show_cursor(bool enable);
private:
	void * m_zmq_socket;
};

#endif
