#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/wfstream.h>
//#include <wx/menuitem.h>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"

enum { 
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_TEXTCTRL_ID,
  MY_BUTTONRUN_ID,
  MY_BUTTONCONT_ID,
  MY_BUTTONSETMON_ID,
  MY_SWITCH_ID,
  MY_NOTEBOOK_ID,
}; // widget identifiers

class MyGLCanvas;

class MyFrame: public wxFrame
{
 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, 
	  names *names_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL, 
	  long style = wxDEFAULT_FRAME_STYLE); // constructor
 private: 
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  names *nmz;                             // pointer to names class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  int cyclescompleted;                    // how many simulation cycles have been completed
  void runnetwork(int ncycles);           // function to run the logic network
  
  // Added functions
  void SetSwitchList(wxWindow *parent, wxSizer* sizer);		//Add checkboxes for editing switches.
  void SetSwitchList(wxCheckListBox *switchbox);		//Add checkboxes for editing switches.
  void AddSwitchMonCtrl(wxSizer *control_sizer);
  
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
  
  
  //void SetColours(wxMenuItem* item);
  //void SetColours(wxWindow* item);
  DECLARE_EVENT_TABLE()
};
    
class MyGLCanvas: public wxGLCanvas
{
 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1, bool spinchange = false); // function to draw canvas contents
 private:
  wxGLContext *context;              // OpenGL rendering context
  bool init;                         // has the OpenGL context been initialised?
  int pan_x;                         // the current x pan
  int pan_y;                         // the current y pan
  int disp_h;			     // height of the displayed drawing area
  int disp_w;			     // width of the displayed drawing area
  double zoom;                       // the current zoom
  int cyclesdisplayed;               // how many simulation cycles have been displayed
  monitor *mmz;                      // pointer to monitor class, used to extract signal traces
  names *nmz;                        // pointer to names class, used to extract signal names
  void InitGL();                     // function to initialise OpenGL context
  void OnSize(wxSizeEvent& event);   // event handler for when canvas is resized
  void OnPaint(wxPaintEvent& event); // event handler for when canvas is exposed
  void OnMouse(wxMouseEvent& event); // event handler for mouse events inside canvas
  void DrawAxes(float x_low, float x_high, float y_low, float y_high); // draw axes for the trace
  DECLARE_EVENT_TABLE()
};
    
#endif /* gui_h */
