/****************************************************************************
 * Custom dialog - General options
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef GENERAL_DIALOG_H
#define GENERAL_DIALOG_H
#include "wx/spinctrl.h"
#include "wx/clrpicker.h"
#include "thc.h"
#include "SuspendEngine.h"
#include "DialogDetect.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_GENERAL_DIALOG    = 10000,
    ID_NOTATION_LANGUAGE = 10001,
    ID_NO_ITALICS        = 10002,
    ID_STRAIGHT_TO_GAME  = 10003,
    ID_LARGE_FONT        = 10004, 
    ID_RESTORE_LIGHT     = 10005,
    ID_RESTORE_DARK      = 10006,
    ID_RESTORE_HIGHLIGHT_LIGHT = 10007,
    ID_RESTORE_HIGHLIGHT_DARK  = 10008,
    ID_RESTORE_HIGHLIGHT_LINE  = 10009,
    ID_NO_AUTO_FLIP			   = 10010,
    ID_STRAIGHT_TO_FIRST_GAME  = 10011,
    ID_EMIT_BELL               = 10012,
	ID_SUPPRESS_HIGHLIGHT	   = 10013,
	ID_HEADING_ABOVE_BOARD     = 10014,
	ID_LINE_HIGHLIGHT	       = 10015
};

// GeneralDialog class declaration
class GeneralDialog: public wxDialog
{    
    DECLARE_CLASS( GeneralDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    GeneralDialog( wxWindow* parent, GeneralConfig *dat_ptr,
      wxWindowID id = ID_GENERAL_DIALOG,
      const wxString& caption = wxT("General settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init( GeneralConfig *dat_ptr );

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_GENERAL_DIALOG,
      const wxString& caption = wxT("General settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();


    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    void OnRestoreLight( wxCommandEvent& event );
    void OnRestoreDark( wxCommandEvent& event );
    void OnRestoreHighlightLight( wxCommandEvent& event );
    void OnRestoreHighlightDark( wxCommandEvent& event );
    void OnRestoreHighlightLine( wxCommandEvent& event );
    void OnNotationLanguage( wxCommandEvent& event );
	void OnColourPicker( wxColourPickerEvent& event );

    wxArrayString   labels;

    // Data members
public:
    wxComboBox*     notation_language_ctrl;
    wxButton*       ok_button;
    wxString        combo_label;
    GeneralConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
	DialogDetect    detect;		// similarly the presence of this var allows tracking of open dialogs

private:
	wxColourPickerCtrl* light_picker;
	wxColourPickerCtrl* dark_picker;
	wxColourPickerCtrl* highlight_light_picker;
	wxColourPickerCtrl* highlight_dark_picker;
	wxColourPickerCtrl* highlight_line_picker;
};

#endif    // GENERAL_DIALOG_H
