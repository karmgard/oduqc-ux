#include "MyApp.h"

/********** Added by me *************/
#include "parameters.h"
parameters *p;
extern void get_command_line( int, wxChar **);
/************************************/

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {

  /********** Added by me *************/
  get_command_line( wxGetApp().argc,wxGetApp().argv );
  /************************************/

  wxInitAllImageHandlers();

  srand(time(NULL));

  MyFrame * frame = new MyFrame();
  SetTopWindow(frame);
  frame->Show();

  return true;
}
