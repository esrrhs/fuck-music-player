#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__

#include "wx/app.h"
#include "wx/frame.h"

class MainFrame;
// Define a new application
class MyApp : public wxApp
{
public:
	MyApp() : m_mainFrame(0) {}
    bool OnInit();
	MainFrame* GetMainFrame() { return m_mainFrame; }
private:
	MainFrame* m_mainFrame;
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
};

#endif
