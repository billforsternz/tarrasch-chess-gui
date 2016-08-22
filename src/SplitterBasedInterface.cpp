#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "Appdefs.h"
#include "GraphicBoardResizable.h"


class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow* parent) : wxFrame(parent, -1, _("Tarrasch Db"),
                                    wxDefaultPosition, wxSize(800,600),
                                    wxDEFAULT_FRAME_STYLE)                              
    {
        // notify wxAUI which frame to use
        m_mgr.SetManagedWindow(this);
 
        // create several text controls
        #if 0
        wxTextCtrl* text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),
        wxDefaultPosition, wxSize(200,150),
        wxNO_BORDER | wxTE_MULTILINE);
        #endif
        GraphicBoardResizable *gb = new GraphicBoardResizable( this,
                              wxID_ANY,
                              wxDefaultPosition, wxSize(200,150), 20 );
        gb->SetPosition( "rnbqkbnrpppppppp                                PPPPPPPPRNBQKBNR" );
        wxTextCtrl* text2 = new wxTextCtrl(this, -1, _("Pane 2 - sample text"),
        wxDefaultPosition, wxSize(200,150),
        wxNO_BORDER | wxTE_MULTILINE);
 
        wxTextCtrl* text3 = new wxTextCtrl(this, -1, _("Main content window"),
        wxDefaultPosition, wxSize(200,150),
        wxNO_BORDER | wxTE_MULTILINE);
 
        // add the panes to the manager
        m_mgr.AddPane(gb, wxLEFT, wxT("Pane Number One"));
        m_mgr.AddPane(text2, wxBOTTOM, wxT("Pane Number Two"));
        m_mgr.AddPane(text3, wxCENTER);
 
        // tell the manager to "commit" all the changes just made
        m_mgr.Update();
    }
 
    ~MyFrame()
    {
        // deinitialize the frame manager
        m_mgr.UnInit();
    }
 
private:
    wxAuiManager m_mgr;
};
 
// our normal wxApp-derived class, as usual
class MyApp : public wxApp
{
public:
 
    bool OnInit()
    {
        wxFrame* frame = new MyFrame(NULL);
        SetTopWindow(frame);
        frame->Show();
        return true;                    
    }
};
 
#ifdef NEW_USER_INTERFACE

DECLARE_APP(MyApp);
IMPLEMENT_APP(MyApp);
#endif

