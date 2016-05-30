#ifndef guiCanvas_h
#define guiCanvas_h

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include "names.h"
#include "devices.h"
#include "monitor.h"
#include "gui.h"
#include "gui_constants.h"



class MyFrame;

class MyGLCanvas: public wxGLCanvas{

 public:
  MyGLCanvas(wxWindow *parent, wxWindowID id = wxID_ANY, monitor* monitor_mod = NULL, names* names_mod = NULL,
	     const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0,
	     const wxString& name = "MyGLCanvas", const wxPalette &palette=wxNullPalette); // constructor
  void Render(wxString example_text = "", int cycles = -1, bool spinchange = false); // function to draw canvas contents
  void Reset(int cycles=-1);		// Called on resetting moitors
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

#endif
