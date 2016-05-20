#include "gui.h"
#include <GL/glut.h>
#include "wx_icon.xpm"
#include <iostream>
#include <string>
//wxSize
#include <wx/gdicmn.h>
#include <wx/spinctrl.h>
// tabs
#include <wx/notebook.h>
//#include <wx/menuitem.h>
#include <wx/string.h>

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
  wxGLCanvas(parent, id, wxglcanvas_attrib_list, pos, size, style, name, palette)
  // Constructor - initialises private variables
{
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

void MyGLCanvas::Render(wxString example_text, int cycles)
  // Draws canvas contents - the following example writes the string "example text" onto the canvas
  // and draws a signal trace. The trace is artificial if the simulator has not yet been run.
  // When the simulator is run, the number of cycles is passed as a parameter and the first monitor
  // trace is displayed.
{
  float y, st_width = 20, plt_height = 60, curr_y;
  unsigned int i, n=0;
  asignal s;

  if (cycles >= 0) cyclesdisplayed = cycles;

  SetCurrent(*context);
  if (!init) {
    InitGL();
    init = true;
  }
  glClear(GL_COLOR_BUFFER_BIT);
  
  int mcount = mmz->moncount();
  
  if ((cyclesdisplayed >= 0) && (mcount > 0)) { // draw the first monitor signal, get trace from monitor class
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for (i=0; i<cyclesdisplayed; i++) {
      if (mmz->getsignaltrace(n, i, s)) {
        if (s==low) y = n*plt_height+10.0;
        if (s==high) y = n*plt_height+30.0;
        glVertex2f(st_width*i+10.0, y); 
        glVertex2f(st_width*i+30.0, y);
      }
    }
    glEnd();
    // untested alternative to draw multiple traces
    /*
    for (n=0; n<mcount; n++)
      glBegin(GL_LINE_STRIP);
      {for (i=0; i<cyclesdisplayed; i++) {
        if (mmz->getsignaltrace(n, i, s)) {
          if (s==low) y = (mcount-n-1)*plt_height+10.0;
          if (s==high) y = (mcount-n-1)*plt_height+30.0;
          glVertex2f(st_width*i+10.0, y); 
          glVertex2f(st_width*i+30.0, y);
        }
      }
      glEnd();
    }*/
    
    

  } else { // draw an artificial trace
    
    mcount = 20;
    
    string monname;
    for (n=0; n<mcount; n++){
      glColor3f(0.0, 0.0, 1.0);
      monname = "Example ";
      monname+= to_string(n+1);
      curr_y = (-1.0)*(n+1)*plt_height;
      glRasterPos2f(10, curr_y+40);
      for (i = 0; i < monname.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, monname[i]);
      
      glColor3f(0.0, 1.0, 0.0);
      glBegin(GL_LINE_STRIP);
      for (i=0; i<5; i++) {
        if (i%2) y = curr_y +10.0;
        else y = curr_y +30.0;
        glVertex2f(st_width*i+10.0, y); 
        glVertex2f(st_width*i+30.0, y);
      }
      glEnd();
      disp_h = -curr_y+10;
    }
    
  }

  // Example of how to use GLUT to draw text on the canvas
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f(50, -100);
  for (i = 0; i < example_text.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, example_text[i]);

  // We've been drawing to the back buffer, flush the graphics pipeline and swap the back buffer to the front
  glFlush();
  SwapBuffers();
}

void MyGLCanvas::InitGL()
  // Function to initialise the GL context
{
  int w, h;

  GetClientSize(&w, &h);
  if (pan_y<h) pan_y=h;
  if (pan_y>disp_h) pan_y=disp_h;// NOTE TO SELF: CHECK IF THIS ALWAYS WORKS. MIGHT CAUSE BUGS
  SetCurrent(*context);
  glDrawBuffer(GL_BACK);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1); 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(pan_x, pan_y, 0.0);
  glScaled(zoom, zoom, zoom);
  
}

void MyGLCanvas::OnPaint(wxPaintEvent& event)
  // Event handler for when the canvas is exposed
{
  int w, h;
  wxString text;

  wxPaintDC dc(this); // required for correct refreshing under MS windows
  GetClientSize(&w, &h);
  text.Printf("Canvas redrawn by OnPaint event handler, canvas size is %d by %d", w, h);
  Render(text);
}

void MyGLCanvas::OnSize(wxSizeEvent& event)
  // Event handler for when the canvas is resized
{
  init = false;; // this will force the viewport and projection matrices to be reconfigured on the next paint
}

void MyGLCanvas::OnMouse(wxMouseEvent& event)
  // Event handler for mouse events inside the GL canvas
{
  wxString text;
  int w, h;;
  static int last_x, last_y;

  GetClientSize(&w, &h);
  if (event.ButtonDown()) {
    last_x = event.m_x;
    last_y = event.m_y;
    text.Printf("Mouse button %d pressed at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  }
  if (event.ButtonUp()) text.Printf("Mouse button %d released at %d %d", event.GetButton(), event.m_x, h-event.m_y);
  if (event.Dragging()) {
    //pan_x += event.m_x - last_x;
    pan_y -= event.m_y - last_y;
    if (pan_y<h) pan_y=h;
    if (pan_y>disp_h) pan_y=disp_h;
    //last_x = event.m_x;
    last_y = event.m_y;
    init = false;
    text.Printf("Mouse dragged to %d %d, pan now %d %d", event.m_x, h-event.m_y, pan_x, pan_y);
  }
  if (event.Leaving()) text.Printf("Mouse left window at %d %d", event.m_x, h-event.m_y);
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
    //TODO: GET PLOT HEIGHT AND RESTRICT SCROLL ACCORDINGLY
    //zoom = zoom / (1.0 + (double)event.GetWheelRotation()/(20*event.GetWheelDelta()));
    init = false;
    text.Printf("Positive mouse wheel rotation, pan_y now %d", pan_y);
  }

  if (event.GetWheelRotation() || event.ButtonDown() || event.ButtonUp() || event.Dragging() || event.Leaving()) Render(text);
}

// MyFrame ///////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
  EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
  EVT_BUTTON(MY_BUTTONRUN_ID, MyFrame::OnButtonRun)
  EVT_BUTTON(MY_BUTTONCONT_ID, MyFrame::OnButtonCont)
  EVT_CHECKBOX(MY_SWITCH_ID, MyFrame::OnSwitchBox)
  EVT_SPINCTRL(MY_SPINCNTRL_ID, MyFrame::OnSpin)
  EVT_TEXT_ENTER(MY_TEXTCTRL_ID, MyFrame::OnText)
END_EVENT_TABLE()
  
MyFrame::MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size,
		 names *names_mod, devices *devices_mod, monitor *monitor_mod, long style):
  wxFrame(parent, wxID_ANY, title, pos, size, style)
  // Constructor - initialises pointers to names, devices and monitor classes, lays out widgets
  // using sizers
{
  SetIcon(wxIcon(wx_icon));

  cyclescompleted = 0;
  nmz = names_mod;
  dmz = devices_mod;
  mmz = monitor_mod;
  if (nmz == NULL || dmz == NULL || mmz == NULL) {
    cout << "Cannot operate GUI without names, devices and monitor classes" << endl;
    exit(1);
  }
  
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
  sim_sizer->Add(spin, MyStdFlag);
  //sim_sizer->Add(cycles_sizer);
  
  sim_sizer->Add(new wxStaticText(this, wxID_ANY, "Text Commands"), 0, wxTOP|wxLEFT|wxRIGHT, 5);
  sim_sizer->Add(new wxTextCtrl(this, MY_TEXTCTRL_ID, "", wxDefaultPosition, CommandSize, wxTE_PROCESS_ENTER), MyStdFlag);
  
  // Set up buttons dirrecting to item selection
  wxStaticBoxSizer *options_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Configure simulation");
  options_sizer->Add(new wxButton(this, MY_BUTTONSETMON_ID, "Add/remove monitors"), MyStdFlag);
  
  // TABBED CONTROL WINDOWS //
  wxNotebook *note_ctrl = new wxNotebook(this, MY_NOTEBOOK_ID);
  note_ctrl->SetMinSize(MinTabSize);
  note_ctrl->SetForegroundColour(*wxBLACK);
  
  wxScrolledWindow* switchwin = new wxScrolledWindow(note_ctrl, wxID_ANY,
                    wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL);
  switchwin->SetBackgroundColour(*wxWHITE);
  // Later replace this with a separate function that initialises the window and all its contents uppon loading a new circuit.
  wxBoxSizer *switch_sizer = new wxBoxSizer(wxVERTICAL);
  #ifndef USE_GUI
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck1"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck2"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck3"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck4"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck5"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck6"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck7"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck8"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck9"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck10"), MySwitchFlag);
  switch_sizer->Add(new wxCheckBox(switchwin, MY_SWITCH_ID, "TestCheck11"), MySwitchFlag);
  #else
  SetSwitchList(switchwin, switch_sizer);
  #endif
  wxScrolledWindow *monwin = new wxScrolledWindow(note_ctrl, wxID_ANY);
  monwin->SetBackgroundColour(*wxWHITE);
  wxBoxSizer *mon_sizer = new wxBoxSizer(wxVERTICAL);
  //mon_sizer->
  
  note_ctrl->AddPage(switchwin,"Switch");
  note_ctrl->AddPage(monwin, "Monitors");
  switchwin->SetSizer(switch_sizer);
  
  control_sizer->Add(sim_sizer);
  control_sizer->Add(options_sizer);
  control_sizer->Add(note_ctrl, MyTabFlag);
  control_sizer->AddSpacer(5);
  
  // Create GL canvas and add
  const wxSize MyCanvasSize = wxSize(size.GetWidth()-200, size.GetHeight()-70);
  canvas = new MyGLCanvas(this, wxID_ANY, monitor_mod, names_mod, wxDefaultPosition, MyCanvasSize);
  
  topsizer->Add(control_sizer, 0, wxALIGN_CENTER);
  topsizer->Add(canvas, 1, wxEXPAND | wxALL, 10);
  SetSizeHints(MinWinSize);
  SetSizer(topsizer);
  
  //Set scrollong for Switch and Monitor windows.
  int w = MinTabSize.GetWidth();
  int h = MinTabSize.GetHeight();
  int scrollwidth;
  int xstep=5, ystep=5;
  switchwin->SetVirtualSize(w-scrollwidth, h);
  switchwin->SetScrollRate(xstep, ystep);
  
  monwin->SetVirtualSize(w-scrollwidth, h-scrollwidth);
  monwin->SetScrollRate(xstep, ystep);
}

// EVENT HANDLERS //

void MyFrame::OnExit(wxCommandEvent &event)
  // Event handler for the exit menu item
{
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

void MyFrame::OnButtonRun(wxCommandEvent &event)
  // Event handler for the push button
{
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

void MyFrame::OnSwitchBox(wxCommandEvent &event)
  // Event handler for the push button
{
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
  name sid = names_mod->lookup(switchname);
  asignal level;
  bool ok;
  
  if(currswitch->IsChecked())
  level = high;
  else level=low;
  
  devices_mod->setswitch( sid, level, ok);
  
  
  #endif
  
}

void MyFrame::OnSpin(wxSpinEvent &event)
  // Event handler for the spin control
{
  wxString text;

  text.Printf("New spinctrl value %d", event.GetPosition());
  canvas->Render(text);
}

void MyFrame::OnText(wxCommandEvent &event)
  // Event handler for the text entry field
{
  wxString text;

  text.Printf("New text entered %s", event.GetString().c_str());
  canvas->Render(text);
}

void MyFrame::runnetwork(int ncycles)
  // Function to run the network, derived from corresponding function in userint.cc
{
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

// ADDED FUNCTIONS //

void MyFrame::SetSwitchList(wxWindow *parent,wxSizer* sizer)
{
  vector<name> switches = dmz->GetSwitches();
  name nm;
  for (int i=0; i<switches.size(); i++)
  {
    nm=switches[i];
    sizer->Add(new wxCheckBox(parent, MY_SWITCH_ID, nmz->getnamefromtable(nm)), MySwitchFlag);
    
  }
  
}

/*
void MyFrame::SetColours(wxMenuItem* item)

{
  item->SetBackgroundColour(BcColour);
  item->SetForegroundColour(*wxWHITE);
}*/

