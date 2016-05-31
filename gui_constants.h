#ifndef gui_constants_h
#define gui_constants_h
#include <string>
#include <algorithm> //sort
#include <vector>
#include <GL/glut.h>
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
//#include "gui.h"


enum {
// Frame events
  MY_BUTTONRUN_ID=wxID_HIGHEST + 1,
  MY_BUTTONCONT_ID,
  MY_BUTTONSETMON_ID,
  MY_SWITCH_ID,
  MY_NOTEBOOK_ID,
  MY_FRAME_ID,

// Monitor dialog box events
  MY_ADDMON_ID,
  MY_REMMON_ID,

// Language menu events
  //MY_LANG_ID,
  //MY_EN_LANG_ID,
  //MY_LT_LANG_ID,
}; // widget identifiers


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




#endif
