/****************************************************************************
 * Custom dialog - Search for pattern in database
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef PATTERN_DIALOG_H
#define PATTERN_DIALOG_H
#include "wx/spinctrl.h"
#include "thc.h"
#include "PatternMatch.h"
#include "SuspendEngine.h"
class BoardSetupControl;

// Control identifiers
enum
{
    ID_PATTERN_DIALOG                   = 10000,
    ID_PATTERN_RESET                    = 10001,
    ID_PATTERN_CLEAR                    = 10002,
    ID_PATTERN_CURRENT                  = 10003,
    ID_PATTERN_MOVE_COUNT               = 10004,
    ID_PATTERN_WHITE_TO_MOVE            = 10005,
    ID_PATTERN_BLACK_TO_MOVE            = 10006,
    ID_PATTERN_EITHER_TO_MOVE           = 10007,
    ID_PATTERN_INC_REFLECTION           = 10008,
    ID_PATTERN_INC_REVERSE              = 10009,
    ID_PATTERN_ALLOW_MORE               = 10010,
    ID_PATTERN_PAWNS_SAME_FILES         = 10011,
    ID_PATTERN_BISHOPS_SAME_COLOUR      = 10012
};

// PatternDialog class declaration
class PatternDialog: public wxDialog
{    
    DECLARE_CLASS( PatternDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructor
    PatternDialog( PatternParameters *parm, wxWindow* parent, const wxString& caption,
      wxWindowID id = ID_PATTERN_DIALOG,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creation
    bool Create( wxWindow* parent, const wxString& caption,
      wxWindowID id = ID_PATTERN_DIALOG,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // PatternDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CURRENT
    void OnCurrentClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PATTERN_CLEAR
    void OnClearClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    // Data members
    PatternParameters      *parm;
    thc::ChessPosition     m_pos;
private:
    wxString        fen;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
    wxCheckBox*     inc_reverse;
    wxCheckBox*     inc_reflection;
    wxCheckBox*     pawns_same_files;
    wxCheckBox*     bishops_same_colour;
    wxCheckBox*     allow_more;
    wxRadioButton*  white_to_move;
    wxRadioButton*  black_to_move;
    wxRadioButton*  either_to_move;
    wxSpinCtrl*     move_count_ctrl;
    wxButton*       ok_button;
    BoardSetupControl *bsc;

    // pattern
    bool b_either;
    bool b_white;
    bool b_black;

    // material balance
    bool b_pawns;
    bool b_bishops;
    bool b_ply;
};

#endif    // PATTERN_DIALOG_H
