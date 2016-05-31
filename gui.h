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
  wxStreamToTextRedirector *cmdOutputRedirect;// object that performs redirecting of comand line outputs to cmddisp
  wxBoxSizer *rightsizer;                 // Sizer that contains the canvas and cmddisp
  wxTextCtrl *monctrl;                    // Text control box that displays the number of cycles completed and displayed on the canvas


  wxCheckListBox *switchwin;              // Check list window that contains the switches interface
  wxListBox *devwin;                      // List window that contains a list of devices

  names *nmz;                             // pointer to names class
  network *netz;			  // pointer to network class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  MyMonManager *monman;			  // monitor manager class instance.
  int cyclescompleted;                    // how many simulation cycles have been completed so far. Also displayed in monctrl

  // Locale and application language properties
  // (don't have to be global, reserved for future extensibility)
  wxLocale* locale;			// Store system locale 
  vector<int> default_language;		// Store list of default language enums 
  vector<int> supported_lang;		// Store supported language enums

  void SetupLanguage();           	// Set the program language
  
  void AddSwitchMonCtrl(wxSizer *control_sizer); // Function that sets up the tabbed window containing the switch and devices lists
  void SetSwitchList(); 	        // Set the switch list in switchwin
  void SetDeviceList();         	// Set the device list in devwin
  
  bool LoadNewCircuit(wxString filepath); // Builds a new network from the circuit definition file given
  void ResetContent();            	// Resets all circuit-related parameters. Called after a new circuit is successfully constructed
  
  void Tell(wxString message);    	// Print a message dirrectly into the cmddisp.

  // EVENT HANDLERS
  // Menu events
  void OnExit(wxCommandEvent& event);     // event handler for exit menu item
  void OnAbout(wxCommandEvent& event);    // event handler for about menu item
  void OnOpen(wxCommandEvent& event);	  // event handler for open menu item
  // Button events
  void OnButtonRun(wxCommandEvent& event);   // event handler for Run button
  void OnButtonCont(wxCommandEvent& event);   // event handler for Continue button
  void OnButtonSetMon(wxCommandEvent& event);   // event handler for when set monitor option is chosen.
  // Switch checkbox events
  void OnSwitchBox(wxCommandEvent& event);    // event handler for ticking/unticking a box in the switch list
  
  DECLARE_EVENT_TABLE()
};

#endif /* gui_h */
