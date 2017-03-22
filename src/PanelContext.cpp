/****************************************************************************
 * Top level UI panel for providing context, kibitzer info etc.
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/button.h"
#include "wx/sysopt.h"
#include "Appdefs.h"
#include "CtrlBox.h"
#include "CtrlBox2.h"
#include "CtrlBoxBookMoves.h"
#include "CtrlChessBoard.h"
#include "PanelContext.h"
#include "GameLogic.h"
#include "CentralWorkSaver.h"
#include "thc.h"
#include "UciInterface.h"
#include "DebugPrintf.h"
#include "Log.h"
#include "Session.h"
#include "DbPrimitives.h"
#include "Database.h"
#include "Book.h"
#include "Tabs.h"
#include "Repository.h"
#include "Objects.h"

//temp
bool view_flags_book_moves;

//-----------------------------------------------------------------------------
// Event table
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PanelContext, wxWindow)//Panel)
    EVT_SIZE( PanelContext::OnSize )
    EVT_NOTEBOOK_PAGE_CHANGED( wxID_ANY, PanelContext::OnTabSelected)   //user selects Suggestion or Engine Analysis
    EVT_BUTTON(ID_BUTTON1,PanelContext::OnButton1)
    EVT_BUTTON(ID_BUTTON2,PanelContext::OnButton2)
    EVT_BUTTON(ID_BUTTON3,PanelContext::OnButton3)
    EVT_BUTTON(ID_BUTTON4,PanelContext::OnButton4)
    EVT_BUTTON(ID_KIBITZ_BUTTON1,PanelContext::OnKibitzButton1)
    EVT_BUTTON(ID_KIBITZ_BUTTON2,PanelContext::OnKibitzButton2)
    EVT_BUTTON(ID_KIBITZ_BUTTON3,PanelContext::OnKibitzButton3)
END_EVENT_TABLE()


// Destructor
PanelContext::~PanelContext()
{
    if( font1 )
    {
        delete font1;
        font1 = NULL;
    }
    if( font2 )
    {
        delete font2;
        font2 = NULL;
    }
    if( font3 )
    {
        delete font3;
        font3 = NULL;
    }
    if( font_book )
    {
        delete font_book;
        font_book = NULL;
    }
    if( box )
    {
        delete box;
        box = NULL;
    }
}

// Constructor
PanelContext::PanelContext
(
    wxWindow *parent, 
    wxWindowID id,
    const wxPoint &point,
    const wxSize &siz,
    PanelBoard *pb_,
    CtrlChessTxt *lb_
)
    : wxWindow( parent, id, point, siz, wxNO_BORDER )
{
    resize_ready = false;
    aui_setup = false;
    popup = NULL;
    pb = pb_;
    lb = lb_;

    context_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize );
    wxColour icon_light(255,226,179);
    suggestions_page = new wxPanel(context_notebook, wxID_ANY, wxPoint(0,0), wxSize((90*siz.x)/100,(90*siz.y)/100), wxNO_BORDER );
    /*suggestions_page->*/SetBackgroundColour(icon_light);
    kibitz_page      = new wxPanel(context_notebook, wxID_ANY, wxPoint(0,0), wxSize((90*siz.x)/100,(90*siz.y)/100), wxNO_BORDER );

    // Add pages to notebook
    context_notebook->AddPage(suggestions_page,"Suggestions",true);
    context_notebook->AddPage(kibitz_page,"Engine Analysis",false);

    font1 = NULL;
    font2 = NULL;
    font3 = NULL;
    font_book = NULL;
    box = NULL;
    view_flags_book_moves = true;

    // Create labels
    font1      = new wxFont( 14, wxFONTFAMILY_ROMAN,  wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD,   false );
    font2      = new wxFont( 14, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    font3      = new wxFont( 12, wxFONTFAMILY_SWISS,  wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
    wxPoint pos;
    wxSize  size;

    font_book = new wxFont( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,   false );
    extern CtrlBoxBookMoves *gbl_book_moves;
    book_moves = gbl_book_moves;
    book_moves->SetBackgroundColour( *wxGREEN );
    book_moves->SetBackgroundStyle( wxBG_STYLE_COLOUR );
    book_moves->SetForegroundColour( *wxBLACK );
    book_moves->SetFont( *font_book );
    book_moves->SetTxt("no book moves",false);
    
    // Create Status box
    box = new wxStaticBox( suggestions_page, ID_BOX,"",wxDefaultPosition, wxDefaultSize );
    //box->SetLabel( "Suggestions" );

    // Create flexible buttons
    button1 = new wxButton( suggestions_page, ID_BUTTON1, "Play engine as white" /*longest button text*/,       wxDefaultPosition, wxDefaultSize ); 
    button2 = new wxButton( suggestions_page, ID_BUTTON2, "", wxDefaultPosition, wxDefaultSize );
    button3 = new wxButton( suggestions_page, ID_BUTTON3, "",  wxDefaultPosition, wxDefaultSize );
    button4 = new wxButton( suggestions_page, ID_BUTTON4, "",  wxDefaultPosition, wxDefaultSize );
    wxSize sz = button1->GetSize();
    wxClientDC dc1(button1);
    wxCoord width, height, descent, external_leading;
    dc1.GetTextExtent( "Play engine as white", &width, &height, &descent, &external_leading );
    button_width_delta = sz.x - width;  // used to neatly size button width to suit flexible labels

    // Create Kibitz box
    kibitz_ctrl = new wxListCtrl( kibitz_page, ID_KIBITZ, wxDefaultPosition, wxDefaultSize , BORDER_COMMON|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_HEADER );
    wxListItem col1;
    col1.SetColumn(0);
    kibitz_ctrl->InsertColumn(0,col1);
    wxListItem col2;
    col2.SetColumn(1);
    kibitz_ctrl->InsertColumn(1,col2);
    kibitz_ctrl->InsertItem( 0, "", 0 );
    kibitz_ctrl->InsertItem( 1, "", 0 );
    kibitz_ctrl->InsertItem( 2, "", 0 );
    kibitz_ctrl->InsertItem( 3, "", 0 );
    kibitz_ctrl->InsertItem( 4, "", 0 );
    wxClientDC dc2(kibitz_ctrl);
    dc2.GetTextExtent( "AbWy123456789", &width, &height, &descent, &external_leading );
    dbg_printf( "Kibitz box font height=%d, descent=%d, external_leading=%d\n", height, descent, external_leading );

    // Create kibitz buttons
    kibitz_button1 = new wxButton( kibitz_page, ID_KIBITZ_BUTTON1, "Capture top",  wxDefaultPosition, wxDefaultSize );
    kibitz_button2 = new wxButton( kibitz_page, ID_KIBITZ_BUTTON2, "Capture all",  wxDefaultPosition, wxDefaultSize );
    kibitz_button3 = new wxButton( kibitz_page, ID_KIBITZ_BUTTON3, "Stop",  wxDefaultPosition, wxDefaultSize );

    // Fit everything to size
    Layout( siz );
}

void PanelContext::SetKibitzPage()
{
    context_notebook->ChangeSelection(1);
}

void PanelContext::SetKibitzButtons( bool kibitz_ )
{
    kibitz = kibitz_;
    kibitz_button3->SetLabel( kibitz ? "Stop" : "Start" );
    kibitz_ctrl->Show(true );
    kibitz_button1->Show(true);
    kibitz_button2->Show(true);
    kibitz_button3->Show(true);
}

void PanelContext::ClearStaleKibitz()
{
    kibitz_ctrl->Show(false);
    kibitz_button1->Show(false);
    kibitz_button2->Show(false);
}

void PanelContext::OnTabSelected( wxBookCtrlEvent& event )
{
    int idx = event.GetSelection();
    bool kibitz_selected = (idx==1);
    if( objs.gl )
        objs.gl->KibitzTabChanged( kibitz_selected );
    SetFocusOnList();
}

void PanelContext::OnSize( wxSizeEvent &evt )
{
    wxSize siz = evt.GetSize();
    wxSize sz1;
    sz1.x = siz.x;
    sz1.y = siz.y;
    context_notebook->SetSize(sz1);
    cprintf( "OnSize()\n" );
    if( true )//resize_ready )
        Layout( siz );

	// Check whether AUI moves panel is too small, if it is request a layout fixup later
	//  (experience shows it's unwise to try and do it inside an OnSize() handler)
	if( aui_setup )
	{
		int panel1,panel2,panel3,panel4;
		GetAuiLayout( panel1, panel2, panel3, panel4 );
		if( siz.x>50 && panel2+50>siz.x && objs.gl )
			objs.gl->fix_layout_flag = true;
	}
}

void PanelContext::Layout( wxSize const &siz )
{
    // Position suggestion box and buttons
    PositionSuggestionButtons();

    // Make kibitz buttons the same width
    wxSize button1_size = kibitz_button1->GetSize();    
    wxSize button2_size = kibitz_button2->GetSize();    
    wxSize button3_size = kibitz_button3->GetSize();    
    size_t max_width;
    if( button1_size.x > button2_size.x )
        max_width = (button1_size.x > button3_size.x) ? button1_size.x : button3_size.x;
    else
        max_width = (button2_size.x > button3_size.x) ? button2_size.x : button3_size.x;
    button1_size.x = max_width;
    button2_size.x = max_width;
    button3_size.x = max_width;
    kibitz_button1->SetSize( button1_size);    
    kibitz_button2->SetSize( button2_size);    
    kibitz_button3->SetSize( button3_size);    

    // Size Kibitz box
    wxPoint kpos;
    wxSize  ksiz;
    #define SPACER 5
    kpos.x = SPACER*2;
    kpos.y = SPACER*2;
    ksiz.x = siz.x - button1_size.x - 3*kpos.x;
    ksiz.y = 125; //jaap 99; //sz2.y;
    kibitz_ctrl->SetPosition(kpos);
    kibitz_ctrl->SetSize(ksiz);
    kibitz_ctrl->SetColumnWidth(0,ksiz.x-18);
    kibitz_ctrl->SetColumnWidth(1,4);

    // Position kibitz buttons
    kpos.x += ksiz.x;
    kpos.x += SPACER*2;
    //kpos.y += (ksiz.y - button1_size.y*2 - SPACER*2)/2;

    kibitz_button3->SetPosition(kpos);
    kpos.y += button1_size.y;
    kpos.y += SPACER*2;
    kibitz_button1->SetPosition(kpos);

    kpos.y += button1_size.y;
    kpos.y += SPACER*2;
    kibitz_button2->SetPosition(kpos);
}


void PanelContext::PositionSuggestionButtons() 
{
    int i;
    wxSize sz;
    wxButton *button;
    int x=0, y=0, h=0, w=0;
    int max_width_so_far=0;
    int max_height_so_far=0;
    wxPoint pos(SPACER*2,SPACER*2);
    box->SetPosition( pos );
    x = pos.x + SPACER;
    y = pos.y + SPACER*4;
    w = SPACER;
    h = SPACER*4;

    // Size buttons optimally
    wxClientDC dc1(button1);
    for( i=0; i<4; i++ )
    {
        switch(i)
        {
            default:
            case 0: button = button1;  break;
            case 1: button = button2;  break;
            case 2: button = button3;  break;
            case 3: button = button4;  break;
        }
        wxString ws = button->GetLabel();
        const char *label = ws.c_str();
        wxCoord width, height, descent, external_leading;
        dc1.GetTextExtent( label, &width, &height, &descent, &external_leading );
        wxSize sz2 = button->GetSize();
        sz2.x = width + button_width_delta;
        button->SetSize(sz2);
    }

    // Position buttons
    for( i=0; i<4; i++ )
    {
        switch(i)
        {
            default:
            case 0: button = button1;  break;
            case 1: button = button2;  break;
            case 2: button = button3;  break;
            case 3: button = button4;  break;
        }
        if( button->IsShown() )
        {
            button->SetPosition(wxPoint(x,y));
            sz = button->GetSize();
            w += (sz.x + SPACER);
            x += (sz.x + SPACER);
            if( sz.x > max_width_so_far )
                max_width_so_far = sz.x;
            if( sz.y > max_height_so_far )
                max_height_so_far = sz.y;
        }
    }

    h = max_height_so_far+SPACER*5;
    sz.x = w;
    sz.y = h;
    box->SetSize(sz);
}

void PanelContext::BookUpdate( bool suppress )
{
    std::vector<thc::Move> bmoves;
    bool have_book_moves=false;
    if( objs.gl )
        have_book_moves = objs.book->Lookup( objs.gl->gd.master_position, bmoves );
    else // during startup
    {
        thc::ChessPosition temp;
        have_book_moves = objs.book->Lookup( temp, bmoves );
    }
    if( suppress )
        have_book_moves = false;
    view_flags_book_moves = have_book_moves;
    if( !view_flags_book_moves )
        book_moves->Show(false);
    else
    {
        wxString txt;
        int n = bmoves.size();
        if( n == 1 )
            txt = "1 book move";
        else
            txt.Printf( "%d book moves", n );
        const char *s = txt.c_str();
        cprintf( "Book moves updated: %s\n", s );
        book_moves->Show(true);
        book_moves->SetTxt(txt);
    }
}

void PanelContext::ButtonCmd( int cmd )
{
    switch( cmd )
    {
        case ID_CMD_FLIP:           objs.gl->CmdFlip();         break;
        case ID_CMD_SET_POSITION:   objs.gl->CmdSetPosition();  break;
        case ID_CMD_NEW_GAME:       objs.gl->CmdNewGame();      break;
        case ID_CMD_TAKEBACK:       objs.gl->CmdTakeback();     break;
        case ID_CMD_MOVENOW:        objs.gl->CmdMoveNow();      break;
        case ID_CMD_EXAMINE_GAME:   objs.gl->CmdExamineGame();  break;
        case ID_CMD_WHITE_RESIGNS:  objs.gl->CmdWhiteResigns(); break;
        case ID_CMD_BLACK_RESIGNS:  objs.gl->CmdBlackResigns(); break;
        case ID_CMD_DRAW:           objs.gl->CmdDraw();         break;
        case ID_CMD_PLAY_WHITE:     objs.gl->CmdPlayWhite();    break;
        case ID_CMD_PLAY_BLACK:     objs.gl->CmdPlayBlack();    break;
        case ID_CMD_SWAP_SIDES:     objs.gl->CmdSwapSides();    break;    
        case ID_CMD_NEXT_GAME:      objs.gl->CmdNextGame();     break;
        case ID_CMD_PREVIOUS_GAME:  objs.gl->CmdPreviousGame(); break;
        case ID_DATABASE_SEARCH:    objs.gl->CmdDatabaseSearch();   break;
        case ID_FILE_TAB_NEW:        objs.gl->CmdTabNew();       break;
        case ID_FILE_TAB_CLOSE:      objs.gl->CmdTabClose();     break;
        case ID_FILE_TAB_INCLUDE:    objs.gl->CmdTabInclude();   break;
        case wxID_NEW:              objs.gl->CmdFileNew();      break;
        case wxID_OPEN:             objs.gl->CmdFileOpen();     break;
        case wxID_SAVE:             objs.gl->CmdFileSave();     break;
        case wxID_UNDO:             objs.gl->CmdEditUndo();     break;
        case wxID_REDO:             objs.gl->CmdEditRedo();     break;
    }
}

void PanelContext::OnButton1( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button1_cmd );
}

void PanelContext::OnButton2( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button2_cmd );
}

void PanelContext::OnButton3( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button3_cmd );
}

void PanelContext::OnButton4( wxCommandEvent& WXUNUSED(event) )
{
    ButtonCmd( button4_cmd );
}

void PanelContext::OnKibitzButton1( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureOne();
}

void PanelContext::OnKibitzButton2( wxCommandEvent& WXUNUSED(event) )
{
    objs.gl->CmdKibitzCaptureAll();
}

void PanelContext::OnKibitzButton3( wxCommandEvent& WXUNUSED(event) )
{
    if( kibitz )
        objs.gl->KibitzStop();
    else
        objs.gl->KibitzStart();
    SetFocusOnList();
}

void PanelContext::Kibitz( int idx, const wxString &txt )
{
    switch( idx )
    {
        default:
        case 0:
        {
            if( str_kibitz0 != txt )
            {
                str_kibitz0 = txt;
                kibitz_ctrl->SetItem( 0, 0, txt );
            }
            break;
        }
        case 1:
        {
            if( str_kibitz1 != txt )
            {
                str_kibitz1 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz1 = %s\n", temp );
                //kibitz1->SetLabel(txt);
                kibitz_ctrl->SetItem( 1, 0, txt );
            }
            break;
        }
        case 2:
        {
            if( str_kibitz2 != txt )
            {
                str_kibitz2 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz2 = %s\n", temp );
                //kibitz2->SetLabel(txt);
                kibitz_ctrl->SetItem( 2, 0, txt );
            }
            break;
        }
        case 3:
        {
            if( str_kibitz3 != txt )
            {
                str_kibitz3 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz3 = %s\n", temp );
                //kibitz3->SetLabel(txt);
                kibitz_ctrl->SetItem( 3, 0, txt );
            }
            break;
        }
        case 4:
        {
            if( str_kibitz4 != txt )
            {
                str_kibitz4 = txt;
                const char *temp = txt.c_str();
                release_printf( "kibitz4 = %s\n", temp );
                //kibitz4->SetLabel(txt);
                kibitz_ctrl->SetItem( 4, 0, txt );
            }
            break;
        }
    }
}

void PanelContext::KibitzScroll( const wxString &txt )
{
    wxString line4 = str_kibitz3;
    wxString line3 = str_kibitz2;
    wxString line2 = str_kibitz1;
    wxString line1 = txt;
    Kibitz( 1, line1 );
    Kibitz( 2, line2 );
    Kibitz( 3, line3 );
    Kibitz( 4, line4 );
}

// AUI Pane layout is a bit of a mystery. Experience shows panel1 is height of skinny top
// pane, panel2 is width of left (board) pane, panel4 is height of bottom (context) pane
// You might think panel 3 would be width of right (moves) pane - but actually that is
// the frame width minus the board pane width - basically panel3 is don't care

/*
                                                +--- first parameter is direction 
                                                |
                                                v
    wxAUI_DOCK_TOP = 1,               dock_size(1,1,0)    panel1 top
    wxAUI_DOCK_RIGHT = 2,
    wxAUI_DOCK_BOTTOM = 3,            dock_size(3,1,0)    panel4 bottom
    wxAUI_DOCK_LEFT = 4,              dock_size(4,1,0)    panel2 left
    wxAUI_DOCK_CENTER = 5,            dock_size(5,1,0)    panel3 centre

    Note that it is correct for panel3 be the "centre" rather than "right"
    If "right" it becomes independently sizable, if "centre" it is adjacent
    to "left", which is what we want.

*/

void PanelContext::GetAuiLayout( int &panel1, int &panel2, int &panel3, int &panel4 )
{
    panel1=0, panel2=0, panel3=0, panel4=0;
    wxString persp = m_mgr.SavePerspective();
    const char *txt = persp.c_str();
	cprintf( "Get AUI Layout: %s\n", txt );
    std::string s(txt);
    int len = strlen("dock_size(1,1,0)=");
    size_t idx = s.find( "dock_size(1,1,0)=" );
    if( idx != std::string::npos )
        panel1 = atoi(txt+idx+len);
    idx = s.find( "dock_size(4,1,0)=" );
    if( idx != std::string::npos )
        panel2 = atoi(txt+idx+len);
    idx = s.find( "dock_size(5,1,0)=" );
    if( idx != std::string::npos )
        panel3 = atoi(txt+idx+len);
    idx = s.find( "dock_size(3,1,0)=" );
    if( idx != std::string::npos )
        panel4 = atoi(txt+idx+len);
}

void PanelContext::SetAuiLayout( int panel1, int panel2, int panel3, int panel4 )
{
    wxString persp = m_mgr.SavePerspective();
    const char *txt = persp.c_str();
	cprintf( "Get AUI Layout: %s\n", txt );
    std::string s(txt);
    size_t idx1 = s.find( "dock_size(1,1,0)=" );
    size_t idx2 = s.find( "dock_size(4,1,0)=" );
    size_t idx3 = s.find( "dock_size(5,1,0)=" );
    size_t idx4 = s.find( "dock_size(3,1,0)=" );
    if( idx1 != std::string::npos &&
        idx2 != std::string::npos &&
        idx3 != std::string::npos &&
        idx4 != std::string::npos &&
        idx2>idx1 && idx3>idx2 && idx4>idx3
        )
    {
        char temp[100];
		wxSize sz = GetSize();
		panel3 = sz.x - panel2;		// Just insist panel3 has a sensible value
		if( panel3 < 20 )
			panel3 = 20;
        sprintf( temp, "dock_size(1,1,0)=%d|dock_size(4,1,0)=%d|dock_size(5,1,0)=%d|dock_size(3,1,0)=%d|", panel1, panel2, panel3, panel4 );
        std::string s2 = s.substr(0,idx1) + std::string(temp);
        wxString persp2(s2.c_str());
		cprintf( "Set AUI Layout: %s\n", s2.c_str() );
        m_mgr.LoadPerspective( persp2, true );
    }
}

void PanelContext::AuiFixLayout()
{
	wxSize siz;
	siz = GetSize();
	int panel1,panel2,panel3,panel4;
	GetAuiLayout( panel1, panel2, panel3, panel4 );
	if( siz.x>60 && panel2+50>siz.x )
	{
		panel2 = siz.x-60;
		SetAuiLayout(panel1,panel2,panel3,panel4);
	}
}

void PanelContext::AuiBegin( wxFrame *frame, wxWindow *top, wxWindow *left, wxWindow *right, wxWindow *bottom, bool restore )
{
    // notify wxAUI which frame to use
    m_mgr.SetManagedWindow(frame);
    m_mgr.SetDockSizeConstraint(0.9,0.9);
 
    // add the panes to the manager Note: Experience shows there is no point trying to change a panel's fundamental characteristics
    //  after adding it to the manager - Things like CaptionVisible(false) etc need to be intantiated with the panel
    m_mgr.AddPane(top,
                  wxAuiPaneInfo().
                  Name("top").CaptionVisible(false). //(wxT("Tree Pane")).
                  Top().Layer(1).Position(0).DockFixed().
                 // Top().Layer(1).Position(0).Fixed().Resizable(false).
                  CloseButton(false).MaximizeButton(false));
    m_mgr.AddPane(left,
                  wxAuiPaneInfo().MinSize(200,200).
                  Name("left").CaptionVisible(false). //(wxT("Tree Pane")).
                  Left().Layer(1).Position(1).
                  CloseButton(false).MaximizeButton(false));
    m_mgr.AddPane(right,
                  wxAuiPaneInfo().
				  Name("right").Center().Layer(1).Position(2).
				  CloseButton(false).CaptionVisible(false).MinSize(50,50) );
    m_mgr.AddPane(bottom,
                  wxAuiPaneInfo().
                  Name("bottom").CaptionVisible(false). //(wxT("Tree Pane")).
                  Bottom().Layer(1).Position(3).
                  CloseButton(false).MaximizeButton(false));


    // tell the manager to "commit" all the changes just made
    m_mgr.Update();

    // If we restored a non-volatile window size and location, try to restore non-volatile panel
    //  locations
    if( restore )
    {
        int panel1 = objs.repository->nv.m_panel1;
        int panel2 = objs.repository->nv.m_panel2;
        int panel3 = objs.repository->nv.m_panel3;
        int panel4 = objs.repository->nv.m_panel4;
        if( panel1>0 && panel2>0 && panel3>0 && panel4>0 )
		{
			SetAuiLayout( panel1, panel2, panel3, panel4 );
		}
    }

	aui_setup = true;
}


void PanelContext::AuiEnd()
{
	// Save the panel layout for next time
	int panel1=0, panel2=0, panel3=0, panel4=0;
	GetAuiLayout( panel1, panel2, panel3, panel4 );
	if( panel1>0 && panel2>0 && panel3>0 && panel4>0 )
	{
		objs.repository->nv.m_panel1 = panel1;
		objs.repository->nv.m_panel2 = panel2;
		objs.repository->nv.m_panel3 = panel3;
		objs.repository->nv.m_panel4 = panel4;
	}
	m_mgr.UnInit(); // deinitialize the frame manager
}
