#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm> //sort
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
  EVT_BUTTON(MY_BUTTONRUN_ID, MyFrame::OnButtonRun)
  EVT_BUTTON(MY_BUTTONCONT_ID, MyFrame::OnButtonCont)
  EVT_BUTTON(MY_BUTTONSETMON_ID, MyFrame::OnButtonSetMon)

  EVT_CHECKLISTBOX(MY_SWITCH_ID, MyFrame::OnSwitchBox)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
  EVT_TEXT_ENTER(MY_TEXTCTRL_ID, MyFrame::OnText)
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


  monctrl = new wxTextCtrl(this, MY_TEXTCTRL_ID, "0", wxDefaultPosition, CommandSize, wxTE_READONLY);
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
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, _("&File"));
  SetMenuBar(menuBar);

  // Set up main controla
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

  sim_sizer->Add(new wxStaticText(this, wxID_ANY, _("Cycles")), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"), wxDefaultPosition, wxDefaultSize);
  spin->SetForegroundColour(*wxBLACK);
  spin->SetRange(1, 500);
  sim_sizer->Add(spin, MyStdFlag);

  // Text command box to display number of cycles completed
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, _("Cycles completed")), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  sim_sizer->Add(monctrl, MyStdFlag);

  // Set up buttons dirrecting to item selection
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
  cmddisp->Clear();
  monman->Reset();
  canvas->Reset(0);
  Tell(_("Plot area reset"));
  SetSwitchList();
  SetDeviceList();

}

// EVENT HANDLERS //

void MyFrame::OnExit(wxCommandEvent &event){
  // Event handler for the exit menu item

  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event){
  // Event handler for the about menu item

  wxMessageDialog about(this, _("Software project 2016 team 6:\n Names and Scanner classes:\n\t Nicholas 仁杰 Capel.\n Parser class: \n\t 娄宇翔 (Yǔ Xiǎng Lóu),\n\t Nicholas 仁杰 Capel.\n GUI: \n\t Kamilė Rastenė."), _("About Logsim"), wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnOpen(wxCommandEvent &event){
  // Event handler for the open menu item
    if(LoadNewCircuit())
        ResetContent();
}

void MyFrame::OnButtonRun(wxCommandEvent &event){
  // Event handler for the push button
  monman->ResetMonitors();
  Tell("Running simulation for "+to_string(spin->GetValue())+" cycles");
  monman->RunNetwork(spin->GetValue());
  canvas->Reset(cyclescompleted);
}

void MyFrame::OnButtonCont(wxCommandEvent &event){
  // Event handler for the push button
  int c = spin->GetValue();
  Tell(_("Continuing simulation for ")+to_string(c)+_(" cycles"));
  if (monman->RunNetwork(spin->GetValue())){
    canvas->Render("",cyclescompleted);
  }
}

void MyFrame::OnButtonSetMon(wxCommandEvent &event){
  const wxSize mon_size = *(new wxSize(400, 400));
  MyMonDialog* mymon = new MyMonDialog(this, wxID_ANY,_("Add or set Monitor"), monman, wxDefaultPosition, mon_size);
  mymon->Centre();
  mymon->ShowModal();


}

void MyFrame::OnSwitchBox(wxCommandEvent &event){
  // Event handler for the push button
  int n = switchwin->GetSelection();

  for (int i=0; i<monman->switches.size(); i++){
    if (switchwin->IsChecked(i)!=monman->switches[i].check){
      monman->FlickSwitch(i);
      break;
    }
  }
}

void MyFrame::OnSpin(wxSpinEvent &event){
  // Event handler for the spin control

}

void MyFrame::OnText(wxCommandEvent &event){
  // Event handler for the text entry field

  wxString text;

  text.Printf("New text entered %s", event.GetString().c_str());
  canvas->Render(text);
}

void MyFrame::OnSize(wxSizeEvent& event){
  Refresh();
}

// ADDED NON-INTERFACE FUNCTIONS //

void MyFrame::SetupLanguage(){
  int default_language = wxLANGUAGE_ENGLISH;
  int supported_lang = wxLANGUAGE_LITHUANIAN;
  wxLocale* locale = new wxLocale(default_language);
  int systLang = locale->GetSystemLanguage();

  // If the system language matches the program language, display a message and do nothing.
  if (systLang==default_language){
    cout<<"Application started English."<<endl;
    return;
  }

  if (locale->IsAvailable(systLang)){
    locale = new wxLocale(systLang);
    locale->AddCatalogLookupPathPrefix(_("."));
    if(locale->AddCatalog(_("guitext")))
        std::cerr << "AddCatalog succeeded\n";
      else
        std::cerr << "AddCatalog failed\n";

      if(! locale->IsOk() )
        std::cerr << "selected language is wrong" << std::endl;
  }
  else cout<<"The selected language is not supported. The applination will default to English."<< endl;

}

bool MyFrame::LoadNewCircuit(){

    wxFileDialog openFileDialog(this, _("Please open logge file"), "", "",
                       "logge files (*.ge)|*.ge|text files (*.txt)|*.txt", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return false;     // the user changed idea...

    wxString filepath = openFileDialog.GetPath();
    // proceed loading the file chosen by the user;
    // this can be done with e.g. wxWidgets input streams:
    wxFileInputStream input_stream(filepath);
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", filepath);
        return false;
    }
    else
    {

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
}

void MyFrame::SetSwitchList(){
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
  devwin->Clear();
  devwin->Append(monman->GetDevices());
}

void MyFrame::AddSwitchMonCtrl(wxSizer *control_sizer){

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
  //cmddisp->Newline();
  cmddisp->AppendText(message+"\n");
}

