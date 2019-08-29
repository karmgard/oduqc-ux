#ifndef __MYAPP_H
#define __MYAPP_H

#include <wx/wx.h>
#include <wx/image.h>
#include "wx/intl.h"
#include <X11/Xlib.h>

#ifndef APP_CATALOG
#define APP_CATALOG "oduqc"
#endif

#include "MyFrame.h"

class MyApp: public wxApp {
 public:
  MyApp(void) {XInitThreads ();return;}
  bool OnInit();
 protected:
};
DECLARE_APP(MyApp)

#endif
