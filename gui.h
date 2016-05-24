#ifndef gui_h
#define gui_h

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/wfstream.h>
#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/checklst.h>
//#include <wx/menuitem.h>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "parser.h"

enum {
  MY_SPINCNTRL_ID = wxID_HIGHEST + 1,
  MY_TEXTCTRL_ID,
  MY_BUTTONRUN_ID,
  MY_BUTTONCONT_ID,
  MY_BUTTONSETMON_ID,
  MY_SWITCH_ID,
  MY_NOTEBOOK_ID,
  MY_FRAME_ID,

  MY_ADDMON_ID,
  MY_REMMON_ID,
}; // widget identifiers

class MyGLCanvas;
class MyMonManager;

struct opProps
{
  name dev;
  name op=-1;
  string devstr;// Actual name of device
  string opstr; // String for output.
  string fullstr; // Output name string as it appears on the GUI interface.
  opProps(name devid=-1, name opid=-1, string devs="", string ops=""){
    dev=devid;
    op=opid;
    devstr = devs;
    opstr = ops;
    fullstr = devs;
    if (opstr !="") fullstr+="."+opstr;
  }
  // from http://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-objects
  bool operator > (const opProps& pr) const{
    if (dev==pr.dev)
        return(op>pr.op);
    else
        return (dev > pr.dev);
  }
  bool operator < (const opProps& pr) const{
    if (dev==pr.dev)
      return(op<pr.op);
    else
      return (dev < pr.dev);
  }
  bool operator == (const opProps& pr) const{
    return(dev==pr.dev&&op==pr.op);
  }
};

struct switchitem{
  name dev;
  bool check;
  string devstr;

  switchitem(name devid, asignal signlev, string dstring){
    dev=devid;
    if(signlev==low)check=false;
    if(signlev==high)check=true;
    devstr=dstring;
  }

};


class MyFrame: public wxFrame{

 public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
	  names *names_mod = NULL, network *network_mod = NULL, devices *devices_mod = NULL, monitor *monitor_mod = NULL,
	  long style = wxDEFAULT_FRAME_STYLE); // constructor

 private:
  MyGLCanvas *canvas;                     // OpenGL drawing area widget to draw traces
  wxSpinCtrl *spin;                       // control widget to select the number of cycles
  wxTextCtrl *cmddisp;                    // textbox that displays error and warning text. Replaces command line outputs
  wxBoxSizer *rightsizer;
  wxTextCtrl *monctrl;                    // Stored as global so that it could be passed to MyMonMan uppon open.
  //wxScrolledWindow* switchwin;
  //wxBoxSizer *switch_sizer;
  wxCheckListBox *switchwin;

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
  void AddSwitchMonCtrl(wxSizer *control_sizer);
  void ResetContent();

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

class MyMonManager{
  // Has access to maxmonitors and maxcycles from monitor.h


 public:
  MyMonManager(names *names_mod, network *network_mod,
                devices *devices_mod, monitor *monitor_mod, int *cyclesp, wxTextCtrl *mon_ctrl, wxTextCtrl *cmd_disp);

  wxArrayString GetMonitoredList();  //Returns array of monitored output names
  wxArrayString GetUnmonitoredList();//Returns array of unmonitored output names
  wxArrayString GetSwitches();
  vector<switchitem> switches;

  bool AddMonitor(int m);      // Adds a monitor and removes it from the unmonitored list
  bool RemoveMonitor(int m);      // Removes a monitor and adds it to the monitored list

  bool RunNetwork(int ncycles);		// Runs network for ncycles
  void ResetMonitors();			// Executes all actions involved in reseting monitors
  void IncrementCycles(int n);		// Executes all actions involved in incrementing number of cycles on the gui
  void FlickSwitch(int n);

 private:
  names *nmz;                             // pointer to names class
  network *netz;			  // pointer to network class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class

  int *cyclescompletedp;
  wxTextCtrl *montextctrl;
  wxTextCtrl *cmddisp;


  vector<opProps> allops;		// Vector to store info about all outputs. Reserved for future use, eg. Clear.
  vector<opProps> unmonitored;		// Vector to store all unmonitored outputs.
  vector<opProps> monitored;		// Vector to store all monitored outputs.


};

class MyMonDialog: public wxDialog{
 public:
  MyMonDialog(wxWindow *parent, wxWindowID id, const wxString &title,MyMonManager *mon_man,
          const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize,
          long style=wxDEFAULT_DIALOG_STYLE, const wxString &name=wxDialogNameStr);//Default constructor
 private:
  MyMonManager *monman;
  wxListBox *optListBox;
  wxListBox *monListBox;

  void RefreshLists();

  void OnBtnAddMon(wxCommandEvent& event);// event to handle Remove button click in monitor dialog
  void OnBtnRemMon(wxCommandEvent& event);// event to handle Add button click in monitor dialog

  DECLARE_EVENT_TABLE()
};
#endif /* gui_h */
