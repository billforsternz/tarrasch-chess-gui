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
#include "SuspendEngine.h"
class BoardSetupControl;

// Control identifiers
enum
{
    ID_PATTERN_DIALOG    = 10000,
    ID_PATTERN_RESET     = 10001,
    ID_PATTERN_CLEAR     = 10002,
    ID_PATTERN_CURRENT   = 10003
};

// PatternDialog class declaration
class PatternDialog: public wxDialog
{    
    DECLARE_CLASS( PatternDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    PatternDialog( );
    PatternDialog( wxWindow* parent,
      wxWindowID id = ID_PATTERN_DIALOG,
      const wxString& caption = wxT("Pattern setup"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();
    void Pos2Fen( const thc::ChessPosition& pos, wxString& fen );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_PATTERN_DIALOG,
      const wxString& caption = wxT("Pattern setup"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

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

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_960
    void OnChess960Click( wxCommandEvent& event );

   // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_APPLY
    void OnApplyClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    void OnSpin( wxSpinEvent& event );
    void OnRadio( wxCommandEvent& event );
    void OnCheckBox( wxCommandEvent& event );
    void OnComboBox( wxCommandEvent& event );
    void OnPredefined( wxCommandEvent& event );
    void ModifyFen();
    void IllegalPatternDialog( const char *caption, thc::ChessRules &cr, thc::ILLEGAL_REASON reason );

    // PatternDialog member variables

    // Data members
    thc::ChessPosition  m_pos;
private:
    wxString     fen;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
    wxCheckBox*     white_ooo;
    wxCheckBox*     white_oo;
    wxCheckBox*     black_ooo;
    wxCheckBox*     black_oo;
    wxRadioButton*  white_to_move;
    wxRadioButton*  black_to_move;
    wxSpinCtrl*     move_count_ctrl;
    wxSpinCtrl*     half_count_ctrl;
    wxComboBox*     en_passant_ctrl;
    wxComboBox*     predefined_patterns_ctrl;
    wxButton*       ok_button;
    wxTextCtrl*     fen_ctrl;
    BoardSetupControl *bsc;
    wxString        combo_label;
    wxArrayString   labels;
    wxArrayString   fens;
    void WriteToControls();
};

#endif    // PATTERN_DIALOG_H
