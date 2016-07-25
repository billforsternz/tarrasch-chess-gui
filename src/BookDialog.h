/****************************************************************************
 * Custom dialog - Opening book
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef BOOK_DIALOG_H
#define BOOK_DIALOG_H
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "SuspendEngine.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_BOOK_DIALOG        = 10000,
    ID_BOOK_RESET         = 10001,
    ID_ENABLED            = 10002,
    ID_SUGGEST            = 10003,
    ID_BOOK_PICKER        = 10004,
    ID_LIMIT_MOVES        = 10005,
    ID_POST_LIMIT_PERCENT = 10006
};

// BookDialog class declaration
class BookDialog: public wxDialog
{    
    DECLARE_CLASS( BookDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    BookDialog( );
    BookDialog(
      const BookConfig &dat,
      wxWindow* parent,
      wxWindowID id = ID_BOOK_DIALOG,
      const wxString& caption = wxT("Opening book settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_BOOK_DIALOG,
      const wxString& caption = wxT("Opening book settings"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // BookDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // ID_BOOK_PICKER
    void OnFilePicked( wxFileDirPickerEvent& event );

    // BookDialog member variables

    // Data members
    BookConfig   dat;
    SuspendEngine   suspendor;  // the mere presence of this var suspends the engine during the dialog
};

#endif    // BOOK_DIALOG_H
