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
  name dev, outp;
  string devstr, opstr;
  opProps temp;

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
//cout<<" Assembling monitored list "<<endl;
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
  sort(monitored.begin(), monitored.end());

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

wxArrayString MyMonManager::GetDevices(){
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
  wxArrayString monlist = *(new wxArrayString);
  for(int i=0; i<monitored.size(); i++)
  {
    monlist.Add(monitored[i].fullstr);
  }
  return monlist;
}

wxArrayString MyMonManager::GetUnmonitoredList(){
  wxArrayString unmonlist = *(new wxArrayString);
  for(int i=0; i<unmonitored.size(); i++)
  {
    unmonlist.Add(unmonitored[i].fullstr);
  }
  return unmonlist;

}

wxArrayString MyMonManager::GetSwitches(){
  wxArrayString swlist = wxArrayString();
  for(int i=0; i<switches.size(); i++)
  {
    swlist.Add(switches[i].devstr);
  }
  return swlist;
}

bool MyMonManager::AddMonitor(int m){
  if(m>unmonitored.size()|| m<0 ) return false;

  if(unmonitored.size()==maxmonitors){
    Tell(_("Monitor count limit (")+to_string(maxmonitors)+_(") exceeded"));
    return false;
  }

  monitored.push_back(unmonitored[m]);
  // Sort the monitored list
  sort(monitored.begin(), monitored.end());

  bool ok;
  mmz->makemonitor(unmonitored[m].dev, unmonitored[m].op, ok);
  unmonitored.erase(unmonitored.begin()+m);

  ResetMonitors();
  Tell(_("Plot area Cleared"));
  canvas->Reset();
  return true;
}

bool MyMonManager::RemoveMonitor(int m){
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
  // Function to run the network, derived from corresponding function in userint.cc

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
  // Reset monitors to work around monitor class inconsistencies
  *cyclescompletedp = 0;
  mmz->resetmonitor();
  const wxString textstr = "0";
  montextctrl->Clear();
  montextctrl->AppendText(textstr);

}

void MyMonManager::IncrementCycles(int n){
  if (n<0) Tell(_("Cycles can only be incremented by a positive amount"));

  *cyclescompletedp += n;

  const wxString textstr = to_string(*cyclescompletedp);
  montextctrl->Clear();
  montextctrl->AppendText(textstr);

}

void MyMonManager::FlickSwitch(int n){
  switches[n].check=!switches[n].check;
  asignal level;
  if(switches[n].check)level=high;
  else level=low;
  bool ok;
  dmz->setswitch( switches[n].dev, level, ok);
}

void MyMonManager::Tell(wxString message){
  //cmddisp->Newline();
  cmddisp->AppendText(message+"\n");
}



////////////////////////////////////////////////////////////////////////
// MYMONDIALOG class that manages the monitors. Needed to create for proper event handling.
BEGIN_EVENT_TABLE(MyMonDialog, wxDialog)
  EVT_BUTTON(MY_REMMON_ID, MyMonDialog::OnBtnRemMon)
  EVT_BUTTON(MY_ADDMON_ID, MyMonDialog::OnBtnAddMon)
END_EVENT_TABLE()
MyMonDialog::MyMonDialog(wxWindow *parent, wxWindowID id, const wxString &title,MyMonManager *mon_man,
          const wxPoint &pos, const wxSize &size,
          long style, const wxString &name):wxDialog(parent, id, title, pos, size, style, name){

  monman=mon_man;

  int vsp = 3;
  // Set opListBox and monListBox
  const wxSize mylistsize = *(new wxSize(180, 250));

  wxArrayString monlist = monman->GetMonitoredList();
  wxArrayString oplist = monman->GetUnmonitoredList();

  optListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize, oplist);
  monListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize, monlist);

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

  wxArrayString monlist = monman->GetMonitoredList();
  wxArrayString optlist = monman->GetUnmonitoredList();

  monListBox->Set(monlist);
  optListBox->Set(optlist);

}

// EVENTS
void MyMonDialog::OnBtnRemMon(wxCommandEvent &event){
  int sel = monListBox->GetSelection();
  //cout<<"Remove Clicked, selection "<<sel<<endl;
  if(sel!=-1){
    monman->RemoveMonitor(sel);
    RefreshLists();
  }
}

void MyMonDialog::OnBtnAddMon(wxCommandEvent &event){
  int sel = optListBox->GetSelection();
  //cout<<"Add Clicked, selection "<<sel<<endl;
  if(sel!=-1){
    monman->AddMonitor(sel);
    RefreshLists();
  }
}

