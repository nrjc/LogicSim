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

// MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()

int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

const int boxheight = 25;// Standard textbox/button height
const wxColour BcColour(0x11350E0E);// background color


const wxSize MinTabSize = *(new wxSize(160, 2000));// Min size for tabs
const wxSize MinWinSize = *(new wxSize(400, 500));

const wxSize RunSize = wxSize(65, boxheight); // Run button size
const wxSize ContSize = *(new wxSize(100, boxheight));//Continue button size
const wxSize CyclesSize = *(new wxSize(160, boxheight));// Cycles textbox size
const wxSize CommandSize = *(new wxSize(160, boxheight));// Command textbox size

const wxSizerFlags MySwitchFlag = wxSizerFlags().Expand().Border(wxALL, 5);
const wxSizerFlags MyStdFlag = wxSizerFlags().Expand().Border(wxALL, 5); // Same as 0, wxALL, 5
const wxSizerFlags MyTabFlag = wxSizerFlags().Proportion(1).Expand().Border(wxALL, 5);// Same as 1, wxALL, 5

MyGLCanvas::MyGLCanvas(wxWindow *parent, wxWindowID id, monitor* monitor_mod, names* names_mod, const wxPoint& pos,
		       const wxSize& size, long style, const wxString& name, const wxPalette& palette):
  wxGLCanvas(parent, id, wxglcanvas_attrib_list, pos, size, style, name, palette){
  // Constructor - initialises private variables

  context = new wxGLContext(this);
  mmz = monitor_mod;
  nmz = names_mod;
  init = false;
  pan_x = 0;
  pan_y = size.GetHeight();
  disp_h = pan_y;
  disp_w = size.GetWidth();
  zoom = 1.0;
  cyclesdisplayed = -1;
}

void MyGLCanvas::Render(wxString example_text, int cycles, bool spinchange){
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.

  float y, st_width = 30, st_height = 30, low_y = 10, high_y = low_y+st_height;
  static float start_x;
  if (pan_x>-50/zoom) start_x=50/zoom;
  float plt_height = high_y*2, curr_y;

  unsigned int i, n=0;
  asignal s;

  int w, h;

  GetClientSize(&w, &h);

  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init ) {
    InitGL();
    init = true;
  }
  glClear(GL_COLOR_BUFFER_BIT);

  int mcount = mmz->moncount();

  if ((cyclesdisplayed >= 0) && (mcount > 0)) { // draw the first monitor signal, get trace from monitor class

    string monname;
    name dev, outp;

    for (n=0; n<mcount; n++){
      // retrieve monitor name
      mmz->getmonname(n, dev, outp);
      monname = (string) nmz->getnamefromtable(dev);
      if (outp!=-1) {monname+="."; monname+=(string) nmz->getnamefromtable(outp);}

      curr_y = (-1.0)*(n+1)*plt_height;

      // DRAW AXES
      DrawAxes(start_x,  st_width,cyclesdisplayed, curr_y+low_y, curr_y+high_y);

      glColor3f(0.0, 0.7, 0.0);
      glLineWidth(2);
      glBegin(GL_LINE_STRIP);
      for (i=0; i<cyclesdisplayed; i++) {
        if (mmz->getsignaltrace(n, i, s)) {

          if (s==low) y = curr_y +low_y;
          if (s==high) y = curr_y+high_y;
          glVertex2f(start_x+st_width*(i), y);
          glVertex2f(start_x+st_width*(i+1), y);
        }
      }
      glEnd();
      // Draw all text associated with the axes
      NameAxes(start_x, st_width,cyclesdisplayed, curr_y+low_y, st_height, monname);
    }

    disp_h = (-curr_y+10)*zoom;
    disp_w = (start_x+cyclesdisplayed*st_width+50)*zoom;
    // if disp_w or disp_h have values lesser than canvas dimensions,
    // they will be fixed with FixPan()
    }
  FixPan();

  // Example of how to use GLUT to draw text on the canvas
  /* disable canvas messages
  glColor3f(0.5, 0.0, 0.5);
  glRasterPos2f((100-pan_x)/zoom, (-pan_y+20)/zoom);
  for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);
  */
  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}

void MyGLCanvas::InitGL(){
  // Function to initialise the GL context

  int w, h;

  GetClientSize(&w, &h);
  if (pan_y<h) pan_y=h;
  if (pan_y>disp_h) pan_y=disp_h;
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);

  glClearColor(0.98, 0.98, 0.98, 0.0);

  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(pan_x, pan_y, 0.0);
  glScaled(zoom, zoom, zoom);

}

void MyGLCanvas::OnPaint(wxPaintEvent& event){
  // Event handler for when the canvas is exposed

  int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  text.Printf("Canvas redrawn by OnPaint event handler, canvas size is %d by %d", w, h);
  Render(text);
}

void MyGLCanvas::OnSize(wxSizeEvent& event){
  // Event handler for when the canvas is resized
  FixPan();
  init = false; // this will force the viewport and projection matrices to be reconfigured on the next paint
  //Render("test");
}

void MyGLCanvas::OnMouse(wxMouseEvent& event){
  // Event handler for mouse events inside the GL canvas

  wxString text;
  int w, h;
  static int last_x, last_y;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) {
    // Required for dragging to work correctly.
    last_x = event.m_x;
    last_y = event.m_y;
    text.Printf("Mouse button %d pressed at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  }

  if (event.Dragging()) {
    pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    FixPan();

    last_x = event.m_x;
    last_y = event.m_y;
    init = false;
    text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h-event.m_y, pan_x, pan_y);
  }

  if (event.GetWheelRotation() < 0) {
    /*pan_y -= (int)10*event.GetWheelRotation()/(event.GetWheelDelta());
    if (pan_y<h) pan_y=h;
    if (pan_y>disp_h) pan_y=disp_h;
    */
    zoom = zoom / (1.0 - (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    FixZoom();
    init = false;
    text.Printf("Negative mouse wheel rotation, zoom now %f", zoom);
  }
  if (event.GetWheelRotation() > 0) {
    /*pan_y -= (int)10*event.GetWheelRotation()/(event.GetWheelDelta());
    if (pan_y<h) pan_y=h;
    if (pan_y>disp_h) pan_y=disp_h;
    */
    zoom = zoom * (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    FixZoom();
    init = false;
    text.Printf("Positive mouse wheel rotation, zoom now %f", zoom);
  }

  if (event.GetWheelRotation() || event.Dragging()||event.ButtonDown()) Render(text);
}

void MyGLCanvas::DrawAxes(float x_low, float x_spacing, int cycles, float y_low, float y_high){
// Draw axes for the trace givent the plot dimmensions


  glLineWidth(1);// Set correct line width.
  glColor3f(0.8, 0.8, 0.8);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x_low, y_high);
  glVertex2f(x_low+x_spacing*cycles+10, y_high);
  glEnd();

  // Draw vertical lines to mark cycle nyumbers
  for (int i=1; i<cycles+1; i++)
  {
    glBegin(GL_LINE_STRIP);
    glVertex2f(x_low+x_spacing*i, y_high);
    glVertex2f(x_low+x_spacing*i, y_low-1);
    glEnd();
  }

  glColor3f(0.0, 0.0, 0.0);
  //glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x_low, y_high+5);
  glVertex2f(x_low, y_low-1);
  glVertex2f(x_low+x_spacing*cycles+10, y_low-1);
  glEnd();

}

void MyGLCanvas::NameAxes(float x_low, float x_spacing, int cycles, float y_low, float st_height, string monname){
  // Put text labels on axes, given the plot dimensions
  string lowstr = "low", highstr = "high";
  string number;
  // Print monitor name
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f((30-pan_x)/zoom, y_low+1.5*st_height);
  for (int i = 0; i < monname.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, monname[i]);

  // Axes text color
  glColor3f(0.0, 0.0, 0.0);
  // Print y values
  glRasterPos2f((20-pan_x)/zoom, y_low+st_height);
  for (int i = 0; i < highstr.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, highstr[i]);
  glRasterPos2f((20-pan_x)/zoom, y_low-5/zoom);
  for (int i = 0; i < lowstr.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, lowstr[i]);

  // Print cycle numbers (x values)
  for (int n = 0; n<=cycles; n++)
  {
    number = to_string(n);
    glRasterPos2f(x_low+x_spacing*n-3/zoom, y_low-15/zoom);
  for (int i = 0; i < number.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, number[i]);


  }

}

void MyGLCanvas::FixPan(){
  // Fixes the pan after a mouse/size event or after drawing.
  int w, h;
  GetClientSize(&w, &h);
  if (disp_h<h) disp_h=h;
  if (disp_w<w) disp_w=w;
  if (pan_y<h) pan_y=h;
  if (pan_y>disp_h) pan_y=disp_h;
  if (pan_x<w-disp_w) pan_x = w-disp_w;
  if (pan_x>0) pan_x = 0;

}

void MyGLCanvas::FixZoom(){
  // Fixes the pan after a mouse/size event or after drawing.
  static double prevzoom=1;
  int w, h;
  if (zoom<minzoom) zoom = minzoom;
  if (zoom>maxzoom) zoom = maxzoom;
  pan_y=pan_y*zoom/prevzoom;
  pan_x=pan_x*zoom/prevzoom;
  prevzoom=zoom;
}

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
    cout<<"Cannot run without names, network, devices and monitor classes"<<endl;
    exit(1);
  }
  // SUPPRESS CIRCUIT EXECUTION MESSAGES
  dmz->debug(false);

  monctrl = new wxTextCtrl(this, MY_TEXTCTRL_ID, "0", wxDefaultPosition, CommandSize, wxTE_READONLY);
  const wxSize MyCmdSize = wxSize(size.GetWidth()-200, 75);
  //cmddisp = new wxTextCtrl();
  cmddisp = new wxTextCtrl(this, -1, wxString(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY|wxTE_BESTWRAP);
  cmdOutputRedirect = new wxStreamToTextRedirector(cmddisp);

  const wxSize MyMinCmdSize = wxSize(20, 75);
  cmddisp->SetMinSize(MyMinCmdSize);

  const wxSize MyCanvasSize = wxSize(size.GetWidth()-200, size.GetHeight()-150);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, wxDefaultPosition, MyCanvasSize);

  monman = new MyMonManager(nmz, netz, dmz, mmz, &cyclescompleted, monctrl, cmddisp, canvas);

  // Set up colors
  SetBackgroundColour(BcColour);
  SetForegroundColour(*wxWHITE);
  // Set up file menu
  wxMenu *fileMenu = new wxMenu;
  wxMenuItem* openMenu = fileMenu->Append(wxID_OPEN, "&Open");
  fileMenu->Append(wxID_ABOUT, "&About");
  fileMenu->Append(wxID_EXIT, "&Quit");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  SetMenuBar(menuBar);

  // Set up main controla
  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->AddSpacer(7);


  // Create control panel
  wxBoxSizer *control_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticBoxSizer *sim_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Simulation");
  // Sizer to horizontally align  Run and Continue buttons.
  wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
  button_sizer->Add(new wxButton(this, MY_BUTTONRUN_ID, "Run", wxDefaultPosition, RunSize), MyStdFlag);
  button_sizer->Add(new wxButton(this, MY_BUTTONCONT_ID, "Continue", wxDefaultPosition, ContSize), MyStdFlag);
  sim_sizer->Add(button_sizer);

  sim_sizer->Add(new wxStaticText(this, wxID_ANY, "Cycles"), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"), wxDefaultPosition, wxDefaultSize);
  spin->SetForegroundColour(*wxBLACK);
  spin->SetRange(1, 500);
  sim_sizer->Add(spin, MyStdFlag);

  // Text command box to display number of cycles completed
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, "Cycles completed"), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  sim_sizer->Add(monctrl, MyStdFlag);

  // Set up buttons dirrecting to item selection
  wxStaticBoxSizer *options_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Configure simulation");
  options_sizer->Add(new wxButton(this, MY_BUTTONSETMON_ID, "Add/remove monitors"), MyStdFlag);

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
  monman->Reset();
  canvas->Render("Canvas Cleared", 0);
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

  wxMessageDialog about(this, "Software project 2016 team 6:\n Names and Scanner classes:\n\t Nickolas Capel.\n Parser class: \n\t Yu Xiang Lou,\n\t Nicholas Capel.\n GUI: \n\t Kamile Rastene.", "About Logsim", wxICON_INFORMATION | wxOK);
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
  monman->RunNetwork(spin->GetValue());
  canvas->Render("Run button pressed", cyclescompleted);
}

void MyFrame::OnButtonCont(wxCommandEvent &event){
  // Event handler for the push button
  if (monman->RunNetwork(spin->GetValue()))
    canvas->Render("Continue button pressed", cyclescompleted);
}

void MyFrame::OnButtonSetMon(wxCommandEvent &event){
  const wxSize mon_size = *(new wxSize(400, 400));
  MyMonDialog* mymon = new MyMonDialog(this, wxID_ANY,"Add or set Monitor", monman, wxDefaultPosition, mon_size);
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
        cout<<"Network built"<<endl;
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

  note_ctrl->AddPage(switchwin,"Switches");
  note_ctrl->AddPage(devwin, "Devices");

  control_sizer->Add(note_ctrl, MyTabFlag);
  control_sizer->AddSpacer(5);


}

void MyFrame::Tell(string message){
  //cmddisp->Newline();
  cmddisp->AppendText(message+"\n");
}

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
    //cout<<devstr<< " id "<< dev<<endl;//////////////COUT//////////////

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
  // Sort the allops list
  sort(allops.begin(), allops.end());

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
    //cout<<"Entered loop "<< allops[i].fullstr<<endl;
    for(int j=0; j<monitored.size(); j++){
      if (allops[i]==monitored[j]){
        found=true;
        //Tell("found monitor "+allops[i].fullstr);
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
    cout<<"Monitor count limit ("<<maxmonitors<<") exceeded"<< endl;
    return false;
  }

  monitored.push_back(unmonitored[m]);
  // Sort the monitored list
  sort(monitored.begin(), monitored.end());

  bool ok;
  mmz->makemonitor(unmonitored[m].dev, unmonitored[m].op, ok);
  unmonitored.erase(unmonitored.begin()+m);

  ResetMonitors();
  Tell("Canvas Cleared");
  canvas->Render("", 0);
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
  Tell("Canvas Cleared");
  canvas->Render("", 0);
  return true;
}

bool MyMonManager::RunNetwork(int ncycles){
  // Function to run the network, derived from corresponding function in userint.cc

  bool ok = true;
  if (*cyclescompletedp==maxcycles){
    Tell("Error: cycles limit ("+to_string(maxcycles)+") exceeded");
    return false;
  }
  if ((*cyclescompletedp+ncycles)>=maxcycles){

    Tell("Cycles limit ("+to_string(maxcycles)+") reached");
    ncycles = maxcycles - *cyclescompletedp;
  }
    int n = ncycles;
  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      Tell("Error: network is oscillating");
  }
  if (ok) IncrementCycles(ncycles);
  else {ResetMonitors(); canvas->Render("Canvas Cleared", 0);}
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
  if (n<0) Tell("Cycles can only be incremented by a positive amount");

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

void MyMonManager::Tell(string message){
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

  optsizer->Add(new wxButton(this, MY_ADDMON_ID, "Add", wxDefaultPosition, ContSize), MyStdFlag);
  monsizer->Add(new wxButton(this, MY_REMMON_ID, "Remove", wxDefaultPosition, ContSize), MyStdFlag);

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
