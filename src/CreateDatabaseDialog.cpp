/****************************************************************************
 * Custom dialog - Maintenance Dialog
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/filepicker.h"
#include "DebugPrintf.h"
#include "Portability.h"
#include "Appdefs.h"
#include "Repository.h"
#include "PgnRead.h"
#include "ProgressBar.h"
#include "DbMaintenance.h"
#include "DbPrimitives.h"
#include "PackedGameBinDb.h"
#include "BinDb.h"
#include "CreateDatabaseDialog.h"

// CreateDatabaseDialog type definition
IMPLEMENT_CLASS( CreateDatabaseDialog, wxDialog )

// CreateDatabaseDialog event table definition
BEGIN_EVENT_TABLE( CreateDatabaseDialog, wxDialog )

EVT_BUTTON( wxID_OK, CreateDatabaseDialog::OnOk )

EVT_BUTTON( wxID_HELP, CreateDatabaseDialog::OnHelpClick )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER_DB, CreateDatabaseDialog::OnDbFilePicked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER1,   CreateDatabaseDialog::OnPgnFile1Picked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER2,   CreateDatabaseDialog::OnPgnFile2Picked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER3,   CreateDatabaseDialog::OnPgnFile3Picked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER4,   CreateDatabaseDialog::OnPgnFile4Picked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER5,   CreateDatabaseDialog::OnPgnFile5Picked )
EVT_FILEPICKER_CHANGED( ID_CREATE_DB_PICKER6,   CreateDatabaseDialog::OnPgnFile6Picked )
END_EVENT_TABLE()

CreateDatabaseDialog::CreateDatabaseDialog(
                           wxWindow* parent,
                           wxWindowID id, bool create_mode,
                           const wxPoint& pos, const wxSize& size, long style )
{
    db_created_ok = false;
    this->create_mode = create_mode;
    Create(parent, id, create_mode?"Create Database":"Add Games to Database", pos, size, style);
}

// Dialog create
bool CreateDatabaseDialog::Create( wxWindow* parent,
                          wxWindowID id, const wxString& caption,
                          const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;
    
    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS/*|wxDIALOG_EX_CONTEXTHELP*/ );
    if( !wxDialog::Create( parent, id, caption, pos, size, style ) )
        okay = false;
    else
    {
        
        CreateControls();
        SetDialogHelp();
        SetDialogValidators();
        
        // This fits the dialog to the minimum size dictated by the sizers
        GetSizer()->Fit(this);
        
        // This ensures that the dialog cannot be sized smaller than the minimum size
        GetSizer()->SetSizeHints(this);
        
        // Centre the dialog on the parent or (if none) screen
        Centre();
    }
    return okay;
}

// Control creation for CreateDatabaseDialog
void CreateDatabaseDialog::CreateControls()
{
    
    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
           create_mode?
           "To create a new database from scratch, name the new database and select\n"
           "one or more .pgn files with the games to go into the database.\n"
           :
           "To add games to an existing database, select the database and one or\n"
           "more .pgn files with the additional games to go into the database.\n"
           , wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);
    
    // Spacer
    //box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    // Label for file
    wxStaticText* db_file_label = new wxStaticText ( this, wxID_STATIC,
                                                        create_mode ?
                                                            wxT("&Name a new database file") :
                                                            wxT("&Choose an existing database file"),
                                                        wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(db_file_label, 0, wxALIGN_LEFT|wxALL, 5);
    
    // File picker controls
    wxFilePickerCtrl *picker_db = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER_DB, db_filename,
                                                        create_mode ?
                                                            wxT("Locate and name a database to create") :
                                                            wxT("Select database to add games to"),
                                                        "*.tdb", wxDefaultPosition, wxDefaultSize,
                                                        create_mode ?
                                                            wxFLP_USE_TEXTCTRL|wxFLP_SAVE :
                                                            wxFLP_USE_TEXTCTRL|wxFLP_FILE_MUST_EXIST
                                                       );
    box_sizer->Add(picker_db, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    restricted_box = NULL;
    if( create_mode )
    {
        restricted_box = new wxCheckBox( this, ID_CREATE_RESTRICTED, "Restricted export database", wxDefaultPosition, wxDefaultSize );
        box_sizer->Add(restricted_box, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    }

    // Label for file
    wxStaticText* pgn_file_label = new wxStaticText ( this, wxID_STATIC,
                                                 wxT("&Select one or more .pgn files to add to the database"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(pgn_file_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxFilePickerCtrl *picker1 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER1, pgn_filename1, wxT("Select 1st .pgn"),
                                                    "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                    wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
    box_sizer->Add(picker1, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

    wxFilePickerCtrl *picker2 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER2, pgn_filename2, wxT("Select 2nd .pgn"),
                                                    "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                    wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
    box_sizer->Add(picker2, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

    if( create_mode )
    {
        wxFilePickerCtrl *picker3 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER3, pgn_filename3, wxT("Select next .pgn"),
                                                        "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
        box_sizer->Add(picker3, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

        wxFilePickerCtrl *picker4 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER4, pgn_filename4, wxT("Select next .pgn"),
                                                        "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
        box_sizer->Add(picker4, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

        wxFilePickerCtrl *picker5 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER5, pgn_filename5, wxT("Select next .pgn"),
                                                        "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
        box_sizer->Add(picker5, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

        wxFilePickerCtrl *picker6 = new wxFilePickerCtrl( this, ID_CREATE_DB_PICKER6, pgn_filename6, wxT("Select next .pgn"),
                                                        "*.pgn", wxDefaultPosition, wxDefaultSize,
                                                        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );
        box_sizer->Add(picker6, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    }


	wxBoxSizer* h1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* v1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* v2 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* v3 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* v4 = new wxBoxSizer(wxVERTICAL);

    // Label for elo cutoff
    wxStaticText* elo_cutoff_label = new wxStaticText ( this, wxID_STATIC,
        "Elo cutoff", wxDefaultPosition, wxDefaultSize, 0 );
    v1->Add(elo_cutoff_label, 0, wxALL, 5);

    // A spin control for the elo cutoff
    elo_cutoff_spin = new wxSpinCtrl ( this, ID_CREATE_ELO_CUTOFF,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 4000, 2000 );
    elo_cutoff_spin->SetValue( objs.repository->database.m_elo_cutoff );
    v1->Add(elo_cutoff_spin, 0, wxALL, 5);

    // Radio options
    ignore = new wxRadioButton( this, wxID_ANY,
        "Ignore Elo cutoff", wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    ignore->SetValue( objs.repository->database.m_elo_cutoff_ignore );
    at_least_one = new wxRadioButton( this, wxID_ANY,
        "At least one player", wxDefaultPosition, wxDefaultSize, 0 );
    at_least_one->SetValue( objs.repository->database.m_elo_cutoff_one );
    both = new wxRadioButton( this, wxID_ANY,
        "Both players", wxDefaultPosition, wxDefaultSize,  0 );
    both->SetValue( objs.repository->database.m_elo_cutoff_both );

    // Unrated players
    fail = new wxRadioButton( this, wxID_ANY,
        "Unrated players fail Elo cutoff", wxDefaultPosition, wxDefaultSize,  wxRB_GROUP );
    fail->SetValue( objs.repository->database.m_elo_cutoff_fail );
    pass = new wxRadioButton( this, wxID_ANY,
        "Unrated players pass Elo cutoff", wxDefaultPosition, wxDefaultSize,  0 );
    pass->SetValue( objs.repository->database.m_elo_cutoff_pass );
    pass_before = new wxRadioButton( this, wxID_ANY,
        "Unrated pass if game played before", wxDefaultPosition, wxDefaultSize,  0 );
    pass_before->SetValue( objs.repository->database.m_elo_cutoff_pass_before );
    wxStaticText* spacer1 = new wxStaticText ( this, wxID_ANY,
        " ", wxDefaultPosition, wxDefaultSize, 0 );
    wxStaticText* spacer2 = new wxStaticText ( this, wxID_ANY,
        " ", wxDefaultPosition, wxDefaultSize, 0 );
    before_year = new wxSpinCtrl ( this, wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1400, 2500, 1990 );
    before_year->SetValue( objs.repository->database.m_elo_cutoff_before_year );

	v2->Add(ignore, 0, wxALL, 5);
    v2->Add(at_least_one, 0, wxALL, 5);
    v2->Add(both, 0, wxALL, 5);
	v3->Add(fail, 0, wxALL, 5);
    v3->Add(pass, 0, wxALL, 5);
    v3->Add(pass_before, 0, wxALL, 5);
    v4->Add(spacer1, 0, wxALL, 5);
    v4->Add(spacer2, 0, wxALL, 5);
    v4->Add(before_year, 0, wxALL, 0);
    h1->Add(v1, 0, wxGROW|wxALL, 5);
    h1->Add(v2, 0, wxGROW|wxALL, 5);
    h1->Add(v3, 0, wxGROW|wxALL, 5);
    h1->Add(v4, 0, wxGROW|wxALL, 5);
    box_sizer->Add(h1, 0, wxGROW|wxALL, 5);

	// A dividing line before the OK and Cancel buttons
    wxStaticLine* line2 = new wxStaticLine ( this, wxID_STATIC,
                                           wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line2, 0, wxGROW|wxALL, 5);
    
    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
  
   
    
    // The OK button
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
                                 wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
                                     wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    
    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
                                  wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

// Set the validators for the dialog controls
void CreateDatabaseDialog::SetDialogValidators()
{
}

// Sets the help text for the dialog controls
void CreateDatabaseDialog::SetDialogHelp()
{
    char buf[200];
    sprintf( buf, "If this is selected, exporting games from the new database to PGN will be restricted (no more than %d at a time).", DATABASE_LOCKABLE_LIMIT );
    wxString restricted_help(buf);
    FindWindow(ID_CREATE_RESTRICTED)->SetHelpText(restricted_help);
    FindWindow(ID_CREATE_RESTRICTED)->SetToolTip(restricted_help);
}

// wxID_OK handler
void CreateDatabaseDialog::OnOk( wxCommandEvent& WXUNUSED(event) )
{
    objs.repository->database.m_elo_cutoff        = elo_cutoff_spin->GetValue();
	objs.repository->database.m_elo_cutoff_ignore = ignore->GetValue();
	objs.repository->database.m_elo_cutoff_one    = at_least_one->GetValue();
	objs.repository->database.m_elo_cutoff_both   = both->GetValue();
	objs.repository->database.m_elo_cutoff_fail   = fail->GetValue();
	objs.repository->database.m_elo_cutoff_pass   = pass->GetValue();
	objs.repository->database.m_elo_cutoff_pass_before = pass_before->GetValue();
	objs.repository->database.m_elo_cutoff_before_year = before_year->GetValue();
    if( create_mode )
        OnCreateDatabase();
    else
        OnAppendDatabase();
}

// Create
void CreateDatabaseDialog::OnCreateDatabase()
{
    bool ok=true;
	bool created_new_db_file = false;
    std::string files[6];
    int cnt=0;
    std::string error_msg;
    db_primitive_error_msg();   // clear error reporting mechanism
    
    // Collect files
    for( int i=1; i<=6; i++ )
    {
        bool exists = false;
        std::string filename;
        switch(i)
        {
            case 1: exists = wxFile::Exists(pgn_filename1);
                    filename = std::string(pgn_filename1.c_str());  break;
            case 2: exists = wxFile::Exists(pgn_filename2);
                    filename = std::string(pgn_filename2.c_str());  break;
            case 3: exists = wxFile::Exists(pgn_filename3);
                    filename = std::string(pgn_filename3.c_str());  break;
            case 4: exists = wxFile::Exists(pgn_filename4);
                    filename = std::string(pgn_filename4.c_str());  break;
            case 5: exists = wxFile::Exists(pgn_filename5);
                    filename = std::string(pgn_filename5.c_str());  break;
            case 6: exists = wxFile::Exists(pgn_filename6);
                    filename = std::string(pgn_filename6.c_str());  break;
        }
        if( exists )
        {
            files[cnt++] = filename;
        }
    }
    bool exists = wxFile::Exists(db_filename);
    db_name = std::string( db_filename.c_str() );
    if( exists )
    {
        error_msg = "Tarrasch database file " + db_name + " already exists";
        ok = false;
    }
    else if( db_name.length() == 0 )
    {
        error_msg = "No Tarrasch database file specified";
        ok = false;
    }
    else if( cnt == 0 )
    {
        error_msg = "No usable pgn files specified";
        ok = false;
    }
    // TODO DatabaseClear();
    FILE *ofile=NULL;
    if( ok )
    {
		ok = false;
        BinDbReadBegin();
        wxString fullpath = db_filename;
        wxFileName wfn(db_filename);
        if( wfn.IsOk() && wfn.HasName() )
        {
            wxString cwd  = wxGetCwd();
            wfn.SetExt("tdb");
            wxString path = wxPathOnly(db_filename);
            bool no_path = (path=="");
            if( no_path )
                wfn.SetPath(cwd);
            fullpath = wfn.GetFullPath();
		    db_name = std::string( fullpath.c_str() );
			ofile = fopen( fullpath.c_str(), "wb" );
			if( ofile )
			{
				created_new_db_file = true;
				ok = true;
			}
		}
		if( !ok )
		{
			error_msg = "Cannot create ";
			error_msg += fullpath;
		}
    }
    for( int i=0; ok && i<cnt; i++ )
    {
        FILE *ifile = fopen( files[i].c_str(), "rt" );
        if( !ifile )
        {
            error_msg = "Cannot open ";
            error_msg += files[i];
            ok = false;
        }
        else
        {
            std::string title( "Creating database, step 1 of 4");
            std::string desc("Reading file #");
            char buf[80];
            sprintf( buf, "%d of %d", i+1, cnt );
            desc += buf;
            ProgressBar progress_bar( title, desc, true, this, ifile );
            uint32_t begin = BinDbGetGamesSize();
            PgnRead pgn('B',&progress_bar);
            bool aborted = pgn.Process(ifile);
            uint32_t end = BinDbGetGamesSize();
            BinDbNormaliseOrder( begin, end );
            if( aborted )
            {
                error_msg = "cancel";
                ok = false;
            }
            fclose(ifile);
        }
    }
    if( ok )
    {
        std::string title( "Creating database");    // Step 2,3 and 4 of 4
        int step=2;
        bool locked = (restricted_box ? restricted_box->GetValue() : false);
        ok = BinDbRemoveDuplicatesAndWrite(title,step,ofile,locked,this);
    }
    if( ofile )
    {
        wxSafeYield();
        fclose(ofile);
        ofile = NULL;
    }
    if( ok )
    {
        wxSafeYield();
        AcceptAndClose();
        db_created_ok = true;
    }
    else
    {
        if( error_msg == "" )
            error_msg = db_primitive_error_msg();
        if( error_msg == "cancel" )
            error_msg = "Database creation cancelled";
        wxMessageBox( error_msg.c_str(), "Database creation failed", wxOK|wxICON_ERROR );
		if( created_new_db_file )
#ifdef THC_UNIX
			unlink(db_filename.c_str());
#else
			_unlink(db_filename.c_str());
#endif
    }
    BinDbCreationEnd();
}

void CreateDatabaseDialog::OnAppendDatabase()
{
    bool ok=true;
    bool locked=false;
	bool created_new_db_file = false;
    std::string files[6];
    int cnt=0;
    std::string error_msg;
    db_primitive_error_msg();   // clear error reporting mechanism

    // Collect files
    for( int i=1; i<=6; i++ )
    {
        bool exists = false;
        std::string filename;
        switch(i)
        {
            case 1: exists = wxFile::Exists(pgn_filename1);
                    filename = std::string(pgn_filename1.c_str());  break;
            case 2: exists = wxFile::Exists(pgn_filename2);
                    filename = std::string(pgn_filename2.c_str());  break;
            case 3: exists = wxFile::Exists(pgn_filename3);
                    filename = std::string(pgn_filename3.c_str());  break;
            case 4: exists = wxFile::Exists(pgn_filename4);
                    filename = std::string(pgn_filename4.c_str());  break;
            case 5: exists = wxFile::Exists(pgn_filename5);
                    filename = std::string(pgn_filename5.c_str());  break;
            case 6: exists = wxFile::Exists(pgn_filename6);
                    filename = std::string(pgn_filename6.c_str());  break;
        }
        if( exists )
        {
            files[cnt++] = filename;
        }
    }
    if( cnt == 0 )
    {
        error_msg = "No usable pgn files specified";
        ok = false;
    }
    if( ok )
    {
        bool exists = wxFile::Exists(db_filename);
        db_name = std::string( db_filename.c_str() );
        if( !exists )
        {
            error_msg = "Tarrasch database file " + db_name + " doesn't exist";
            ok = false;
        }
    }
    if( ok )
    {
        ok = BinDbOpen( db_filename.c_str(), error_msg );
    }
    if( ok )
    {
        bool dummyb=false;
        int dummyi=false;
        std::string title( "Appending to database, step 1 of 5");
        std::string desc("Reading existing database");
        ProgressBar progress_bar( title, desc, true, this );
        std::vector< smart_ptr<ListableGame> > &mega_cache = BinDbLoadAllGamesGetVector();
		cprintf( "Appending to database, step 1 of 5 begin\n" );
        bool killed = BinDbLoadAllGames( locked, true, mega_cache, dummyi, dummyb, &progress_bar );
		cprintf( "Appending to database, step 1 of 5 end, killed=%s\n", killed?"true":"false" );
		if( killed )
			ok=false;
        BinDbClose();
    }
    FILE *ofile=NULL;
    if( ok )
    {
        ofile = fopen( db_name.c_str(), "wb" );
        if( ofile )
			created_new_db_file = true;
		else
        {
            error_msg = "Cannot open ";
            error_msg += db_name;
            ok = false;
        }
    }
    for( int i=0; ok && i<cnt; i++ )
    {
        FILE *ifile = fopen( files[i].c_str(), "rt" );
        if( !ifile )
        {
            error_msg = "Cannot open ";
            error_msg += files[i];
            ok = false;
        }
        else
        {
            std::string title2( "Appending to database, step 2 of 5");
            std::string desc2("Reading file #");
            char buf[80];
            sprintf( buf, "%d of %d", i+1, cnt );
            desc2 += buf;
            ProgressBar progress_bar2( title2, desc2, true, this, ifile );
            uint32_t begin = BinDbGetGamesSize();
            PgnRead pgn('B',&progress_bar2);
            bool aborted = pgn.Process(ifile);
            uint32_t end = BinDbGetGamesSize();
            BinDbNormaliseOrder( begin, end );
            if( aborted )
            {
                error_msg = "cancel";
                ok = false;
            }
            fclose(ifile);
        }
    }
    if( ok )
    {
        std::string title3( "Appending to database");    // Step 3,4 and 5 of 5
        int step=3;
        ok = BinDbRemoveDuplicatesAndWrite(title3,step,ofile,locked,this);
    }
	if( ofile )
	{
        fclose(ofile);
		ofile = NULL;
	}
    if( ok )
    {
        wxSafeYield();
        AcceptAndClose();
        db_created_ok = true;
    }
    else
    {
        if( error_msg == "" )
            error_msg = db_primitive_error_msg();
        if( error_msg == "cancel" )
            error_msg = "Database creation cancelled";
        wxMessageBox( error_msg.c_str(), "Database creation failed", wxOK|wxICON_ERROR );
		if( created_new_db_file )
#ifdef THC_UNIX
			unlink(db_filename.c_str());
#else
			_unlink(db_filename.c_str());
#endif
    }
    BinDbCreationEnd();
}

void CreateDatabaseDialog::OnDbFilePicked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    db_filename = file;
}

void CreateDatabaseDialog::OnPgnFile1Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename1 = file;
}

void CreateDatabaseDialog::OnPgnFile2Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename2 = file;
}

void CreateDatabaseDialog::OnPgnFile3Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename3 = file;
}

void CreateDatabaseDialog::OnPgnFile4Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename4 = file;
}

void CreateDatabaseDialog::OnPgnFile5Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename5 = file;
}

void CreateDatabaseDialog::OnPgnFile6Picked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    pgn_filename6 = file;
}

void CreateDatabaseDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
     wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */
    
    wxString helpText =
    wxT("\nTarrasch database files (.tdb files) are compact game collections. ")
    wxT("At the moment only complete games are supported (i.e. no game fragments). ")
    wxT("Similarly no comments or variations are supported at this time. ")
    wxT("Tarrasch uses one database file at a time (the current database). ")
    wxT("Tarrasch can search the current database quickly and efficiently ")
    wxT("for any position or for piece patterns or material balances. ")
    wxT("Tarrasch databases are created from .pgn files.")
    wxT("\n\n")
    wxT("The only way Tarrasch databases can be modified (at this time) is by ")
    wxT("appending more games to them with the Append to database command. ")
    wxT("Tarrasch automatically rejects duplicate games. ")
    wxT("Rejected duplicate games are saved in file TarraschDbDuplicatesFile.pgn. ")
    wxT("Games can be rejected based on ")
    wxT("rating if desired. ")
    wxT("For best results, create databases with older .pgn files first, and ")
    wxT("append newer games as you collect them. ")
    wxT("Tarrasch will try to present most recent games first." );
    wxMessageBox(helpText,
                 wxT("Create database / Append to database help"),
                 wxOK|wxICON_INFORMATION, this);
}
