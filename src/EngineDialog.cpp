/****************************************************************************
 * Custom dialog - Select UCI engine
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/filepicker.h"
#include "Portability.h"
#include "Appdefs.h"
#include "EngineDialog.h"

// EngineDialog type definition
IMPLEMENT_CLASS( EngineDialog, wxDialog )

// EngineDialog event table definition
BEGIN_EVENT_TABLE( EngineDialog, wxDialog )
 // EVT_UPDATE_UI( ID_VOTE, EngineDialog::OnVoteUpdate )
 // EVT_BUTTON( ID_ENGINE_BROWSE, EngineDialog::OnBrowseClick )
    EVT_BUTTON( ID_ENGINE_RESET, EngineDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, EngineDialog::OnHelpClick )
    EVT_FILEPICKER_CHANGED( ID_ENGINE_PICKER, EngineDialog::OnFilePicked )
END_EVENT_TABLE()

// EngineDialog constructors
EngineDialog::EngineDialog()
{
    Init();
}

EngineDialog::EngineDialog(
  const EngineConfig &dat,
  wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    this->dat = dat;
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void EngineDialog::Init()
{
    dat.m_file = "";
}

// Dialog create
bool EngineDialog::Create( wxWindow* parent,
    wxWindowID id, const wxString& caption,
    const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;

    // Get nbr of CPUs
#ifdef WINDOWS_FIX_LATER
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    nbr_cpus = sysinfo.dwNumberOfProcessors;
#else
    nbr_cpus = 1;
#endif

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

// Control creation for EngineDialog
void EngineDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        "This panel lets you select the UCI engine to run (an .exe file).\n"
        "It is important that the .exe you select is actually a UCI engine!\n"
        "You can also select parameters to modify engine behaviour. Expert\n"
        "users might enjoy experimenting with the custom parameters."
        , wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    // box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for file
    wxStaticText* file_label = new wxStaticText ( this, wxID_STATIC,
        wxT("UCI Engine executable file:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(file_label, 0, wxALIGN_LEFT|wxALL, 5);

    // File picker control
    wxString path = dat.m_file;
    wxFilePickerCtrl *picker = new wxFilePickerCtrl( this, ID_ENGINE_PICKER, path, "", //wxT("&Select UCI Engine"),
#ifdef THC_WINDOWS
        "*.exe", wxDefaultPosition, wxDefaultSize,
#else         
        "*", wxDefaultPosition, wxDefaultSize,
#endif        
        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST ); //|wxFLP_CHANGE_DIR );    
    box_sizer->Add(picker, 1, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxBOTTOM|wxRIGHT, 5);

    // Ponder enabled
    wxCheckBox* ponder_box = new wxCheckBox( this, ID_PONDER, 
       wxT("Ponder"), wxDefaultPosition, wxDefaultSize, 5 );
    ponder_box->SetValue( dat.m_ponder );
    //box_sizer->Add( ponder_box, 0,
    //    wxALL, 5);
/*
    // Label for the hash
    wxStaticText* hash_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Hash:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(hash_label, 0, wxALL, 5);

    // A spin control for the hash
    wxSpinCtrl* hash_spin = new wxSpinCtrl ( this, ID_HASH,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, 4096, 64 );
    box_sizer->Add(hash_spin, 0, wxALL, 5);
*/

    // Label for the hash
    wxStaticText* hash_label = new wxStaticText ( this, wxID_STATIC,
        wxT("       Hash size (MB):"), wxDefaultPosition, wxDefaultSize, 5 );

    // A spin control for the hash
    wxSpinCtrl* hash_spin = new wxSpinCtrl ( this, ID_HASH,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, 4096, 64 );
    wxBoxSizer* hash_horiz  = new wxBoxSizer(wxHORIZONTAL);
    hash_horiz->Add( ponder_box, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    hash_horiz->Add( hash_label,  0, wxALIGN_LEFT|wxGROW|wxALL, 10);
    hash_horiz->Add( hash_spin,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    box_sizer->Add( hash_horiz, 0, wxTOP|wxBOTTOM|wxRIGHT, 5);

/*
    // Label for max cpu cores
    wxStaticText* max_cpu_cores_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Max CPU cores:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(max_cpu_cores_label, 0, wxALL, 5);

    // A spin control for max cpu cores
    wxSpinCtrl* max_cpu_cores_spin = new wxSpinCtrl ( this, ID_MAX_CPU_CORES,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, nbr_cpus, 1 );
    box_sizer->Add(max_cpu_cores_spin, 0, wxALL, 5);
*/

    // Label for max cpu cores
    wxStaticText* max_cpu_cores_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Max CPU cores:"), wxDefaultPosition, wxDefaultSize, 0 );

    // A spin control for max cpu cores
    wxSpinCtrl* max_cpu_cores_spin = new wxSpinCtrl ( this, ID_MAX_CPU_CORES,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, nbr_cpus, 1 );
    wxBoxSizer* max_cpu_cores_horiz  = new wxBoxSizer(wxHORIZONTAL);
    max_cpu_cores_horiz->Add( max_cpu_cores_label,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    max_cpu_cores_horiz->Add( max_cpu_cores_spin,  0, wxALIGN_LEFT|wxGROW|wxALL, 5);
    box_sizer->Add( max_cpu_cores_horiz, 0, wxTOP|wxBOTTOM|wxRIGHT, 5);

    // Text controls for custom parameter 1
    wxTextCtrl *custom1a_ctrl = new wxTextCtrl ( this, ID_CUSTOM1A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom1b_ctrl = new wxTextCtrl ( this, ID_CUSTOM1B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 1
    wxStaticText* custom1_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 1 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom1_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom1_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom1_horiz->Add( custom1a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom1_horiz->Add( custom1b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom1_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // Text controls for custom parameter 2
    wxTextCtrl *custom2a_ctrl = new wxTextCtrl ( this, ID_CUSTOM2A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom2b_ctrl = new wxTextCtrl ( this, ID_CUSTOM2B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 2
    wxStaticText* custom2_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 2 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom2_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom2_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom2_horiz->Add( custom2a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom2_horiz->Add( custom2b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom2_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // Text controls for custom parameter 3
    wxTextCtrl *custom3a_ctrl = new wxTextCtrl ( this, ID_CUSTOM3A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom3b_ctrl = new wxTextCtrl ( this, ID_CUSTOM3B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 3
    wxStaticText* custom3_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 3 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom3_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom3_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom3_horiz->Add( custom3a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom3_horiz->Add( custom3b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom3_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // Text controls for custom parameter 4
    wxTextCtrl *custom4a_ctrl = new wxTextCtrl ( this, ID_CUSTOM4A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom4b_ctrl = new wxTextCtrl ( this, ID_CUSTOM4B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 4
    wxStaticText* custom4_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 4 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom4_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom4_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom4_horiz->Add( custom4a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom4_horiz->Add( custom4b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom4_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // Text controls for custom parameter 5
    wxTextCtrl *custom5a_ctrl = new wxTextCtrl ( this, ID_CUSTOM5A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom5b_ctrl = new wxTextCtrl ( this, ID_CUSTOM5B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 5
    wxStaticText* custom5_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 5 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom5_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom5_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom5_horiz->Add( custom5a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom5_horiz->Add( custom5b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom5_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // Text controls for custom parameter 6
    wxTextCtrl *custom6a_ctrl = new wxTextCtrl ( this, ID_CUSTOM6A, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    wxTextCtrl *custom6b_ctrl = new wxTextCtrl ( this, ID_CUSTOM6B, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );

    // Label for custom parameter 6
    wxStaticText* custom6_label = new wxStaticText ( this, wxID_STATIC,
        wxT("Custom parameter 6 (name, value):"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(custom6_label, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* custom6_horiz  = new wxBoxSizer(wxHORIZONTAL);
    custom6_horiz->Add( custom6a_ctrl,  2, wxALIGN_LEFT|wxGROW|wxLEFT|wxBOTTOM|wxRIGHT, 5);
    custom6_horiz->Add( custom6b_ctrl,  1, wxLEFT|wxBOTTOM|wxRIGHT, 5);
    box_sizer->Add( custom6_horiz, 0, wxALIGN_LEFT|wxLEFT|wxBOTTOM, 0 );

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_ENGINE_RESET, wxT("&Reset"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
 */
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
void EngineDialog::SetDialogValidators()
{
    FindWindow(ID_PONDER)->SetValidator(
        wxGenericValidator(& dat.m_ponder));
    FindWindow(ID_HASH)->SetValidator(
        wxGenericValidator(& dat.m_hash));
    FindWindow(ID_MAX_CPU_CORES)->SetValidator(
        wxGenericValidator(& dat.m_max_cpu_cores));
    FindWindow(ID_CUSTOM1A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom1a));
    FindWindow(ID_CUSTOM1B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom1b));
    FindWindow(ID_CUSTOM2A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom2a));
    FindWindow(ID_CUSTOM2B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom2b));
    FindWindow(ID_CUSTOM3A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom3a));
    FindWindow(ID_CUSTOM3B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom3b));
    FindWindow(ID_CUSTOM4A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom4a));
    FindWindow(ID_CUSTOM4B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom4b));
    FindWindow(ID_CUSTOM5A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom5a));
    FindWindow(ID_CUSTOM5B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom5b));
    FindWindow(ID_CUSTOM6A)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom6a));
    FindWindow(ID_CUSTOM6B)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &dat.m_custom6b));
}

// Sets the help text for the dialog controls
void EngineDialog::SetDialogHelp()
{
    wxString file_help = wxT("The UCI engine to use (a .exe file, eg Rybka v2.3.2a.mp.w32.exe, or komodo3-64.exe).");
    FindWindow(ID_ENGINE_PICKER)->SetHelpText(file_help);
    FindWindow(ID_ENGINE_PICKER)->SetToolTip(file_help);
    wxString ponder_help = wxT("Allow the engine to think on human's time (if capable).");
    FindWindow(ID_PONDER)->SetHelpText(ponder_help);
    FindWindow(ID_PONDER)->SetToolTip(ponder_help);
    wxString hash_help = wxT("The maximum size of hash table the engine is allowed (in megabytes).");
    FindWindow(ID_HASH)->SetHelpText(hash_help);
    FindWindow(ID_HASH)->SetToolTip(hash_help);
    wxString max_cpu_cores_help = wxT("The number of CPU cores the engine can use.");
    FindWindow(ID_MAX_CPU_CORES)->SetHelpText(max_cpu_cores_help);
    FindWindow(ID_MAX_CPU_CORES)->SetToolTip(max_cpu_cores_help);
    wxString custom_1a_help = wxT("Optional extra parameter, specify name here, eg UCI_Elo");
    FindWindow(ID_CUSTOM1A)->SetHelpText(custom_1a_help);
    FindWindow(ID_CUSTOM1A)->SetToolTip(custom_1a_help);
    wxString custom_1b_help = wxT("Optional extra parameter, specify value here, eg 1600");
    FindWindow(ID_CUSTOM1B)->SetHelpText(custom_1b_help);
    FindWindow(ID_CUSTOM1B)->SetToolTip(custom_1b_help);
    wxString custom_2a_help = wxT("Optional extra parameter, specify name here, eg UCI_LimitStrength");
    FindWindow(ID_CUSTOM2A)->SetHelpText(custom_2a_help);
    FindWindow(ID_CUSTOM2A)->SetToolTip(custom_2a_help);
    wxString custom_2b_help = wxT("Optional extra parameter, specify value here, eg true");
    FindWindow(ID_CUSTOM2B)->SetHelpText(custom_2b_help);
    FindWindow(ID_CUSTOM2B)->SetToolTip(custom_2b_help);
    wxString custom_a_help = wxT("Optional extra parameter, specify name here");
    FindWindow(ID_CUSTOM3A)->SetHelpText(custom_a_help);
    FindWindow(ID_CUSTOM3A)->SetToolTip(custom_a_help);
    FindWindow(ID_CUSTOM4A)->SetHelpText(custom_a_help);
    FindWindow(ID_CUSTOM4A)->SetToolTip(custom_a_help);
    FindWindow(ID_CUSTOM5A)->SetHelpText(custom_a_help);
    FindWindow(ID_CUSTOM5A)->SetToolTip(custom_a_help);
    FindWindow(ID_CUSTOM6A)->SetHelpText(custom_a_help);
    FindWindow(ID_CUSTOM6A)->SetToolTip(custom_a_help);
    wxString custom_b_help = wxT("Optional extra parameter, specify value here");
    FindWindow(ID_CUSTOM3B)->SetHelpText(custom_b_help);
    FindWindow(ID_CUSTOM3B)->SetToolTip(custom_b_help);
    FindWindow(ID_CUSTOM4B)->SetHelpText(custom_b_help);
    FindWindow(ID_CUSTOM4B)->SetToolTip(custom_b_help);
    FindWindow(ID_CUSTOM5B)->SetHelpText(custom_b_help);
    FindWindow(ID_CUSTOM5B)->SetToolTip(custom_b_help);
    FindWindow(ID_CUSTOM6B)->SetHelpText(custom_b_help);
    FindWindow(ID_CUSTOM6B)->SetToolTip(custom_b_help);
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ENGINE_RESET
void EngineDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// ID_ENGINE_PICKER
void EngineDialog::OnFilePicked( wxFileDirPickerEvent& event )
{
    wxString file = event.GetPath();
    dat.m_file = file;
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void EngineDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("\nUse this screen to select the UCI engine to use. ")
      wxT("A UCI engine is a .exe file. For example Stockfish V8 64 bit ")
      wxT("is;\n")
      wxT("stockfish_8_x64.exe.\n\n" )
      wxT("Another example, Komodo 3 64 bit is;\n")
      wxT("komodo3-64.exe." )
	  wxT("\n\n")
      wxT("One weak engine is provided. TarraschToyEngine.exe is ")
      wxT("good enough to be fun but readily beatable for most players, ")
      wxT("it is ideal for blindfold training (see the training options).")
	  wxT("\n\n")
      wxT("You can also set three engine options that are useful for ")
      wxT("human versus engine operation. Note that not all engines ")
      wxT("support all these options (the Tarrasch Toy engine doesn't ")
      wxT("actually support any of them!). If you don't care about ")
      wxT("these options then leave the default values (ponder off, ")
      wxT("hash size= 64Mbytes, max cpu cores=1) in place.")
	  wxT("\n\n")
      wxT("If you are unsure whether an .exe is a UCI engine, first ")
      wxT("run it outside the Tarrasch program. It should open a ")
      wxT("console window and quietly await a text command. The ")
      wxT("command \"uci\" should provide information followed by ")
      wxT("the response \"uciokay\". Then go \"quit\" to finish.")
	  wxT("\n\n")
      wxT("This process also reveals all parameters supported by ")
      wxT("an engine. Expert users can experiment with these using ")
      wxT("up to six optional custom parameters settings. For ")
      wxT("example Stockfish provides a parameter to artificially reduce its strength. " )
      wxT("To try it out;\n")
      wxT("  Parameter 1, set name=Skill Level and value=0") 
      wxT("\n\n" )
      wxT("Experiment with values in the range 0 to 20 (higher is ")
      wxT("stronger - start with 0, and adjust upwards if you find ")
      wxT("that insufficiently challenging). Many other engines support ")
      wxT("a similar feature using two parameters. For those engines, to limit strength ")
      wxT("to 1200 (say);\n")
      wxT("  Parameter 1, set name=UCI_Elo and value=1200\n")
      wxT("  Parameter 2, set name=UCI_LimitStrength and value=true")
      wxT("\n\n" )
      wxT("Komodo does not support this option at all and Rybka ")
      wxT("doesn't support it very well. Note that it is unclear how " )
      wxT("Stockfish's Skill Level parameter corresponds to Elo." )
      wxT("\n\n" )
      wxT("Please exercise caution when using the custom parameters!\n");
    wxMessageBox(helpText,
      wxT("Engine dialog help"),
      wxOK|wxICON_INFORMATION, this);
}
