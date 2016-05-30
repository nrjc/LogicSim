#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/control.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
//#include <wx/richtext/richtextctrl.h>
#include <wx/wfstream.h>
#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/checklst.h>
#include <wx/listbox.h>
//#include <wx/menuitem.h>
#include <wx/intl.h> // Support internationalisation

#include "guiCanvas.h"
#include "gui_monitor.h"
#include "gui_constants.h"
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "parser.h"


class MyGLCanvas;
class MyMonManager;

class MyFrame: public wxFrame{

 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
	  names *names_mod = NULL, network *network_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL,
	  long style = wxDEFAULT_FRAME_STYLE); // constructor

 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  wxTextCtrl *cmddisp;                    // textbox that displays error and warning text. Replaces command line outputs
  wxStreamToTextRedirector *cmdOutputRedirect;
  wxBoxSizer *rightsizer;
  wxTextCtrl *monctrl;                    // Stored as global so that it could be passed to MyMonMan uppon open.
  //wxScrolledWindow* switchwin;
  //wxBoxSizer *switch_sizer;
  wxCheckListBox *switchwin;
  wxListBox *devwin;

  names *nmz;                             // pointer to names class
  network *netz;			  // pointer to network class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  MyMonManager *monman;			  // monitor manaker class instance.
  int cyclescompleted;                    // how many simulation cycles have been completed

  // Added functions
  //void SetSwitchList(wxWindow *parent, wxSizer* sizer);		//Add checkboxes for editing switches.
  //void SetSwitchList(wxCheckListBox *switchbox);		//Add checkboxes for editing switches.
  void SetupLanguage();     // Set the program language depending on the request.
  void SetSwitchList();
  void SetDeviceList();
  void AddSwitchMonCtrl(wxSizer *control_sizer);
  void ResetContent();
  bool LoadNewCircuit();
  void Tell(wxString message);  //Print message in the message box

  // Event handlers
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnOpen(wxCommandEvent& event);	  // event handler for open menu item
  void OnButtonRun(wxCommandEvent& event);   // event handler for Run button
  void OnButtonCont(wxCommandEvent& event);   // event handler for Continue button
  void OnSwitchBox(wxCommandEvent& event);
  void OnButtonSetMon(wxCommandEvent& event);   // event handler for when set monitor option is chosen.
  void OnSpin(wxSpinEvent& event);        // event handler for spin control
  void OnText(wxCommandEvent& event);     // event handler for text entry field
  void OnSize(wxSizeEvent& event);   // event handler for when frame is resized
 //void SetColours(wxMenuItem* item);
  //void SetColours(wxWindow* item);
  DECLARE_EVENT_TABLE()
};

#endif /* gui_h */
