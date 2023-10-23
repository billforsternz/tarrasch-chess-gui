/****************************************************************************
 * Control - Chess move entry
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef CTRL_CHESS_TXT_H
#define CTRL_CHESS_TXT_H
#include <vector>
#include "wx/wx.h"
#include "wx/richtext/richtextctrl.h"
#include "GameDocument.h"
#include "GameView.h"
#include "NavigationKey.h"

class GameLogic;
class CtrlChessTxt : public wxRichTextCtrl
{
public:
    GameLogic *gl;
    CtrlChessTxt( wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& point = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize );
    ~CtrlChessTxt() { if( context_menu ) {delete context_menu; context_menu=0; } }

public:
    void SetGameDocument( GameDocument *gd );
    void Goto( unsigned long pos, bool from_mouse_move=false );
    void NavigationKey( NAVIGATION_KEY nk );
    void Copy();
    void Paste();
    void Cut();
    void Delete();
    void SetFontSize();

private:
    void OnProcessCustom(wxCommandEvent& event);
    void OnMouseMove( wxMouseEvent& event );
    void OnMouseLeftUp( wxMouseEvent &event );
    void OnMouseLeftDown( wxMouseEvent &event );
    void OnContext( wxContextMenuEvent &event );
    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
    void OnSize(wxSizeEvent& WXUNUSED(evt));
    void OnUndo(wxCommandEvent& WXUNUSED(event));
    void OnUpdateUndo( wxUpdateUIEvent &event );
    void OnRedo(wxCommandEvent& WXUNUSED(event));
    void OnUpdateRedo( wxUpdateUIEvent &event );

    // 13 event handlers
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnText(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTextURL(wxTextUrlEvent& event);
    void OnTextMaxLen(wxCommandEvent& event);
    void OnTextCut(wxClipboardTextEvent & event);
    void OnTextCopy(wxClipboardTextEvent & event);
    void OnTextPaste(wxClipboardTextEvent & event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    void OnAnnotNag1( int nag1 );
    void OnAnnotNag2( int nag2 );
    void OnAnnot1(wxCommandEvent& event);
    void OnAnnot2(wxCommandEvent& event);
    void OnAnnot3(wxCommandEvent& event);
    void OnAnnot4(wxCommandEvent& event);
    void OnAnnot5(wxCommandEvent& event);
    void OnAnnot6(wxCommandEvent& event);
    void OnAnnot7(wxCommandEvent& event);
    void OnAnnot10(wxCommandEvent& event);
    void OnAnnot14(wxCommandEvent& event);
    void OnAnnot15(wxCommandEvent& event);
    void OnAnnot16(wxCommandEvent& event);
    void OnAnnot17(wxCommandEvent& event);
    void OnAnnot18(wxCommandEvent& event);
    void OnAnnot19(wxCommandEvent& event);
    void OnAnnot20(wxCommandEvent& event);

    int max_w, max_h, height;
private:
    wxFont font;
    wxMenu *context_menu;
    GameTree *popup_gt;
    GameDocument *gd;
    GameView *gv;
    DECLARE_EVENT_TABLE()
};

#endif // CTRL_CHESS_TXT_H
