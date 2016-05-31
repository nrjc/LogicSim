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
/* A structure to store all relevant device output properties for easy
 * access and use by the MyMonManager class.
 * It is left global in case more GUI classes need to store such properties.
 */
  name dev;       // Device ID
  name op=-1;     // Output ID
  string devstr;  // Actual name of device
  string opstr;   // String for output.
  string fullstr; // Output name string as it appears on the GUI interface.
  
  // Constructor
  opProps(name devid=-1, name opid=-1, string devs="", string ops=""){
    dev=devid;
    op=opid;
    devstr = devs;
    opstr = ops;
    fullstr = devs;
    if (opstr !="") fullstr+="."+opstr;
  }
  // Operator definitions for sorting vectors by fullstr string.
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
  /*
  // Alternative operator definitions for sorting by ID
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
  }*/
};

struct switchitem{
  /* A structure to store all relevant switch properties for easy access
   * and use by MyMonManager and MyFrame classes
   * 
   */
  name dev;       // Device ID
  bool check;     // true means on, false means off. Corresponds to switch checkbox state
  string devstr;  // Device name string

  // Constructor
  switchitem(name devid, asignal signlev, string dstring){
    dev=devid;
    if(signlev==low)check=false;
    if(signlev==high)check=true;
    devstr=dstring;
  }
  
  // Operators for list sorting by devstr.
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
  /*********************************************************************
   * 
   * Class used to separate comunications with monitor and devices
   * classes from most parts of the GUI.
   * This improves object-orientation and maintainability and is more
   * in line with the principles of GUI design, such as MVVM.
   * 
   ********************************************************************/

 public:
  MyMonManager(names *names_mod, network *network_mod,
                devices *devices_mod, monitor *monitor_mod, int *cyclesp, wxTextCtrl *mon_ctrl, wxTextCtrl *cmd_disp, MyGLCanvas *mycanvas);

  wxArrayString GetDevices();         //Returns an array of output names
  wxArrayString GetMonitoredList();   //Returns an array of monitored output names
  wxArrayString GetUnmonitoredList(); //Returns an array of unmonitored output names
  wxArrayString GetSwitches();        //Returns an array of switch names

  vector<switchitem> switches;        // Vector that stores properties of all switches. MyFrame::SetSwitchList accesses it.
  
  bool AddMonitor(int m);             // Adds a monitor and removes it from the unmonitored list
  bool RemoveMonitor(int m);          // Removes a monitor and adds it to the monitored list

  bool RunNetwork(int ncycles);		    // Runs network for ncycles
  void ResetMonitors();			          // Executes all actions involved in reseting monitors
  void IncrementCycles(int n);		    // Executes all actions involved in incrementing number of cycles on the gui
  void FlickSwitch(int n);            // Change the state of the switch switchitem[n]
  void Reset();
  void Tell(wxString message);        // Print message in the message box

 private:
  names *nmz;                         // pointer to names class
  network *netz;			                // pointer to network class
  devices *dmz;                       // pointer to devices class
  monitor *mmz;                       // pointer to monitor class
  MyGLCanvas *canvas;

  int *cyclescompletedp;              // pointer to cyclescompleted variable in MyFrame
  wxTextCtrl *montextctrl;            // pointer to "Cycles completed" textbox
  wxTextCtrl *cmddisp;

  vector<opProps> allops;		        // Vector to store info about all outputs.
  vector<opProps> unmonitored;		  // Vector to store all unmonitored outputs.
  vector<opProps> monitored;		    // Vector to store all monitored outputs.
};


class MyMonDialog: public wxDialog{
  /*********************************************************************
   * Class for "Add or Remove monitors" dialog window.
   * Object created by MyFrame
   * Connects GUI back-end of adding and removing monitors in MyMonManager
   * class to the front end, controlled by the user.
   ********************************************************************/
 public:
  MyMonDialog(wxWindow *parent, wxWindowID id, const wxString &title,MyMonManager *mon_man,
          const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize,
          long style=wxDEFAULT_DIALOG_STYLE, const wxString &name=wxDialogNameStr);//Default constructor
 private:
  MyMonManager *monman;           // Pointer to MyMonManager class instance
  wxListBox *optListBox;          // Pointer to the unmonitored output list box
  wxListBox *monListBox;          // Pointer to the monitored output list box

  void RefreshLists();            // Loads new lists of outputs into the two list boxes

  void OnBtnAddMon(wxCommandEvent& event);// event to handle "Remove button" click in monitor dialog
  void OnBtnRemMon(wxCommandEvent& event);// event to handle "Add button" click in monitor dialog

  DECLARE_EVENT_TABLE()
};

#endif
