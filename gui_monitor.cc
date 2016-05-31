#include "gui_monitor.h"

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

using namespace std;

////////////////////////////////////////////////////////////////////////
// MYMONMANAGER: added class to make monitor point managing easier and more object-oriented
MyMonManager::MyMonManager(names *names_mod, network *network_mod, devices *devices_mod,
                          monitor *monitor_mod, int *cyclesp, wxTextCtrl *mon_ctrl, wxTextCtrl *cmd_disp,MyGLCanvas *mycanvas){
  /* MyMonManager constructor.
   * Initialise the global variables and pointers passed by MyFrame and
   * Call Reset() to initialise all other variables.
   */
  nmz = names_mod;
  netz = network_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  cyclescompletedp = cyclesp;
  montextctrl = mon_ctrl;
  cmddisp = cmd_disp;
  canvas = mycanvas;
  Reset();

}

void MyMonManager::Reset(){
  /* Reset all content not given in the constructor.
   * This involves calling various functions in the network and names
   * classes
   */
  name dev, outp;
  string devstr, opstr;
  opProps temp;

  // clear all vectors
  allops.clear();
  unmonitored.clear();
  monitored.clear();
  switches.clear();

  // find all outputs and all switches
  devlink d = netz->devicelist();
  outplink o;

  while( d != NULL){
    dev = d->id;
    devstr = nmz->getnamefromtable(dev);
  
    o = d->olist;
    while(o != NULL)
    {
      outp=o->id;
      if (outp==-1){
      temp = *(new opProps(dev, outp, devstr));
      // temp.opstr preset to ""
      allops.push_back(temp);
      // Add a switch
      if(d->kind==aswitch){
        switches.push_back(switchitem(dev,d->swstate, devstr));
      }
      } else {
      opstr = nmz->getnamefromtable(outp);
      temp = *(new opProps(dev, outp, devstr, opstr));
      allops.push_back(temp);

      }
      o = o->next;
    }
    d=d->next;
  }
  // Sort the allops and switches lists
  sort(allops.begin(), allops.end());
  sort(switches.begin(), switches.end());

  // Assemble list of current monitor points
  for(int i=0; i<mmz->moncount();i++){

    mmz->getmonname(i, dev, outp);
    if (outp==-1){
      temp = *(new opProps(dev, outp, nmz->getnamefromtable(dev)));
      monitored.push_back(temp);
      } else {
      temp=*(new opProps(dev, outp, nmz->getnamefromtable(dev), nmz->getnamefromtable(outp) ));
      monitored.push_back(temp);
      }
  }
  // Sort the monitored list
  // EDIT: removed to make the list be in the same order as the traces on canvas. 
  //sort(monitored.begin(), monitored.end());

  //cout<<" Assembling unmonitored list "<<endl;
  // Assemble a list of unmonitored outputs. For each element of allops
  // add it to unmonitored, if it does not exist in monitored. Since the
  // number of monitors is more restricted than the number of devices,
  // this strategy may help exploit temporal locality of reference. And
  // speed up the process.
  for(int i=0; i<allops.size();i++){
    bool found=false;
    for(int j=0; j<monitored.size(); j++){
      if (allops[i]==monitored[j]){
        found=true;
        break;
      }
      else{
      }
    }
    if(!found) unmonitored.push_back(allops[i]);
  }
  // Sort the unmonitored list
  sort(unmonitored.begin(), unmonitored.end());

  ResetMonitors();

}

wxArrayString MyMonManager::GetSwitches(){
  /* Assembles a list of Switches for MyFrame::SetSwitchList()
   */
  wxArrayString swlist = wxArrayString();
  for(int i=0; i<switches.size(); i++)
  {
    swlist.Add(switches[i].devstr);
  }
  return swlist;
}

wxArrayString MyMonManager::GetDevices(){
  /* Assembles a list of devices for MyFrame::SetDeviceList()
   */
  wxArrayString oplist = *(new wxArrayString);
  oplist.Add(allops[0].devstr);
  for(int i=1; i<allops.size(); i++)
  {
    if (allops[i].devstr!=allops[i-1].devstr)
        oplist.Add(allops[i].devstr);
  }
  return oplist;
}

wxArrayString MyMonManager::GetMonitoredList(){
  /* Assembles a list of monitorerd outputs for MyMonDialog;
   */
  wxArrayString monlist = *(new wxArrayString);
  for(int i=0; i<monitored.size(); i++)
  {
    monlist.Add(monitored[i].fullstr);
  }
  return monlist;
}

wxArrayString MyMonManager::GetUnmonitoredList(){
  /* Assembles a list of unmonitorerd outputs for MyMonDialog;
   */
  wxArrayString unmonlist = *(new wxArrayString);
  for(int i=0; i<unmonitored.size(); i++)
  {
    unmonlist.Add(unmonitored[i].fullstr);
  }
  return unmonlist;

}

bool MyMonManager::AddMonitor(int m){
  /* GUI Back-end for adding a monitor point.
   * Updates local lists and calls the monitor class to update.
   * Resets canvas to prompt the user of the changes
   */
  if(m>unmonitored.size()|| m<0 ) return false;

  if(unmonitored.size()==maxmonitors){
    Tell(_("Monitor count limit (")+to_string(maxmonitors)+_(") exceeded"));
    return false;
  }

  monitored.push_back(unmonitored[m]);
  // Sort the monitored list 
  // EDIT: removed to make the list be in the same order as the traces on canvas. 
  //sort(monitored.begin(), monitored.end());

  bool ok;
  mmz->makemonitor(unmonitored[m].dev, unmonitored[m].op, ok);
  unmonitored.erase(unmonitored.begin()+m);

  ResetMonitors();
  Tell(_("Plot area Cleared"));
  canvas->Reset(0);
  return true;
}

bool MyMonManager::RemoveMonitor(int m){
  /* GUI Back-end for removing a monitor point.
   * Updates local lists and calls the monitor class to update.
   * Resets canvas to prompt the user of the changes
   */
  if(m>monitored.size()|| m<0) return false;
  unmonitored.push_back(monitored[m]);
  // Sort the monitored list
  sort(unmonitored.begin(), unmonitored.end());
  bool ok;
  mmz->remmonitor(monitored[m].dev, monitored[m].op, ok);
  monitored.erase(monitored.begin()+m);

  ResetMonitors();
  Tell(_("Plot area Cleared"));
  canvas->Reset(0);
  return true;
}

bool MyMonManager::RunNetwork(int ncycles){
  /* GUI Back-end for running the network.
   * Checks the number of cycles completed and prompts the user if it
   * has been reached/exeded.
   * Derived from the corresponding function in userint.cc
   */
  bool ok = true;
  wxString m1, m2;
  if (*cyclescompletedp==maxcycles){
    Tell(_("Error: cycles limit (") +to_string(maxcycles)+_(") exceeded"));
    return false;
  }
  if ((*cyclescompletedp+ncycles)>=maxcycles){
    Tell(_("Error: cycles limit (") +to_string(maxcycles)+_(") reached"));
    ncycles = maxcycles - *cyclescompletedp;
  }
    int n = ncycles;
  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      Tell(_("Error: network is oscillating"));
  }
  if (ok) IncrementCycles(ncycles);
  else {ResetMonitors(); Tell(_("Plot area cleared")); canvas->Reset(0);}
  return ok;
}

void MyMonManager::ResetMonitors(){
  /* Resets the monitor recordings, called by several functions within this class.
   * Reseting is rewquired every time the monitor list is changed, because 
   * the monitor class does not support continuing recordings after the 
   * list of monitors is changed.
   */
  *cyclescompletedp = 0;
  mmz->resetmonitor();
  const wxString textstr = "0";
  // Refresh display of cycles completed count.
  montextctrl->Clear();
  montextctrl->AppendText(textstr);

}

void MyMonManager::IncrementCycles(int n){
  /* Handle all actions associeated with incrementing cycle count.
   */
  // Print error in case some changes in code resulted in negative increment
  if (n<0) Tell(_("Cycles can only be incremented by a positive amount"));
  
  // Update cycle count
  *cyclescompletedp += n;
  
  // Update "Cycles completed" textvox
  const wxString textstr = to_string(*cyclescompletedp);
  montextctrl->Clear();
  montextctrl->AppendText(textstr);

}

void MyMonManager::FlickSwitch(int n){
  /* Reverse the state of the given switch (switch at the given index
   * of the switches vector.
   * This function is used in SetSwitcList on MyFrame.
   */
  switches[n].check=!switches[n].check;
  asignal level;
  if(switches[n].check)level=high;
  else level=low;
  bool ok;
  dmz->setswitch( switches[n].dev, level, ok);
}

void MyMonManager::Tell(wxString message){
  /* Print a message on the command window.
   * This function is used for most messages that are translated, because
   * it works better non-ASCII characters than cout.
   * Same as the Tell function on the frame
   */
  cmddisp->AppendText(message+"\n");
}



////////////////////////////////////////////////////////////////////////
// MYMONDIALOG class that manages the monitors.
// Created for proper event handling in the monitor management dialog box.
//
BEGIN_EVENT_TABLE(MyMonDialog, wxDialog)
  EVT_BUTTON(MY_REMMON_ID, MyMonDialog::OnBtnRemMon)
  EVT_BUTTON(MY_ADDMON_ID, MyMonDialog::OnBtnAddMon)
END_EVENT_TABLE()
MyMonDialog::MyMonDialog(wxWindow *parent, wxWindowID id, const wxString &title,MyMonManager *mon_man,
          const wxPoint &pos, const wxSize &size,
          long style, const wxString &name):wxDialog(parent, id, title, pos, size, style, name){
/* MyMonDialog constructor.
 * Sets up all the items within the dialog box, calling MyMonManager monman
 * to acquire the lists.
 */
  monman=mon_man;

  int vsp = 3;
  // Set opListBox and monListBox
  const wxSize mylistsize = *(new wxSize(180, 250));
  
  optListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize);
  monListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize);
  RefreshLists();

  // Set up the sizers for neat display
  wxBoxSizer *tsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *listsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *optsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *monsizer = new wxBoxSizer(wxVERTICAL);

  optsizer->Add(optListBox, MyTabFlag);
  optsizer->AddSpacer(vsp);
  monsizer->Add(monListBox, MyTabFlag);
  monsizer->AddSpacer(vsp);

  optsizer->Add(new wxButton(this, MY_ADDMON_ID, _("Add"), wxDefaultPosition, ContSize), MyStdFlag);
  monsizer->Add(new wxButton(this, MY_REMMON_ID, _("Remove"), wxDefaultPosition, ContSize), MyStdFlag);

  listsizer->Add(optsizer, MyTabFlag);
  listsizer->Add(monsizer, MyTabFlag);
  listsizer->AddSpacer(10);
  tsizer->Add(listsizer);
  tsizer->Add(new wxButton(this, wxID_OK, "OK", wxDefaultPosition, ContSize),0, wxALIGN_CENTER);

  SetSizer(tsizer);

}

void MyMonDialog::RefreshLists(){
 /* Call MyMonManager monman to get the listd of monitored and unmonitored
  * outputs and fill in the list boxes
  */
  wxArrayString monlist = monman->GetMonitoredList();
  wxArrayString optlist = monman->GetUnmonitoredList();

  monListBox->Set(monlist);
  optListBox->Set(optlist);

}

// EVENTS
void MyMonDialog::OnBtnRemMon(wxCommandEvent &event){
  /* When "Remove" button is clicked, remove the selected output from
   * the monitored devices list.
   */
  int sel = monListBox->GetSelection();
  if(sel!=-1){
    monman->RemoveMonitor(sel);
    RefreshLists();
  }
}

void MyMonDialog::OnBtnAddMon(wxCommandEvent &event){
  /* When "Add" button is clicked, add the selected output to
   * the monitored devices list.
   */
  int sel = optListBox->GetSelection();
  if(sel!=-1){
    monman->AddMonitor(sel);
    RefreshLists();
  }
}

