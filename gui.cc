#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm> //sort, find
#include <vector>
//wxSize
#include <wx/gdicmn.h>
#include <wx/spinctrl.h>
// tabs
#include <wx/notebook.h>
#include <wx/aui/auibook.h>

// Monitors
#include <wx/listbox.h>
#include <wx/arrstr.h>
//#include <wx/menuitem.h>
#include <wx/string.h>
#include <wx/richtext/richtextctrl.h>

#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "parser.h"

#define USE_GUI

using namespace std;

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  //EVT_MENU(MY_LANG_ID, MyFrame::OnLanguage)

  EVT_BUTTON(MY_BUTTONRUN_ID, MyFrame::OnButtonRun)
  EVT_BUTTON(MY_BUTTONCONT_ID, MyFrame::OnButtonCont)
  EVT_BUTTON(MY_BUTTONSETMON_ID, MyFrame::OnButtonSetMon)

  EVT_CHECKLISTBOX(MY_SWITCH_ID, MyFrame::OnSwitchBox)
  //EVT_SIZE(MyFrame::OnSize)

END_EVENT_TABLE()

MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, network *network_mod, devices *devices_mod, monitor *monitor_mod, long style):
  wxFrame(parent, MY_FRAME_ID, title, pos, size, style){
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers

  SetIcon(wxIcon(wx_icon));

  cyclescompleted = 0;
  nmz = names_mod;
  netz = network_mod;
  dmz = devices_mod;
  mmz = monitor_mod;


  if (nmz == NULL || netz==NULL || dmz == NULL || mmz == NULL) {
    cout<<_("Cannot run without names, network, devices and monitor classes")<<endl;
    exit(1);
  }
  // SUPPRESS CIRCUIT EXECUTION MESSAGES
  dmz->debug(false);


  monctrl = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, CommandSize, wxTE_READONLY);
  const wxSize MyCmdSize = wxSize(size.GetWidth()-200, 75);
  //cmddisp = new wxTextCtrl();
  cmddisp = new wxTextCtrl(this, -1, wxString(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY|wxTE_BESTWRAP);
  wxTextAttr myTextStyle = cmddisp->GetDefaultStyle();
  myTextStyle.SetFontFamily(wxFONTFAMILY_TELETYPE);
  cmddisp->SetDefaultStyle(myTextStyle);
  const wxSize MyMinCmdSize = wxSize(20, 75);
  cmddisp->SetMinSize(MyMinCmdSize);
  cmdOutputRedirect = new wxStreamToTextRedirector(cmddisp);

  SetupLanguage();

  const wxSize MyCanvasSize = wxSize(size.GetWidth()-200, size.GetHeight()-173);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, wxDefaultPosition, MyCanvasSize);

  monman = new MyMonManager(nmz, netz, dmz, mmz, &cyclescompleted, monctrl, cmddisp, canvas);

  // Set up colors
  SetBackgroundColour(BcColour);
  SetForegroundColour(*wxWHITE);

  // Set up file menu
  wxMenu *fileMenu = new wxMenu;
  wxMenuItem* openMenu = fileMenu->Append(wxID_OPEN, _("&Open"));
  fileMenu->Append(wxID_ABOUT, _("&About"));
  fileMenu->Append(wxID_EXIT, _("&Quit"));

  //wxMenu *langMenu = new wxMenu;
  //langMenu->AppendRadioItem(MY_LANG_ID, _("English"));
  //langMenu->AppendRadioItem(MY_LANG_ID, _("Lithuanian"));

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _("&File"));
  //menuBar->Append(langMenu, _("Language"));
  SetMenuBar(menuBar);
  // Set up main controls
  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->AddSpacer(7);


  // Create control panel
  wxBoxSizer *control_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticBoxSizer *sim_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Simulation"));

  // Sizer to horizontally align  Run and Continue buttons.
  wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
  button_sizer->Add(new wxButton(this, MY_BUTTONRUN_ID, _("Run"), wxDefaultPosition, RunSize), MyStdFlag);
  button_sizer->Add(new wxButton(this, MY_BUTTONCONT_ID, _("Continue"), wxDefaultPosition, ContSize), MyStdFlag);
  sim_sizer->Add(button_sizer);

  // Add cycle count control
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, _("Cycles")), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  spin = new wxSpinCtrl(this, wxID_ANY, wxString("10"), wxDefaultPosition, wxDefaultSize);
  spin->SetForegroundColour(*wxBLACK);
  spin->SetRange(1, 500);
  sim_sizer->Add(spin, MyStdFlag);

  // Text command box to display number of cycles completed
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, _("Cycles completed")), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  sim_sizer->Add(monctrl, MyStdFlag);

  // Set up button to open monitor management dialog
  wxStaticBoxSizer *options_sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Configure simulation"));
  options_sizer->Add(new wxButton(this, MY_BUTTONSETMON_ID, _("Add/remove monitors")), MyStdFlag);

  control_sizer->Add(sim_sizer);
  control_sizer->Add(options_sizer);

  // Add tabbed control windows
  AddSwitchMonCtrl(control_sizer);


  wxBoxSizer *rightsizer = new wxBoxSizer(wxVERTICAL);
  // Create GL canvas and add

  rightsizer-> Add(canvas, 1, wxEXPAND | wxALL, 5);
  rightsizer-> Add(cmddisp, 0.3,  wxEXPAND | wxLEFT|wxRIGHT, 5);
  topsizer->Add(control_sizer, 0, wxALIGN_CENTER);
  topsizer->Add(rightsizer, 1, wxEXPAND | wxALL, 10);
  SetSizeHints(MinWinSize);
  SetSizer(topsizer);
}

void MyFrame::ResetContent(){
// Call all children classes to reset all circuit-related content
  cmddisp->Clear();
  monman->Reset();
  canvas->Reset(0);
  Tell(_("Plot area reset"));
  // Reset lists in tab controls
  SetSwitchList();
  SetDeviceList();

}

// EVENT HANDLERS //

void MyFrame::OnExit(wxCommandEvent &event){
  // Event handler for the exit menu item
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event){
  // Display a message dialog showing infromation on the application
  Tell(to_string(event.GetSelection()));
  wxMessageDialog about(this, _("Software project 2016 team 6:\n Names and Scanner classes:\n\t Nicholas 仁杰 Capel.\n Parser class: \n\t 娄宇翔 (Yǔ Xiǎng Lóu),\n\t Nicholas 仁杰 Capel.\n GUI: \n\t Kamilė Rastenė."), _("About Logsim"), wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnOpen(wxCommandEvent &event){
  /* Event handler for the open menu item
   * Open a file dialog for the user to choose a circuit definition file.
   * If the file is valid, try loading a circuit from it. If this passes,
   * reset the circuit management.
  */
    wxFileDialog openFileDialog(this, _("Please open logge file"), "", "",
                       "logge files (*.ge)|*.ge|text files (*.txt)|*.txt", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL) return;
Tell(to_string(event.GetSelection()));
    wxString filepath = openFileDialog.GetPath();

    wxFileInputStream input_stream(filepath);
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", filepath);
    }
    else
    {
      if(LoadNewCircuit(filepath))
        ResetContent();
    }
}


void MyFrame::OnButtonRun(wxCommandEvent &event){
  /* Event handler for the Run button.
   * Reset monitors and run for the amount of cycles indicated in the spin box.
  */
  monman->ResetMonitors();
  Tell(_("Running simulation for ")+to_string(spin->GetValue())+_(" cycles"));
  monman->RunNetwork(spin->GetValue());
  canvas->Reset(cyclescompleted); // cyclescompleted was changed by monman
}

void MyFrame::OnButtonCont(wxCommandEvent &event){
  /* Event handler for the Continue button
   * Get the spin box value, continue running the network and refresh canvas.
  */
  int c = spin->GetValue();
  Tell(_("Continuing simulation for ")+to_string(c)+_(" cycles"));
  if (monman->RunNetwork(spin->GetValue())){
    canvas->Render("",cyclescompleted); // cyclescompleted was updated by monman
  }
}

void MyFrame::OnButtonSetMon(wxCommandEvent &event){
  /* Create a MyMonDialog window in which the user manages the list of
   * monitor points.
   */
  const wxSize mon_size = *(new wxSize(400, 400));
  MyMonDialog* mymon = new MyMonDialog(this, wxID_ANY,_("Add or set Monitor"), monman, wxDefaultPosition, mon_size);
  mymon->Centre();
  mymon->ShowModal();
}

void MyFrame::OnSwitchBox(wxCommandEvent &event){
  /* Event handler for ticking/unticking a box in switch list.
   * Go through all items on the list and check if its state is different
   * from the state stored in the switches vector. If it differs, update
   * and break loop, since only one item at a time can be triggered.
   *
   * Intuitively, it would seem more sensible to get the selected item
   * and trigger that, however the index of the previous selection is
   * returned instead.
   * EDIT: it may have been possible to handle this through the GetInt()
   * command. This is a possible future improvement.
   */
  for (int i=0; i<monman->switches.size(); i++){
    if (switchwin->IsChecked(i)!=monman->switches[i].check){
      monman->FlickSwitch(i);
      break;
    }
  }
}

// ADDED NON-INTERFACE FUNCTIONS //

void MyFrame::SetupLanguage(){
  /* Handles the initial setup of the application language.
   * If the system language matches the language all of this application's
   * text has been written in, no catalog needs to be loaded and the function
   * returns. Otherwise, if this application supports the system language
   * (or the one specified on the command line), a language pack is loaded.
   * If the language is not supported, an error is reported and Logsim
   * loads with its default language.
   */

  default_language.push_back(wxLANGUAGE_ENGLISH);
  default_language.push_back(wxLANGUAGE_ENGLISH_UK);
  default_language.push_back(wxLANGUAGE_ENGLISH_US);
  default_language.push_back(wxLANGUAGE_ENGLISH_AUSTRALIA);
  default_language.push_back(wxLANGUAGE_ENGLISH_BELIZE);
  default_language.push_back(wxLANGUAGE_ENGLISH_BOTSWANA);
  default_language.push_back(wxLANGUAGE_ENGLISH_CANADA);
  default_language.push_back(wxLANGUAGE_ENGLISH_CARIBBEAN);
  default_language.push_back(wxLANGUAGE_ENGLISH_DENMARK);
  default_language.push_back(wxLANGUAGE_ENGLISH_EIRE);
  default_language.push_back(wxLANGUAGE_ENGLISH_JAMAICA);
  default_language.push_back(wxLANGUAGE_ENGLISH_NEW_ZEALAND);
  default_language.push_back(wxLANGUAGE_ENGLISH_PHILIPPINES);
  default_language.push_back(wxLANGUAGE_ENGLISH_SOUTH_AFRICA);
  default_language.push_back(wxLANGUAGE_ENGLISH_TRINIDAD);
  default_language.push_back(wxLANGUAGE_ENGLISH_ZIMBABWE);

  // Set up as a vector in case more languages become available in the future.
  supported_lang.push_back(wxLANGUAGE_LITHUANIAN);

  locale = new wxLocale(default_language[0]);
  int systLang = locale->GetSystemLanguage();

  // If the system language matches the program language, display a message and return.
  if (find(default_language.begin(),default_language.end(), systLang )!=default_language.end()){
    cout<<"Application started in English."<<endl;
    return;
  }
  // Otherwise try loading
  if (locale->IsAvailable(systLang)){
    locale = new wxLocale(systLang);
    locale->AddCatalogLookupPathPrefix(_("."));
    if(locale->AddCatalog(_("guitext")))
        Tell(_("Language pack loaded"));
      else{
        cout<<"The requested application is not supported by the application."<<endl;
        cout<<"Logsim started in English."<<endl;
      }
      if(! locale->IsOk() )
        std::cerr << "selected language is wrong" << std::endl;
  }
  else cout<<"The selected language is not supported. The application will default to English."<< endl;

}

bool MyFrame::LoadNewCircuit(wxString filepath){
/* Initialises new scanner and parser objects to build the new network
 * according to the circuit described in filepath. If the network was
 * built successfully, the content pointed to by the current names,
 * network, devices and monitor pointers is overwritten with the newly
 * built network information and the function returns true.
 * Otherwise, return false.
 */

  names *new_nmz =new names();
  network *new_netz = new network(new_nmz);
  devices *new_dmz = new devices(new_nmz, new_netz);
  monitor *new_mmz = new monitor(new_nmz, new_netz);

  scanner *smz = new scanner(new_nmz, filepath.mb_str());
  error *err = new error(smz);
  parser *pmz = new parser(new_netz, new_dmz, new_mmz, smz,err);

  if (pmz->readin ()){
    cout<<_("Network built")<<endl;
    *nmz = *new_nmz;
    *netz = *new_netz;
    *dmz = *new_dmz;
    *mmz = *new_mmz;
    return true;
  }
  return false;

}

void MyFrame::SetSwitchList(){
/* Create a list of switches for the application.
 * MyMonManager class executes the back-end code required to assemble the list.
 */

  asignal currswstate;

  string nmstring;
  wxArrayString tempstring = monman->GetSwitches();
  switchwin->Clear();
  switchwin->Append(tempstring);
  // Going backwards through the list, because the network adds new
  //devices/switches at the beginning of the device list
  for (int i=0; i<monman->switches.size(); i++)
  {
  switchwin->Check(i,monman->switches[i].check);
  }
}

void MyFrame::SetDeviceList(){
  /* Renew the list of existing devices. Uses MyMonManager clsss for the back-end
   */
  devwin->Clear();
  devwin->Append(monman->GetDevices());
}

void MyFrame::AddSwitchMonCtrl(wxSizer *control_sizer){
  /* Create the tabbed control window to view the list of switches and 
   * devices in the application.
   * Calls SetSwitchList and SetDevice list in the process.
   */

  wxAuiNotebook *note_ctrl = new wxAuiNotebook(this, MY_NOTEBOOK_ID, wxDefaultPosition, wxDefaultSize,
                                                wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS);
  note_ctrl->SetMinSize(MinTabSize);
  note_ctrl->SetForegroundColour(*wxBLACK);

  switchwin = new wxCheckListBox(note_ctrl, MY_SWITCH_ID);
  switchwin->SetBackgroundColour(*wxWHITE);
  switchwin->SetForegroundColour(*wxBLACK);

  SetSwitchList();

  devwin = new wxListBox(note_ctrl, wxID_ANY);
  devwin->SetBackgroundColour(*wxWHITE);
  devwin->SetForegroundColour(*wxBLACK);

  SetDeviceList();

  note_ctrl->AddPage(switchwin,_("Switches"));
  note_ctrl->AddPage(devwin, _("Devices"));

  control_sizer->Add(note_ctrl, MyTabFlag);
  control_sizer->AddSpacer(5);


}

void MyFrame::Tell(wxString message){
  /* Print a message on the command window.
   * This function is used for most messages that are translated, because
   * it works better non-ASCII characters than cout.
   */
  cmddisp->AppendText(message+"\n");
}

