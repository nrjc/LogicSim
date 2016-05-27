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
  void SetSwitchList();
  void SetDeviceList();
  void AddSwitchMonCtrl(wxSizer *control_sizer);
  void ResetContent();
  bool LoadNewCircuit();
  void Tell(string message);  //Print message in the message box

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

class MyGLCanvas: public wxGLCanvas{

 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1, bool spinchange = false); // function to draw canvas contents
  void Reset(int cycles=-1);		// Called on resetting moitors
 private:
  wxGLContext *context;              // OpenGL rendering context
  bool init;                         // has the OpenGL context been initialised?
  int pan_x;                         // the current x pan
  int pan_y;                         // the current y pan
  int disp_h;			     // height of the displayed drawing area
  int disp_w;			     // width of the displayed drawing area
  double zoom;                       // the current zoom
  const float minzoom = 0.8;
  const float maxzoom = 2.5;

  int cyclesdisplayed;               // how many simulation cycles have been displayed
  monitor *mmz;                      // pointer to monitor class, used to extract signal traces
  names *nmz;                        // pointer to names class, used to extract signal names
  void InitGL();                     // function to initialise OpenGL context
  void OnSize(wxSizeEvent& event);   // event handler for when canvas is resized
  void OnPaint(wxPaintEvent& event); // event handler for when canvas is exposed
  void OnMouse(wxMouseEvent& event); // event handler for mouse events inside canvas
  void DrawAxes(float x_low, float x_spacing, int cycles, float y_low, float y_high); // draw axes for the trace
  void NameAxes(float x_low, float x_spacing, int cycles, float y_low, float st_height, string monname); // draw axes text labels for the trace
  void FixPan(); 		// Fixes the canvas pan
  void FixZoom(); 		// Fixes the canvas zoom
  DECLARE_EVENT_TABLE()
};

#endif /* gui_h */
