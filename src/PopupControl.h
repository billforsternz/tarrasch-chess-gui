/****************************************************************************
 * Control - My own popup menu (wxWidget popup menu is Windows specific)
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef POPUPCONTROL_H
#define POPUPCONTROL_H
#include "wx/wx.h"
enum POPUP_MODE
{
    CLICK_ON_DESTINATION,
    SLIDE_TO_DESTINATION,
    BOOK_HOVER
};

class PopupControl : public wxControl
{
public:
    PopupControl( wxWindow* parent,
                  wxArrayString &strs,
                  wxArrayString &terses,
                  wxArrayString &book,
                  POPUP_MODE popup_mode,
                  wxRect     &hover,    // if popup_mode == BOOK_HOVER
                  wxWindowID id = wxID_ANY,
                  const wxPoint& point = wxDefaultPosition );
    bool done;
    char terse_move[6];

private:
    bool upside_down;
    wxArrayString strs;
    wxArrayString terses;
    wxArrayString book;
    int sel;
    int count;
    int height;
    POPUP_MODE popup_mode;
    wxPoint point;
    wxRect  hover;
    bool release_button_to_exit;
    void Shutdown( bool inject_move );
    void OnMouseMove( wxMouseEvent& event );
    void OnMouseLeftUp( wxMouseEvent &event );
    void OnMouseLeftDown( wxMouseEvent &event );
    void OnPaint(wxPaintEvent& WXUNUSED(evt));
    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt));
    void OnSize(wxSizeEvent& WXUNUSED(evt));
    void OnMouseCaptureLost( wxMouseCaptureLostEvent &event );
private:
    DECLARE_EVENT_TABLE()
};

#endif // POPUPCONTROL_H
