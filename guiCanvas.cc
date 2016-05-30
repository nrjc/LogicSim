#include "guiCanvas.h"

using namespace std;

// MyGLCanvas ////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
  EVT_SIZE(MyGLCanvas::OnSize)
  EVT_PAINT(MyGLCanvas::OnPaint)
  EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
END_EVENT_TABLE()

int wxglcanvas_attrib_list[5] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};

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

  wxString text="";
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

  if (event.GetWheelRotation() || event.Dragging()||event.ButtonDown()||event.Entering()||event.Leaving()) Render(text);
}

void MyGLCanvas::DrawAxes(float x_low, float x_spacing, int cycles, float y_low, float y_high){
// Draw axes for the trace givent the plot dimmensions


  glLineWidth(1);// Set correct line width.
  glColor3f(0.9, 0.9, 0.9);
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
  wxString lowstr = _("low"), highstr = _("high");
  string number;
  // Print monitor name
  glColor3f(0.0, 0.0, 1.0);
  glRasterPos2f((30-pan_x)/zoom, y_low+1.5*st_height);
  for (int i = 0; i < monname.length(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, monname[i]);

  // Axes text color
  glColor3f(0.0, 0.0, 0.0);
  // Print y values
  glRasterPos2f((20-pan_x)/zoom, y_low+st_height+2/zoom);
  for (int i = 0; i < highstr.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, highstr[i]);
  glRasterPos2f((20-pan_x)/zoom, y_low+1/zoom);
  for (int i = 0; i < lowstr.Len(); i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, lowstr[i]);

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
  if (pan_y>disp_h) pan_y=disp_h;
  if (pan_y<h) pan_y=h;
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

void MyGLCanvas::Reset(int cycles){
  pan_y=0;
  pan_x=0;
  FixPan();
  init=false;
  Render("",cycles);
}
