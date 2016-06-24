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
    //ID_POSITION_DIALOG = 10000,
    ID_PATTERN_DIALOG  = 10001,
    ID_MATERIAL_BALANCE_DIALOG,
    ID_PATTERN_RESET,
    ID_PATTERN_CLEAR,
    ID_PATTERN_CURRENT,
    ID_PATTERN_MOVE_COUNT,
    ID_PATTERN_WHITE_TO_MOVE,
    ID_PATTERN_BLACK_TO_MOVE,
    ID_PATTERN_EITHER_TO_MOVE,
    ID_PATTERN_INC_REFLECTION,
    ID_PATTERN_INC_REVERSE,
    ID_PATTERN_ALLOW_MORE,
    ID_PATTERN_PAWNS_SAME_FILES,
    ID_PATTERN_BISHOPS_SAME_COLOUR,
    ID_PATTERN_LOCKDOWN_WK,
    ID_PATTERN_LOCKDOWN_WQ,
    ID_PATTERN_LOCKDOWN_WR,
    ID_PATTERN_LOCKDOWN_WB,
    ID_PATTERN_LOCKDOWN_WN,
    ID_PATTERN_LOCKDOWN_WP,
    ID_PATTERN_LOCKDOWN_BK,
    ID_PATTERN_LOCKDOWN_BQ,
    ID_PATTERN_LOCKDOWN_BR,
    ID_PATTERN_LOCKDOWN_BB,
    ID_PATTERN_LOCKDOWN_BN,
    ID_PATTERN_LOCKDOWN_BP,
    ID_PATTERN_MORE_PIECES_WQ,
    ID_PATTERN_MORE_PIECES_WR,
    ID_PATTERN_MORE_PIECES_WB,
    ID_PATTERN_MORE_PIECES_WN,
    ID_PATTERN_MORE_PIECES_WP,
    ID_PATTERN_MORE_PIECES_BQ,
    ID_PATTERN_MORE_PIECES_BR,
    ID_PATTERN_MORE_PIECES_BB,
    ID_PATTERN_MORE_PIECES_BN,
    ID_PATTERN_MORE_PIECES_BP
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

    void ModifyLockdown( int offset );

    // Data members
    PatternParameters      *parm;
    thc::ChessPosition     m_pos;
private:
    wxString        fen;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
    wxStaticText*   lockdown_text;
    wxCheckBox*     inc_reverse;
    wxCheckBox*     inc_reflection;
    wxCheckBox*     pawns_same_files;
    wxCheckBox*     bishops_same_colour;
    wxCheckBox*     allow_more;
    wxCheckBox*     lockdown_wk;
    wxCheckBox*     lockdown_wq;
    wxCheckBox*     lockdown_wr;
    wxCheckBox*     lockdown_wb;
    wxCheckBox*     lockdown_wn;
    wxCheckBox*     lockdown_wp;
    wxCheckBox*     lockdown_bk;
    wxCheckBox*     lockdown_bq;
    wxCheckBox*     lockdown_br;
    wxCheckBox*     lockdown_bb;
    wxCheckBox*     lockdown_bn;
    wxCheckBox*     lockdown_bp;
    wxCheckBox*     more_pieces_wq;
    wxCheckBox*     more_pieces_wr;
    wxCheckBox*     more_pieces_wb;
    wxCheckBox*     more_pieces_wn;
    wxCheckBox*     more_pieces_wp;
    wxCheckBox*     more_pieces_bq;
    wxCheckBox*     more_pieces_br;
    wxCheckBox*     more_pieces_bb;
    wxCheckBox*     more_pieces_bn;
    wxCheckBox*     more_pieces_bp;
    wxRadioButton*  white_to_move;
    wxRadioButton*  black_to_move;
    wxRadioButton*  either_to_move;
    wxSpinCtrl*     move_count_ctrl;
    wxButton*       ok_button;
    BoardSetupControl *bsc;
    bool            support_lockdown;
    int             offset_persist;

    // pattern
    bool b_either;
    bool b_white;
    bool b_black;
    bool b_allow_more;

    // material balance
    bool b_piece_map;
    bool b_pawns;
    bool b_bishops;
    bool b_ply;
};

#endif    // PATTERN_DIALOG_H
