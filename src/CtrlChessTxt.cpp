/****************************************************************************
 * Control - Chess move entry
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/clipbrd.h"
#include "Appdefs.h"
#include "GameLogic.h"
#include "thc.h"
#include "DebugPrintf.h"
#include "Objects.h"
#include "Repository.h"
#include "GameDocument.h"
#include "CtrlChessTxt.h"

// Create a custom event
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND, 7777)
END_DECLARE_EVENT_TYPES()
DEFINE_EVENT_TYPE(wxEVT_MY_CUSTOM_COMMAND)

// It may also be convenient to define an event table macro for this event type
#define EVT_MY_CUSTOM_COMMAND(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        wxEVT_MY_CUSTOM_COMMAND, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &fn ), \
        (wxObject *) NULL \
    ),


CtrlChessTxt::CtrlChessTxt
(
    wxWindow* parent,
    wxWindowID     id, //= wxID_ANY,
    const wxPoint &point,
    const wxSize &size
)  : wxRichTextCtrl( parent, id, "", point, size,   wxNO_BORDER+wxWANTS_CHARS+wxRE_MULTILINE+wxRE_READONLY )

{
    gl = 0;
    context_menu = 0;
    bool large_font = objs.repository->general.m_large_font;
    wxFont temp(*wxNORMAL_FONT);
    this->font_normal = temp;
    temp.SetPointSize(temp.GetPointSize() + 2);
    this->font_large  = temp;
    SetLargeFont( large_font ); // SetFont(*wxNORMAL_FONT);
    //GetBuffer().SetFloatingLayoutMode(false); attempt to speed up
}

void CtrlChessTxt::SetGameDocument( GameDocument *gd_ )
{
    this->gd = gd_;
    unsigned long pos = gd_->non_zero_start_pos ? gd_->non_zero_start_pos : gd_->gv.FindMove0();
    if( gl )
        gl->atom.Redisplay(pos);
    cprintf( "SetGameDocument() pos = %lu\n", pos );
}

void CtrlChessTxt::OnProcessCustom(wxCommandEvent& WXUNUSED(event) )
{
    if( gl->IsManual() )
    {
        unsigned long pos = GetInsertionPoint();
        Goto(pos);
    }
}

void CtrlChessTxt::OnMouseMove( wxMouseEvent& WXUNUSED(event) )
{
}

void CtrlChessTxt::OnMouseLeftUp( wxMouseEvent& event )
{
    event.Skip();   //continue to process event
}

void CtrlChessTxt::OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
{
    // intentionally empty
}

void CtrlChessTxt::OnSize( wxSizeEvent& event )
{
    event.Skip();   //continue to process event
/*    wxSize sz = event.GetSize();
    ((wxRichTextCtrl *)this)->SetSize(sz);
    Refresh();
    Update(); */
}

void CtrlChessTxt::OnMouseLeftDown( wxMouseEvent& ev )
{
    gl->SetManual(NULL,false);
    ev.Skip();   //continue to process event
}

void CtrlChessTxt::OnContext( wxContextMenuEvent &event )
{
    thc::ChessRules cr;
    std::string move_txt;
    GAME_MOVE *gm = gd->GetSummaryMove( cr, move_txt );
    popup_mt = gd->GetSummary();
    if( gm && popup_mt && move_txt.length() )
    {
        if( context_menu )
        {
            delete context_menu;
            context_menu = 0;
        }
        context_menu = new wxMenu;
        context_menu->SetTitle( move_txt.c_str() );
        context_menu->Append(ID_ANNOT1, "!"  );    // $1 
        context_menu->Append(ID_ANNOT2, "?"  );    // $2 
        context_menu->Append(ID_ANNOT3, "!!" );    // $3 
        context_menu->Append(ID_ANNOT4, "??" );    // $4 
        context_menu->Append(ID_ANNOT5, "!?" );    // $5 
        context_menu->Append(ID_ANNOT6, "?!" );    // $6 
        context_menu->Append(ID_ANNOT7,"None of the above");
        context_menu->AppendSeparator();
        context_menu->Append(ID_ANNOT10, "="  );   // $10 
        context_menu->Append(ID_ANNOT14, "+=" );   // $14 
        context_menu->Append(ID_ANNOT15, "=+" );   // $15 
        context_menu->Append(ID_ANNOT16, "+/-");   // $16 
        context_menu->Append(ID_ANNOT17, "-/+");   // $17 
        context_menu->Append(ID_ANNOT18, "+-" );   // $18 
        context_menu->Append(ID_ANNOT19, "-+" );   // $19 
        context_menu->Append(ID_ANNOT20,"None of the above");
        wxPoint pos = event.GetPosition();
//        wxPoint pos2 = GetParent()->GetPosition();
//        pos.x -= pos2.x;
//        pos.y -= pos2.y;
        pos = ScreenToClient( pos );
        pos.x += 4;
        PopupMenu(context_menu, pos );
    }
}

void CtrlChessTxt::OnMouseEvent( wxMouseEvent& ev )
{
    if( gl->IsManual() )
    {
        unsigned long pos = GetInsertionPoint();
        Goto(pos,true);
        ev.Skip();   //continue to process event
    }
}

void CtrlChessTxt::OnMouseWheel( wxMouseEvent &ev )
{
    Atomic begin(false);
    wxPoint pt( ev.GetPosition() );
    int rotation = ev.GetWheelRotation();
    //CalcUnscrolledPosition( pt.x, pt.y, &x, &y );
    dbg_printf( "Mouse wheel event; Rotation: %d, delta = %d\n",
                  rotation, ev.GetWheelDelta() );
    bool up = rotation>0;
    NAVIGATION_KEY nk  = (up ? NK_UP : NK_DOWN);
    NavigationKey( nk );
}

void CtrlChessTxt::NavigationKey( NAVIGATION_KEY nk )
{
    unsigned long pos1 = gl->atom.GetInsertionPoint();
    unsigned long pos2 = pos1;
    pos2 = gd->NavigationKey(pos1,nk);
    if( pos2 != pos1 )
    {
        gl->atom.SetInsertionPoint(pos2);
        Goto(pos2);
    }
}

void CtrlChessTxt::Goto( unsigned long pos, bool from_mouse_move )
{
    static bool old_from_mouse_move;
    static unsigned long old_pos;
    static thc::ChessRules old_cr;
    static std::string old_title;
    static bool old_at_move0;
    thc::ChessRules cr;
    std::string title;
    bool at_move0;
    MoveTree *mt = gd->Locate(pos,cr,title,at_move0);
    bool unchanged = (from_mouse_move && old_from_mouse_move && pos==old_pos && cr==old_cr && title==old_title && at_move0==old_at_move0);
    if( !unchanged )
    {
        gl->gd.master_position = cr;
        gl->SetManual( mt, at_move0, from_mouse_move );
        objs.canvas->SetChessPosition( cr );
        objs.canvas->SetBoardTitle(title.c_str());
    }
    old_from_mouse_move = from_mouse_move;
    old_pos      =pos;
    old_cr       =cr;
    old_title    =title;
    old_at_move0 =at_move0;
}

void CtrlChessTxt::OnSetFocus(wxFocusEvent& event)
{
    event.Skip();
}

void CtrlChessTxt::OnKillFocus(wxFocusEvent& event)
{
    event.Skip();
}

void CtrlChessTxt::OnText(wxCommandEvent& WXUNUSED(event))
{
}

void CtrlChessTxt::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
}

void CtrlChessTxt::OnTextCut(wxClipboardTextEvent& WXUNUSED(event))
{
}

void CtrlChessTxt::OnTextCopy(wxClipboardTextEvent& WXUNUSED(event))
{
    dbg_printf( "Copy\n" );
}

void CtrlChessTxt::OnTextPaste(wxClipboardTextEvent& WXUNUSED(event))
{
    dbg_printf( "Paste 2\n" );
}

void CtrlChessTxt::Paste()
{
    Atomic begin;
    dbg_printf( "Paste 1\n" );
    if( wxTheClipboard->Open() )
    {
        if( wxTheClipboard->IsSupported( wxDF_TEXT ) )
        {
            wxTextDataObject data;
            wxTheClipboard->GetData( data );
            wxString txt_to_paste = data.GetText();
            std::string txt_to_insert;
            for( unsigned int i=0; i<txt_to_paste.Length(); i++ )
            {
                char c=txt_to_paste[i];
                if( c == '\n' )
                    txt_to_insert += ' ';
                else if( c != '\r' )
                    txt_to_insert += c;
            }
            if( gd->IsSelectionInComment(this) )
                gd->DeleteSelection(this);
            else
            {
                long pos=GetInsertionPoint();
                SetSelection(pos,pos);
            }
            if( !gd->PromotePaste(txt_to_insert) )
                gd->CommentEdit(this,txt_to_insert);
        }  
        wxTheClipboard->Close();
    }
}

void CtrlChessTxt::Copy()
{
    wxString txt=GetStringSelection();
    if( txt.Length() )
    {
        if( wxTheClipboard->Open() )
        {
            //if( wxTheClipboard->IsSupported( wxDF_TEXT ) )
                wxTheClipboard->SetData( new wxTextDataObject(txt) );
            wxTheClipboard->Close();
        }
    }
}

void CtrlChessTxt::Cut()
{
    Atomic begin;
    if( gd->IsSelectionInComment(this) )
    {
        Copy();
        gd->DeleteSelection(this);
    }
}

void CtrlChessTxt::Delete()
{
    Atomic begin;
    long pos1, pos2;
    GetSelection( &pos1, &pos2 );
    bool is_selection = (pos1!=pos2);
    bool is_selection_in_comment = gd->IsSelectionInComment(this);
    if( is_selection_in_comment )
        gd->DeleteSelection(this);
    else if( !is_selection )
    {
        bool done=gd->CommentEdit(this,WXK_DELETE);
        if( !done )
            gd->DeleteRestOfVariation();
    }
}

void CtrlChessTxt::OnTextURL(wxTextUrlEvent& WXUNUSED(event) )
{
}

void CtrlChessTxt::OnTextMaxLen(wxCommandEvent& WXUNUSED(event) )
{
}

void CtrlChessTxt::OnChar(wxKeyEvent& event)
{
    Atomic begin(false);
    NAVIGATION_KEY nk=NK_NULL;
    bool done=false;
    bool request_post_key_update = false;
    bool shift=false;
    long pos1, pos2;
    GetSelection( &pos1, &pos2 );
    bool is_selection = (pos1!=pos2);
    bool is_selection_in_comment = gd->IsSelectionInComment(this);
    if( event.GetModifiers() & wxMOD_SHIFT )
        shift = true;
    long keycode = event.GetKeyCode();
    //cprintf( "OnChar() keycode=%lu\n", keycode );
    {
        switch ( keycode )
        {
            case WXK_NUMPAD_DELETE: // fall-thru
            case WXK_DELETE:
            {
                if( !shift )
                {
                    if( is_selection_in_comment )
                    {
                        gd->DeleteSelection(this);
                        done = true;
                    }
                    else if( is_selection )
                        done = true;
                    else
                    {
                        done = gd->CommentEdit(this,WXK_DELETE);
                        nk=NK_DELETE;
                    }
                }
                break;
            }
            case WXK_PAGEUP:                nk=NK_PGUP;     break;
            case WXK_PAGEDOWN:              nk=NK_PGDOWN;   break;
            case WXK_HOME:                  nk=NK_HOME;     break;
            case WXK_END:                   nk=NK_END;      break;
            case WXK_NUMPAD_LEFT:           nk=NK_LEFT;     break;
            case WXK_NUMPAD_UP:             nk=NK_UP;       break;
            case WXK_NUMPAD_RIGHT:          nk=NK_RIGHT;    break;
            case WXK_NUMPAD_DOWN:           nk=NK_DOWN;     break;
            case WXK_NUMPAD_HOME:           nk=NK_HOME;     break;
            case WXK_NUMPAD_PAGEUP:         nk=NK_PGUP;     break;
            case WXK_NUMPAD_PAGEDOWN:       nk=NK_PGDOWN;   break;
            case WXK_NUMPAD_END:            nk=NK_END;      break;
#ifdef THC_MAC
            case '[':
#endif
            case WXK_LEFT:
            {
                if(!shift)
                    done = gd->CommentEdit(this,WXK_LEFT);
                nk=NK_LEFT;
                break;
            }
#ifdef THC_MAC
            case ']':
#endif
            case WXK_RIGHT:
            {
                if( !shift )
                    done = gd->CommentEdit(this,WXK_RIGHT);
                nk=NK_RIGHT;
                break;
            }
            case WXK_UP:
            {
                bool in_comment = gd->IsInComment(this);
                if( !shift && !in_comment )
                    nk=NK_UP;
                else if( in_comment )
                    request_post_key_update = true;
                break;
            }
            case WXK_DOWN:
            {
                bool in_comment = gd->IsInComment(this);
                if( !shift && !in_comment )
                    nk=NK_DOWN;
                else if( in_comment )
                    request_post_key_update = true;
                break;
            }
            case WXK_BACK:
            {
                if( is_selection_in_comment )
                {
                    gd->DeleteSelection(this);
                    done = true;
                }
                else
                {
                    done=gd->CommentEdit(this,WXK_BACK);
                }
                break;
            }
            case WXK_SPACE:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,' ');
                break;
            }
            case WXK_NUMPAD0:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'0');
                break;
            }
            case WXK_NUMPAD1:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'1');
                break;
            }
            case WXK_NUMPAD2:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'2');
                break;
            }
            case WXK_NUMPAD3:   
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'3');
                break;
            }
            case WXK_NUMPAD4: 
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'4');
                break;
            }
            case WXK_NUMPAD5:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'5');
                break;
            }
            case WXK_NUMPAD6:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'6');
                break;
            }
            case WXK_NUMPAD7:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'7');
                break;
            }
            case WXK_NUMPAD8:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'8');
                break;
            }
            case WXK_NUMPAD9:
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'9');
                break;
            }
            case WXK_MULTIPLY:  
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'*');
                break;
            }
            case WXK_ADD:       
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'+');
                break;
            }
            case WXK_SUBTRACT:  
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'-');
                break;
            }
            case WXK_DECIMAL:   
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'.');
                break;
            }
            case WXK_DIVIDE:    
            {
                if( is_selection_in_comment )
                    gd->DeleteSelection(this);
                done=gd->CommentEdit(this,'\\');
                break;
            }
            case WXK_TAB:       
            case WXK_RETURN:    
            case WXK_ESCAPE:    
            case WXK_START:     
            case WXK_LBUTTON:   
            case WXK_RBUTTON:   
            case WXK_CANCEL:    
            case WXK_MBUTTON:   
            case WXK_CLEAR:     
            case WXK_SHIFT:     
            case WXK_ALT:       
            case WXK_CONTROL:   
            case WXK_MENU:      
            case WXK_PAUSE:     
            case WXK_CAPITAL:   
            case WXK_SELECT:    
            case WXK_PRINT:     
            case WXK_EXECUTE:   
            case WXK_SNAPSHOT:  
            case WXK_INSERT:    
            case WXK_HELP:      
            case WXK_SEPARATOR: 
            case WXK_F1:  
            case WXK_F2:  
            case WXK_F3:  
            case WXK_F4:  
            case WXK_F5:  
            case WXK_F6:  
            case WXK_F7:  
            case WXK_F8:  
            case WXK_F9:  
            case WXK_F10: 
            case WXK_F11: 
            case WXK_F12: 
            case WXK_F13: 
            case WXK_F14: 
            case WXK_F15: 
            case WXK_F16: 
            case WXK_F17: 
            case WXK_F18: 
            case WXK_F19: 
            case WXK_F20: 
            case WXK_F21: 
            case WXK_F22: 
            case WXK_F23: 
            case WXK_F24: 
            case WXK_NUMLOCK:           
            case WXK_SCROLL:            
            case WXK_NUMPAD_SPACE:      
            case WXK_NUMPAD_TAB:        
            case WXK_NUMPAD_ENTER:      
            case WXK_NUMPAD_F1:         
            case WXK_NUMPAD_F2:         
            case WXK_NUMPAD_F3:         
            case WXK_NUMPAD_F4:         
            case WXK_NUMPAD_BEGIN:      
            case WXK_NUMPAD_INSERT:     
            case WXK_NUMPAD_EQUAL:      
            case WXK_NUMPAD_MULTIPLY:   
            case WXK_NUMPAD_ADD:        
            case WXK_NUMPAD_SEPARATOR:  
            case WXK_NUMPAD_SUBTRACT:   
            case WXK_NUMPAD_DECIMAL:
            {
                break;
            }
            default:
            {
                bool iso8859_extended_charset = (0xa0<=keycode && keycode<=0xff);
                if( iso8859_extended_charset || wxIsprint((int)keycode) )
                {
                    if( is_selection_in_comment )
                        gd->DeleteSelection(this);
                    done=gd->CommentEdit(this,keycode); 
                }
            }
        }
    }
    if( !shift && !done && nk!=NK_NULL )
    {
        if( nk == NK_DELETE )
            gd->DeleteRestOfVariation();
        else
            NavigationKey(nk);
    }
    else if( !done )
    {
        event.Skip();   //continue to process event
        if( request_post_key_update )
        {
            wxCommandEvent eventCustom(wxEVT_MY_CUSTOM_COMMAND);
            wxPostEvent(this, eventCustom);
        }
    }
}

void CtrlChessTxt::OnKeyUp(wxKeyEvent& event)
{
    event.Skip();   //continue to process event
}

void CtrlChessTxt::OnKeyDown(wxKeyEvent& event)
{
    Atomic begin(false);
    NAVIGATION_KEY nk=NK_NULL;
    bool done=false;
    bool request_post_key_update = false;
    bool shift=false;
    long pos1, pos2;
    GetSelection( &pos1, &pos2 );
    bool is_selection = (pos1!=pos2);
    bool is_selection_in_comment = gd->IsSelectionInComment(this);
    if( event.GetModifiers() & wxMOD_SHIFT )
        shift = true;
    long keycode = event.GetKeyCode();
    //cprintf( "OnKeyDown() keycode=%lu\n", keycode );
    {
        switch ( keycode )
        {
            case WXK_NUMPAD_DELETE: // fall-thru
            case WXK_DELETE:
            {
                if( !shift )
                {
                    if( is_selection_in_comment )
                    {
                        gd->DeleteSelection(this);
                        done = true;
                    }
                    else if( is_selection )
                        done = true;
                    else
                    {
                        done = gd->CommentEdit(this,WXK_DELETE);
                        nk=NK_DELETE;
                    }
                }
                break;
            }
            case WXK_PAGEUP:                nk=NK_PGUP;     break;
            case WXK_PAGEDOWN:              nk=NK_PGDOWN;   break;
            case WXK_HOME:                  nk=NK_HOME;     break;
            case WXK_END:                   nk=NK_END;      break;
            case WXK_NUMPAD_LEFT:           nk=NK_LEFT;     break;
            case WXK_NUMPAD_UP:             nk=NK_UP;       break;
            case WXK_NUMPAD_RIGHT:          nk=NK_RIGHT;    break;
            case WXK_NUMPAD_DOWN:           nk=NK_DOWN;     break;
            case WXK_NUMPAD_HOME:           nk=NK_HOME;     break;
            case WXK_NUMPAD_PAGEUP:         nk=NK_PGUP;     break;
            case WXK_NUMPAD_PAGEDOWN:       nk=NK_PGDOWN;   break;
            case WXK_NUMPAD_END:            nk=NK_END;      break;
            case WXK_LEFT:
            {
                if(!shift)
                    done = gd->CommentEdit(this,WXK_LEFT);
                nk=NK_LEFT;
                break;
            }
            case WXK_RIGHT:
            {
                if( !shift )
                    done = gd->CommentEdit(this,WXK_RIGHT);
                nk=NK_RIGHT;
                break;
            }
            case WXK_UP:
            {
                bool in_comment = gd->IsInComment(this);
                if( !shift && !in_comment )
                    nk=NK_UP;
                else if( in_comment )
                    request_post_key_update = true;
                break;
            }
            case WXK_DOWN:
            {
                bool in_comment = gd->IsInComment(this);
                if( !shift && !in_comment )
                    nk=NK_DOWN;
                else if( in_comment )
                    request_post_key_update = true;
                break;
            }
        }
    }
    if( !shift && !done && nk!=NK_NULL )
    {
        if( nk == NK_DELETE )
            gd->DeleteRestOfVariation();
        else
            NavigationKey(nk);
    }
    else if( !done )
    {
        event.Skip();   //continue to process event
        if( request_post_key_update )
        {
            wxCommandEvent eventCustom(wxEVT_MY_CUSTOM_COMMAND);
            wxPostEvent(this, eventCustom);
        }
    }
}

void CtrlChessTxt::OnAnnotNag1( int nag1 )
{
    Atomic atomic;
    popup_mt->game_move.nag_value1 = nag1;
    gl->gd.Rebuild();
    gl->atom.Undo();
    gl->atom.Redisplay( gl->gd.gv.GetMoveOffset(popup_mt) );
}

void CtrlChessTxt::OnAnnotNag2( int nag2 )
{
    Atomic atomic;
    popup_mt->game_move.nag_value2 = nag2;
    gl->gd.Rebuild();
    gl->atom.Undo();
    gl->atom.Redisplay( gl->gd.gv.GetMoveOffset(popup_mt) );
}

void CtrlChessTxt::OnUndo(wxCommandEvent& WXUNUSED(event))
{
    gl->CmdEditUndo();
}

void CtrlChessTxt::OnUpdateUndo( wxUpdateUIEvent &event )
{
    bool enabled = gl->CmdUpdateEditUndo();
    event.Enable(enabled);
}

void CtrlChessTxt::OnRedo(wxCommandEvent& WXUNUSED(event))
{
    gl->CmdEditRedo();
}

void CtrlChessTxt::OnUpdateRedo( wxUpdateUIEvent &event )
{
    bool enabled = gl->CmdUpdateEditRedo();
    event.Enable(enabled);
}

void CtrlChessTxt::OnAnnot1(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(1);
}

void CtrlChessTxt::OnAnnot2(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(2);
}

void CtrlChessTxt::OnAnnot3(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(3);
}

void CtrlChessTxt::OnAnnot4(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(4);
}

void CtrlChessTxt::OnAnnot5(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(5);
}

void CtrlChessTxt::OnAnnot6(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(6);
}

void CtrlChessTxt::OnAnnot7(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag1(0);
}

void CtrlChessTxt::OnAnnot10(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(10);
}

void CtrlChessTxt::OnAnnot14(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(14);
}

void CtrlChessTxt::OnAnnot15(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(15);
}

void CtrlChessTxt::OnAnnot16(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(16);
}

void CtrlChessTxt::OnAnnot17(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(17);
}

void CtrlChessTxt::OnAnnot18(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(18);
}

void CtrlChessTxt::OnAnnot19(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(19);
}

void CtrlChessTxt::OnAnnot20(wxCommandEvent& WXUNUSED(event))
{
    OnAnnotNag2(0);
}


BEGIN_EVENT_TABLE(CtrlChessTxt, wxRichTextCtrl)
//    EVT_PAINT(CtrlChessTxt::OnPaint)
      EVT_SIZE(CtrlChessTxt::OnSize)
//    EVT_MOUSE_EVENTS(CtrlChessTxt::OnMouseEvent)
//    EVT_LEFT_UP (CtrlChessTxt::OnMouseLeftUp)
//    EVT_LEFT_DOWN (CtrlChessTxt::OnMouseLeftDown)
//    EVT_MOTION (CtrlChessTxt::OnMouseMove)
//    EVT_ERASE_BACKGROUND(CtrlChessTxt::OnEraseBackground)

    // 13 event handlers
    EVT_KEY_DOWN(CtrlChessTxt::OnKeyDown)
    EVT_KEY_UP(CtrlChessTxt::OnKeyUp)
    EVT_CHAR(CtrlChessTxt::OnChar)
    EVT_MENU (wxID_UNDO,           CtrlChessTxt::OnUndo)
        EVT_UPDATE_UI (wxID_UNDO,           CtrlChessTxt::OnUpdateUndo)
    EVT_MENU (wxID_REDO,           CtrlChessTxt::OnRedo)
        EVT_UPDATE_UI (wxID_REDO,           CtrlChessTxt::OnUpdateRedo)
    EVT_TEXT(wxID_ANY, CtrlChessTxt::OnText)
    EVT_TEXT_ENTER(wxID_ANY, CtrlChessTxt::OnTextEnter)
    EVT_TEXT_URL(wxID_ANY, CtrlChessTxt::OnTextURL)
    EVT_TEXT_MAXLEN(wxID_ANY, CtrlChessTxt::OnTextMaxLen)
    EVT_TEXT_CUT(wxID_CUT,   CtrlChessTxt::OnTextCut)
    EVT_TEXT_COPY(wxID_COPY,  CtrlChessTxt::OnTextCopy)
    EVT_TEXT_PASTE(wxID_PASTE, CtrlChessTxt::OnTextPaste)
    EVT_LEFT_DOWN (CtrlChessTxt::OnMouseLeftDown)
    EVT_MOUSEWHEEL( CtrlChessTxt::OnMouseWheel )
    EVT_MOUSE_EVENTS(CtrlChessTxt::OnMouseEvent)
    EVT_SET_FOCUS(CtrlChessTxt::OnSetFocus)
    EVT_KILL_FOCUS(CtrlChessTxt::OnKillFocus)
    EVT_CONTEXT_MENU(CtrlChessTxt::OnContext)
    EVT_MY_CUSTOM_COMMAND(wxID_ANY, CtrlChessTxt::OnProcessCustom)
    EVT_MENU( ID_ANNOT1, CtrlChessTxt::OnAnnot1 ) //"!"  );    // $1 
    EVT_MENU( ID_ANNOT2, CtrlChessTxt::OnAnnot2 ) //"?"  );    // $2 
    EVT_MENU( ID_ANNOT3, CtrlChessTxt::OnAnnot3 ) //"!!" );    // $3 
    EVT_MENU( ID_ANNOT4, CtrlChessTxt::OnAnnot4 ) //"??" );    // $4 
    EVT_MENU( ID_ANNOT5, CtrlChessTxt::OnAnnot5 ) //"!?" );    // $5 
    EVT_MENU( ID_ANNOT6, CtrlChessTxt::OnAnnot6 ) //"?!" );    // $6 
    EVT_MENU( ID_ANNOT7, CtrlChessTxt::OnAnnot7 ) //"None of the above");
    EVT_MENU( ID_ANNOT10, CtrlChessTxt::OnAnnot10 ) //"="  );   // $10 
    EVT_MENU( ID_ANNOT14, CtrlChessTxt::OnAnnot14 ) //"+=" );   // $14 
    EVT_MENU( ID_ANNOT15, CtrlChessTxt::OnAnnot15 ) //"=+" );   // $15 
    EVT_MENU( ID_ANNOT16, CtrlChessTxt::OnAnnot16 ) //"+/-");   // $16 
    EVT_MENU( ID_ANNOT17, CtrlChessTxt::OnAnnot17 ) //"-/+");   // $17 
    EVT_MENU( ID_ANNOT18, CtrlChessTxt::OnAnnot18 ) //"+-" );   // $18 
    EVT_MENU( ID_ANNOT19, CtrlChessTxt::OnAnnot19 ) //"-+" );   // $19 
    EVT_MENU( ID_ANNOT20, CtrlChessTxt::OnAnnot20 ) //"None of the above");
END_EVENT_TABLE()

