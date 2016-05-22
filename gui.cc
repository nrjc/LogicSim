#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
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

const wxSize RunSize = *(new wxSize(65, boxheight)); // Run button size
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
  disp_w = size.GetWidth();//NOT TESTED. for use of scrolling.
  zoom = 1.0;
  cyclesdisplayed = -1;
}

void MyGLCanvas::Render(wxString example_text, int cycles, bool spinchange){
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.

  float y, st_width = 30, st_height = 40, low_y = 10, high_y = low_y+st_height;
  float start_x = 30;
  float plt_height = high_y*2, curr_y;
  //string lowstr = "low", highstr = "high";
  unsigned int i, n=0;
  asignal s;
  
  int w, h;
  
  GetClientSize(&w, &h);
  
  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init ||spinchange) {
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
      
      glColor3f(0.0, 0.0, 1.0);
      curr_y = (-1.0)*(n+1)*plt_height;
      
      // Write out monitor name
      glRasterPos2f(start_x-15-pan_x, curr_y+high_y+st_height/2);
      for (i = 0; i < monname.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, monname[i]);
      // DRAW AXES
      DrawAxes(start_x, start_x+cyclesdisplayed*st_width, curr_y+low_y, curr_y+high_y);
      
      glColor3f(0.0, 0.7, 0.0);
      glLineWidth(2);
      glBegin(GL_LINE_STRIP);
      for (i=0; i<cyclesdisplayed; i++) {
        if (mmz->getsignaltrace(n, i, s)) {
          cout<<"monitor"<<n<<" cycle"<< i <<" trace"<< s <<endl;
          if (s==low) y = curr_y +low_y;
          if (s==high) y = curr_y+high_y;
          glVertex2f(start_x+st_width*(i), y); 
          glVertex2f(start_x+st_width*(i+1), y);
        }
      }
      glEnd();
      NameAxes(start_x, start_x+cyclesdisplayed*st_width, curr_y+low_y, curr_y+high_y);
    }
    
    disp_h = -curr_y+10;
    disp_w = start_x+cyclesdisplayed*st_width+50;
    // if disp_w or disp_h have values lesser than canvas dimensions,
    // they will be fixed with FixPan()
  
} else { cout<<"Error: cyclesdispalyed or mcount have unexpected values"<< endl;
  }
  FixPan();
  
  
  // Example of how to use GLUT to draw text on the canvas
  glColor3f(0.5, 0.0, 0.5);
  glRasterPos2f(100-pan_x, -pan_y+20);
  for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);

  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}

void MyGLCanvas::InitGL(){
  // Function to initialise the GL context

  int w, h;

  GetClientSize(&w, &h);
  if (pan_y<h) pan_y=h;
  if (pan_y>disp_h) pan_y=disp_h;// NOTE TO SELF: CHECK IF THIS ALWAYS WORKS. MIGHT CAUSE BUGS
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);
  //glClearColor(1.0, 1.0, 1.0, 0.0);
  glClearColor(0.98, 0.98, 0.98, 0.0);
  //glClearColor(0.0, 0.0, 0.0, 0.0);
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
  init = false;; // this will force the viewport and projection matrices to be reconfigured on the next paint
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
  //if (event.ButtonUp()) text.Printf("Mouse button %d released at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  if (event.Dragging()) {
    pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    FixPan();
    
    last_x = event.m_x;
    last_y = event.m_y;
    init = false;
    text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h-event.m_y, pan_x, pan_y);
  }
  //if (event.Leaving()) text.Printf("Mouse left window at %d %d", event.m_x, h-event.m_y);
  if (event.GetWheelRotation() < 0) {
    pan_y -= (int)10*event.GetWheelRotation()/(event.GetWheelDelta());
    if (pan_y<h) pan_y=h;
    if (pan_y>disp_h) pan_y=disp_h;
    //zoom = zoom * (1.0 - (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Negative mouse wheel rotation, pan_y now %d", pan_y);
  }
  if (event.GetWheelRotation() > 0) {
    pan_y -= (int)10*event.GetWheelRotation()/(event.GetWheelDelta());
    if (pan_y<h) pan_y=h;
    if (pan_y>disp_h) pan_y=disp_h;
    //zoom = zoom / (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Positive mouse wheel rotation, pan_y now %d", pan_y);
  }

  if (event.GetWheelRotation() || event.Dragging()) Render(text);
}

void MyGLCanvas::DrawAxes(float x_low, float x_high, float y_low, float y_high){
// Draw axes for the trace givent the plot dimmensions

  
  glLineWidth(1);// Set correct line width.
  glColor3f(0.8, 0.8, 0.8);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x_low, y_high);
  glVertex2f(x_high+10, y_high);
  glEnd();
  
  
  glColor3f(0.0, 0.0, 0.0);
  //glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x_low, y_high+5);
  glVertex2f(x_low, y_low-1);
  glVertex2f(x_high+10, y_low-1);
  glEnd();
  
}

void MyGLCanvas::NameAxes(float x_low, float x_high, float y_low, float y_high){
  // Put text labels on axes, given the plot dimensions
  string lowstr = "low", highstr = "high";
  
  glColor3f(0.0, 0.0, 0.0);
  glRasterPos2f(x_low-28-pan_x, y_high);
  for (int i = 0; i < highstr.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, highstr[i]);
  glRasterPos2f(x_low-28-pan_x, y_low-10);
  for (int i = 0; i < lowstr.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, lowstr[i]);
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
  if (pan_x>10) pan_x = 10;
  
}

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_BUTTON(MY_BUTTONRUN_ID, MyFrame::OnButtonRun)
  EVT_BUTTON(MY_BUTTONCONT_ID, MyFrame::OnButtonCont)
  EVT_BUTTON(MY_BUTTONSETMON_ID, MyFrame::OnButtonSetMon)  
  EVT_CHECKBOX(MY_SWITCH_ID, MyFrame::OnSwitchBox)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
  EVT_TEXT_ENTER(MY_TEXTCTRL_ID, MyFrame::OnText)
END_EVENT_TABLE()
  
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, network *network_mod, devices *devices_mod, monitor *monitor_mod, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style){
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers

  SetIcon(wxIcon(wx_icon));

  cyclescompleted = 0;
  nmz = names_mod;
  netz = network_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  if (nmz == NULL || netz==NULL || dmz == NULL || mmz == NULL) {
    cout << "Cannot operate GUI without names, devices and monitor classes" << endl;
    exit(1);
  }
  
  monman = new MyMonManager(nmz, netz, dmz, mmz);
  
  SetBackgroundColour(BcColour);
  SetForegroundColour(*wxWHITE);
  
  wxMenu *fileMenu = new wxMenu;
  //SetColours(fileMenu->Append(wxID_OPEN, "&Open"));
  wxMenuItem* openMenu = fileMenu->Append(wxID_OPEN, "&Open");
  //openMenu->SetBackgroundColour(BcColour);
  fileMenu->Append(wxID_ABOUT, "&About");
  fileMenu->Append(wxID_EXIT, "&Quit");
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  SetMenuBar(menuBar);
  
  wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);
  topsizer->AddSpacer(7);
  
  
  // To resize items: will probably need to create named ones or also specify the position
  // wxSize(int width, int height)
  wxBoxSizer *control_sizer = new wxBoxSizer(wxVERTICAL);
                             
  wxStaticBoxSizer *sim_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Simulation");
  // Sizer to horizontally align  Run and Continue buttons.
  wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
  button_sizer->Add(new wxButton(this, MY_BUTTONRUN_ID, "Run", wxDefaultPosition, RunSize), MyStdFlag);
  button_sizer->Add(new wxButton(this, MY_BUTTONCONT_ID, "Continue", wxDefaultPosition, ContSize), MyStdFlag);
  sim_sizer->Add(button_sizer);
  
  //wxBoxSizer *cycles_sizer = new wxBoxSizer(wxHORIZONTAL);
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, "Cycles"), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  spin = new wxSpinCtrl(this, MY_SPINCNTRL_ID, wxString("10"), wxDefaultPosition, wxDefaultSize);
  spin->SetForegroundColour(*wxBLACK);
  spin->SetRange(1, 2000);
  sim_sizer->Add(spin, MyStdFlag);
  //sim_sizer->Add(cycles_sizer);
  
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, "Text Commands"), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  sim_sizer->Add(new wxTextCtrl(this, MY_TEXTCTRL_ID, "", wxDefaultPosition, CommandSize, wxTE_PROCESS_ENTER), MyStdFlag);
  
  // Set up buttons dirrecting to item selection
  wxStaticBoxSizer *options_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Configure simulation");
  options_sizer->Add(new wxButton(this, MY_BUTTONSETMON_ID, "Add/remove monitors"), MyStdFlag);
  
  control_sizer->Add(sim_sizer);
  control_sizer->Add(options_sizer);
  
  // Add tabbed bontrol windows
  AddSwitchMonCtrl(control_sizer);
  
  // Create GL canvas and add
  const wxSize MyCanvasSize = wxSize(size.GetWidth()-200, size.GetHeight()-70);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, wxDefaultPosition, MyCanvasSize);
  
  topsizer->Add(control_sizer, 0, wxALIGN_CENTER);
  topsizer->Add(canvas, 1, wxEXPAND | wxALL, 10);
  SetSizeHints(MinWinSize);
  SetSizer(topsizer);
}

// EVENT HANDLERS //

void MyFrame::OnExit(wxCommandEvent &event){
  // Event handler for the exit menu item

  
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
  // Event handler for the about menu item
{
  wxMessageDialog about(this, "Example wxWidgets GUI\nAndrew Gee\nJune 2014", "About Logsim", wxICON_INFORMATION | wxOK);
  about.ShowModal();
}

void MyFrame::OnOpen(wxCommandEvent &event)
  // Event handler for the open menu item
{
    // Check out http://docs.wxwidgets.org/trunk/classwx_file_dialog.html
    // Create and Open file dialog
    /*  if (...current content has not been saved...)
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
                         wxICON_QUESTION | wxYES_NO, this) == wxNO )
            return;
        //else: proceed asking to the user the new file to open
    }*/
    
    wxFileDialog openFileDialog(this, _("Open logge file"), "", "",
                       "logge files (*.ge)|*.ge|text files (*.txt)|*.txt", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
    
    // proceed loading the file chosen by the user;
    // this can be done with e.g. wxWidgets input streams:
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }
    else 
    {
      // Non-functional testing
      canvas->Render("File opened");
      /* Functional wireframe. Only activated when the objects are
       * actually created, hence when logsim launches the GUI. */
      #ifdef USE_GUI
      /* NO POSIBILITY TO OPEN AT THE MOMENT, NEED TO CHANGE THE
       * INITIALISATION AND LOGSIM TO PASS THE PARSER AND SCANNER OBJECT
       * POINTERS OR COMPLETELY PASS THE CONTROL TO GUI CLASS - EVICT
       *  COMMAND LINE FILE EXECUTION */
      #endif
      
    }
}

void MyFrame::OnButtonRun(wxCommandEvent &event){
  // Event handler for the push button

  int n, ncycles;

  cyclescompleted = 0;
  mmz->resetmonitor ();
  runnetwork(spin->GetValue());
  canvas->Render("Run button pressed", cyclescompleted);
}

void MyFrame::OnButtonCont(wxCommandEvent &event)
  // Event handler for the push button
{
  int n, ncycles;
  
  runnetwork(spin->GetValue());
  canvas->Render("Continue button pressed", cyclescompleted);
}

void MyFrame::OnButtonSetMon(wxCommandEvent &event){

  /*wxRect framerect = this->GetRect();
  int w = framerect.GetWidth()/2;
  int h = framerect.GetHeight()/2;
  printf("Width %d   Height %d", w, h);
  printf("  Top %d", framerect.GetY());
  cout<<endl;
  const wxPoint mon_pos = *(new wxPoint(w,h ));*/
  
  const wxSize mon_size = *(new wxSize(400, 400));
  MyMonDialog* mymon = new MyMonDialog(this, wxID_ANY,"Add or set Monitor", monman, wxDefaultPosition, mon_size);
  mymon->Centre();
  
  //my_mon->CreateButtonSizer(wxOK|wxCANCEL);
  mymon->ShowModal(); 
  
  
}


void MyFrame::OnSwitchBox(wxCommandEvent &event){
  // Event handler for the push button

  string textstring="Switch ";
  wxCheckBox* currswitch = (wxCheckBox*)event.GetEventObject();
  textstring += currswitch->GetLabelText();
  //string switchname =(string) currswitch->GetLabelText();
  if(currswitch->IsChecked()) 
    textstring+=" was checked";
  else textstring += " was unchecked"; 
    
  canvas->Render(textstring, cyclescompleted);
  
  // pseudocode for when the network is accessible
  #ifdef USE_GUI
  namestring switchname = (namestring)currswitch->GetLabelText();
  name sid = nmz->lookup(switchname);
  asignal level;
  bool ok;
  
  if(currswitch->IsChecked())
  level = high;
  else level=low;
  
  dmz->setswitch( sid, level, ok);
  
  
  #endif
  
}

void MyFrame::OnSpin(wxSpinEvent &event){
  // Event handler for the spin control

  /*
  wxString text;
  int pos = event.GetPosition();
  cout<<pos<<endl;
  text.Printf("New spinctrl value %d", pos);
  canvas->Render(text, pos, true);*/
}

void MyFrame::OnText(wxCommandEvent &event){
  // Event handler for the text entry field

  wxString text;

  text.Printf("New text entered %s", event.GetString().c_str());
  canvas->Render(text);
}

void MyFrame::runnetwork(int ncycles){
  // Function to run the network, derived from corresponding function in userint.cc

  bool ok = true;
  int n = ncycles;

  while ((n > 0) && ok) {
    dmz->executedevices (ok);
    if (ok) {
      n--;
      mmz->recordsignals ();
    } else
      cout << "Error: network is oscillating" << endl;
  }
  if (ok) cyclescompleted = cyclescompleted + ncycles;
  else cyclescompleted = 0;
}

// ADDED NON-INTERFACE FUNCTIONS //

void MyFrame::SetSwitchList(wxWindow *parent,wxSizer* sizer)
{
  vector<devlink> switches = dmz->GetSwitches();
  name nm;
  asignal currswstate;
  wxCheckBox* currbox;
  string nmstring;
  // Going backwards through the list, because the network adds new 
  //devices/switches at the beginning of the device list
  for (int i=switches.size()-1; i>-1; i--)
  {
    nm=switches[i]->id;
    currswstate = switches[i]->swstate;
    nmstring = (string) nmz->getnamefromtable(nm);
    currbox = new wxCheckBox(parent, MY_SWITCH_ID, nmstring);
    if (currswstate == high) currbox->SetValue(true);
    else if (currswstate == low) currbox->SetValue(false);
    else cout<< "Invalid switch state, switch id "<< nm << ", name "<< nmstring<< endl;
    cout<< "Switch id "<< nm << ", name "<< nmstring<< endl;
    sizer->Add(currbox, MySwitchFlag);
    
  }
  
  // for debugging
  /*devlink d = switches[0];
  for (int i = 0; i<2; i++)
  {
    cout<<" id"<<d->id<<" name "<<nmz->getnamefromtable(d->id)<<" op"<<d->olist->id<<endl;//<<" inp"<<d->ilist->id<<endl;
    d = d->next;
  }
  */
}

void MyFrame::AddSwitchMonCtrl(wxSizer *control_sizer)
{
  wxAuiNotebook *note_ctrl = new wxAuiNotebook(this, MY_NOTEBOOK_ID, wxDefaultPosition, wxDefaultSize,
                                                wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS);
  note_ctrl->SetMinSize(MinTabSize);
  note_ctrl->SetForegroundColour(*wxBLACK);
  
  wxScrolledWindow* switchwin = new wxScrolledWindow(note_ctrl, wxID_ANY,
                    wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL);
  switchwin->SetBackgroundColour(*wxWHITE);
  // Later replace this with a separate function that initialises the window and all its contents uppon loading a new circuit.
  wxBoxSizer *switch_sizer = new wxBoxSizer(wxVERTICAL);
  #ifndef USE_GUI
  for (int i = 0; i<15; i++)
  {
    string temp = "TestCheck";
    temp+= to_string(i+1);
    switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, temp), MySwitchFlag);
  }
  #else
  SetSwitchList(switchwin, switch_sizer);
  #endif
  wxScrolledWindow *monwin = new wxScrolledWindow(note_ctrl, wxID_ANY);
  monwin->SetBackgroundColour(*wxWHITE);
  wxBoxSizer *mon_sizer = new wxBoxSizer(wxVERTICAL);
  //mon_sizer->
  
  note_ctrl->AddPage(switchwin,"Switches");
  note_ctrl->AddPage(monwin, "Monitors");
  switchwin->SetSizer(switch_sizer);
  
  control_sizer->Add(note_ctrl, MyTabFlag);
  control_sizer->AddSpacer(5);
  
  //Set scrollong for Switch and Monitor windows.
  int w = MinTabSize.GetWidth();
  int h = MinTabSize.GetHeight();
  int scrollwidth = 5;
  int xstep=5, ystep=5;
  switchwin->SetVirtualSize(w-scrollwidth, h);
  switchwin->SetScrollRate(xstep, ystep);
  
  monwin->SetVirtualSize(w-scrollwidth, h-scrollwidth);
  monwin->SetScrollRate(xstep, ystep);
}

////////////////////////////////////////////////////////////////////////
// MYMONMANAGER: added class to make monitor point managing easier and more object-oriented
MyMonManager::MyMonManager(names *names_mod, network *network_mod, devices *devices_mod, monitor *monitor_mod)
{
  nmz = names_mod;
  netz = network_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  
  name dev, outp;
  string devstr, opstr;
  opProps temp;
  /*for(int i=40; i<45; i++)
  {
    cout<<nmz->getnamefromtable(i)<<endl;//////////////COUT//////////////
  }*/
  
  // find all outputs.
  devlink d = netz->devicelist();
  outplink o;
  cout<< "devices" <<endl;
  while( d != NULL){
    dev = d->id;
    devstr = nmz->getnamefromtable(dev);
    cout<<devstr<< " id "<< dev<<endl;//////////////COUT//////////////
    
    o = d->olist;
    while(o != NULL)
    {
      outp=o->id;
      if (outp==-1){
      temp = *(new opProps(dev, outp, devstr));
      // temp.opstr preset to ""
      allops.push_back(temp);
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
    //cout<<"Iteration "<< i<< endl;
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
  
  cout<<" Assembling unmonitored list "<<endl;
  // Assemble a list of unmonitored outputs. For each element of allops
  // add it to unmonitored, if it does not exist in monitored. Since the
  // number of monitors is more restricted than the number of devices,
  // this strategy may help exploit temporal locality of reference. And
  // speed up the process.
  for(int i=0; i<allops.size();i++){
    bool found=false;
    cout<<"Entered loop "<< allops[i].fullstr<<endl;
    for(int j=0; j<monitored.size(); j++){
      if (allops[i]==monitored[j]){
        found=true;
        cout<<"found monitor "<<allops[i].fullstr<<endl;
        break;
      }
      else{
        //cout<<"monitor not found "<< allops[i].fullstr<<" vs "<< monitored[i].fullstr<<endl;
      }
    }
    if(!found) unmonitored.push_back(allops[i]);
  }
  // Sort the unmonitored list
  sort(unmonitored.begin(), unmonitored.end());
  
  
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

bool MyMonManager::AddMonitor(int m){
  if(m>unmonitored.size()|| m<0) return false;
  monitored.push_back(unmonitored[m]);
  // Sort the monitored list
  sort(monitored.begin(), monitored.end());
  bool ok;
  mmz->makemonitor(unmonitored[m].dev, unmonitored[m].op, ok);
  unmonitored.erase(unmonitored.begin()+m);
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
  
  return true;
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
  const wxSize mylistsize = *(new wxSize(180, 300));
  
  wxArrayString monlist = monman->GetMonitoredList();
  wxArrayString oplist = monman->GetUnmonitoredList();
  
  optListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize, oplist);
  monListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition,  mylistsize, monlist);
  
  wxBoxSizer *tsizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer *optsizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *monsizer = new wxBoxSizer(wxVERTICAL);
  
  optsizer->Add(optListBox, MyTabFlag);
  optsizer->AddSpacer(vsp);
  monsizer->Add(monListBox, MyTabFlag);
  monsizer->AddSpacer(vsp);
  
  optsizer->Add(new wxButton(this, MY_ADDMON_ID, "Add", wxDefaultPosition, ContSize), MyStdFlag);
  monsizer->Add(new wxButton(this, MY_REMMON_ID, "Remove", wxDefaultPosition, ContSize), MyStdFlag);
  
  tsizer->Add(optsizer, MyTabFlag);
  tsizer->Add(monsizer, MyTabFlag);
  tsizer->AddSpacer(10);
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
  cout<<"Remove Clicked, selection "<<sel<<endl;
  if(sel!=-1){
    monman->RemoveMonitor(sel);
    RefreshLists();
  }
}

void MyMonDialog::OnBtnAddMon(wxCommandEvent &event){
  cout<<"Add Clicked"<<endl;
  int sel = optListBox->GetSelection();
  cout<<"Add Clicked, selection "<<sel<<endl;
  if(sel!=-1){
    monman->AddMonitor(sel);
    RefreshLists();
  }
}

/*
void MyFrame::SetColours(wxMenuItem* item)

{
  item->SetBackgroundColour(BcColour);
  item->SetForegroundColour(*wxWHITE);
}*/

