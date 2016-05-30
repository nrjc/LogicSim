#ifndef gui_monitor_h
#define gui_monitor_h
#include <string>
#include <algorithm> //sort
#include <vector>

#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "network.h"
#include "parser.h"
#include "gui.h"
#include "guiCanvas.h"
#include "gui_constants.h"



struct opProps{

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
  /*bool operator > (const opProps& pr) const{
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
  }*/
  bool operator > (const opProps& pr) const{
    if (fullstr !=""&&pr.fullstr!="") return (fullstr>pr.fullstr);
    else
    if (dev==pr.dev)
        return(op>pr.op);
    else
        return (dev > pr.dev);
  }
  bool operator < (const opProps& pr) const{
    if (fullstr !=""&&pr.fullstr!="") return (fullstr<pr.fullstr);
    else
    if (dev==pr.dev)
      return(op<pr.op);
    else
      return (dev < pr.dev);
  }
  bool operator == (const opProps& pr) const{
    if (fullstr !=""&&pr.fullstr!="") return (fullstr==pr.fullstr);
    else
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
  bool operator > (const switchitem& pr) const{
    return (devstr>pr.devstr);
  }
  bool operator < (const switchitem& pr) const{
    return (devstr<pr.devstr);
  }
  bool operator == (const switchitem& pr) const{
    return (devstr==pr.devstr);
  }
};


class MyGLCanvas;
class MyMonManager{
  // Has access to maxmonitors and maxcycles from monitor.h


 public:
  MyMonManager(names *names_mod, network *network_mod,
                devices *devices_mod, monitor *monitor_mod, int *cyclesp, wxTextCtrl *mon_ctrl, wxTextCtrl *cmd_disp, MyGLCanvas *mycanvas);

  wxArrayString GetDevices();
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
  void Reset();
  void Tell(wxString message);  //Print message in the message box

 private:
  names *nmz;                             // pointer to names class
  network *netz;			  // pointer to network class
  devices *dmz;                           // pointer to devices class
  monitor *mmz;                           // pointer to monitor class
  MyGLCanvas *canvas;

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

#endif
