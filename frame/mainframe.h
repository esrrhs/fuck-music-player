#ifndef __MAIN_FRAME__
#define __MAIN_FRAME__

#include "wx/app.h"
#include "wx/frame.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

// Define a new frame
class MyFrame : public wxFrame
{
public:

    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);
    ~MyFrame();

};


#endif
