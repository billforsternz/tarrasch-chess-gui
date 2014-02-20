/****************************************************************************
 * Custom dialog - Setup position on board
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef POSITION_DIALOG_H
#define POSITION_DIALOG_H
#include "wx/spinctrl.h"
#include "thc.h"
#include "SuspendEngine.h"
class BoardSetupControl;

// Control identifiers
enum
{
    ID_POSITION_DIALOG    = 10000,
    ID_POSITION_RESET     = 10001,
    ID_POSITION_CLEAR     = 10002,
    ID_POSITION_CURRENT   = 10003,
    ID_POSITION_960       = 10004,
    ID_APPLY              = 10005,
    ID_FEN                = 10006,
    ID_WHITE_OO           = 10007,
    ID_WHITE_OOO          = 10008,
    ID_BLACK_OO           = 10009,
    ID_BLACK_OOO          = 10010,
    ID_WHITE_TO_MOVE      = 10011,
    ID_BLACK_TO_MOVE      = 10012,
    ID_MOVE_COUNT         = 10013,
    ID_HALF_COUNT         = 10014,
    ID_EN_PASSANT         = 10015,
    ID_PREDEFINED         = 10016
};

// PositionDialog class declaration
class PositionDialog: public wxDialog
{    
    DECLARE_CLASS( PositionDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    PositionDialog( );
    PositionDialog( wxWindow* parent,
      wxWindowID id = ID_POSITION_DIALOG,
      const wxString& caption = wxT("Position setup"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();
    void Pos2Fen( const thc::ChessPosition& pos, wxString& fen );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_POSITION_DIALOG,
      const wxString& caption = wxT("Position setup"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // PositionDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_CURRENT
    void OnCurrentClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_POSITION_CLEAR
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
    void IllegalPositionDialog( const char *caption, thc::ChessRules &cr, thc::ILLEGAL_REASON reason );

    // PositionDialog member variables

    // Data members
    wxString     fen;
private:
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
    wxComboBox*     predefined_positions_ctrl;
    wxButton*       ok_button;
    wxTextCtrl*     fen_ctrl;
    BoardSetupControl *bsc;
    thc::ChessPosition  m_pos;
    wxString        combo_label;
    wxArrayString   labels;
    wxArrayString   fens;
    void WriteToControls();
};

#endif    // POSITION_DIALOG_H
