#include "MyFrame.h"
#include <signal.h>
#include <time.h>

/********************************************************************/
// Timer variables & callback
static timer_t firstTimerID;
static MyFrame *self;

static void timerHandler( int sig, siginfo_t *si, void *uc ) {
  timer_t *tidp = (timer_t *)si->si_value.sival_ptr;
  static int firstCounter = 0;

  if ( *tidp == firstTimerID ) {
    firstCounter++;
    if ( self )
      cout << "Motor alarm going off\n";
    if ( firstCounter > 3 && firstTimerID != 0 )
      timer_delete(firstTimerID);
  }

  return;
}
/********************************************************************/

MyFrame::~MyFrame(void) {
  if ( arduino ) {
    arduino->send("Q");
    delete arduino;
  }

  if ( frmGraph )
    delete frmGraph;
  if ( frmFace )
    delete frmFace;
  if ( endMsg )
    delete endMsg;
  if ( dialog )
    delete dialog;
  return;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString) {

  if ( p->getBool("FULLSCREEN") )
    ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR);

  // Create the status bar
  statusbar = CreateStatusBar(1, 0);

  // The graph & ODU cartoon
  frmGraph     = new graph          (this, wxID_ANY);
  frmFace      = new face           (this, wxID_ANY);

  uint w = p->getUInt("wxWIDTH") / 8;
  wxSize size (w, 48);

  // And the button bar
  runButton    = new wxButton       (this, ID_RUNBTN,   _("&Run"), wxDefaultPosition, size);
  exitButton   = new wxButton       (this, ID_EXTBTN,   _("E&xit"), wxDefaultPosition, size);

  contButton   = new wxButton       (this, ID_CONTINUE, _("&Continue"), wxDefaultPosition, size);
  redoButton   = new wxButton       (this, ID_REDO,     _("Re&do"), wxDefaultPosition, size);
  cancelButton = new wxButton       (this, ID_CANCEL,   _("C&ancel"), wxDefaultPosition, size);
  setupButton  = new wxToggleButton (this, ID_SETBTN,   _("&Setup"), wxDefaultPosition, size);

  const int numEle = p->getInt("ODUID_MAX")+2;

  wxString * oduChoices = new wxString[numEle];
  oduChoices[0] = _("ODU ID");
  oduChoices[1] = _("Test");
  for ( int i=2; i<numEle; i++ ) {
    string s ("ODU "+to_string(i-1));
    wxString entry(s.c_str(), wxConvUTF8);
    oduChoices[i] = entry;
  }

  oduChoice = new wxChoice(this, ID_ODUID, wxDefaultPosition, size, numEle, oduChoices, 0);

  numConnectors  = p->getUInt("NUMBER_OF_CONNECTORS");
  numChannels    = p->getUInt("NUMBER_OF_CHANNELS");

  verbose      = p->getInt("VERBOSE");
  rerunTrigger = p->getFloat("RERUN_TRIGGER");
  rerunFloor   = p->getFloat("RERUN_FLOOR");
  warningLimit = p->getFloat("WARNING_LIMIT");
  maxReRuns    = p->getInt("MAX_RETESTS");
  numReRuns    = 0;

  whichConnector = -1;
  connector = (p->getBool("RUN_ALL")) ? -1 : 0;
  useHardware = p->getBool("USE_HARDWARE");
  

  /************** Change me back *******************/
  automagick = p->getBool("FULL_AUTO");
  /************************************************/

  // If we're running automagickally, don't show 
  // the continue & redo buttons. They're handled
  // auto-magickally. If not, no need to use the 
  // rerunTrigger since reruns are decided by a
  // human-like intelligence.
  if ( automagick ) {
    contButton->Show(false);
    redoButton->Show(false);
  } 
  else {
    rerunTrigger = -1;
    maxReRuns = 5;
  }
  // If we're using the arduino, we're not
  // ready until the POST is finished (which calls
  // setReady() later on)
  ready = !useHardware;

  cancelButton->Disable();
  redoButton->Disable();
  contButton->Disable();
  runButton->Disable();
  oduChoice->Disable();

  // Set up our dialog and message boxes
  endMsg = new message(this);
  dialog = new BasicDialog( this );

  // Setup the hardware if it's available
  setHardwareReady(ready);

  // Clean up a tiny little bit
  if ( oduChoices )
    delete [] oduChoices;

  // Lay out the window
  set_properties();
  do_layout();

  
  if ( useHardware ) {

    useRobot = false;

    arduino = new serial(this, p->getString("DEVICE_NAME"));
    if ( arduino->getDevice() == "dummy" )
      useHardware = false;
  }
  else {
    useRobot = false;
    arduino  = new serial(this);
  }

  return;
}

void MyFrame::verboseLevelChange( int v ) {
  if ( v < 0 )
    v = 0;
  else if ( v > 2 )
    v = 2;
  this->verbose = v;

  if ( arduino )
    arduino->setVerbose(v);

  return;
}

void MyFrame::useRobot_Changed( bool useIt ) {
  this->useRobot = useIt;
  return;
}

void MyFrame::autoRun_Changed( bool autoRun ) {

  this->automagick = autoRun;
  
  contButton->Show(!automagick);
  contButton->Disable();

  redoButton->Show(!automagick);
  contButton->Disable();
    
  do_layout();

  return;
}

void MyFrame::set_properties() {

  SetTitle(_("ODUQC"));
  SetSize(wxSize(p->getUInt("wxWIDTH"), p->getUInt("wxHEIGHT")));

  int statusbar_widths[] = { -1 };
  statusbar->SetStatusWidths(1, statusbar_widths);
  for(int i = 0; i < statusbar->GetFieldsCount(); ++i)
    setStatus(i);

  if ( p->getBool("USE_HARDWARE") )
    setStatus(19);

  return;
}

void MyFrame::do_layout() {

  wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizer_3 = new wxBoxSizer(wxHORIZONTAL);

  // Add the graph frames to the window
  sizer_2->Add(frmGraph, p->getUInt("GRAPH_RATIO"), wxALIGN_LEFT, 0);
  sizer_2->Add(frmFace,  p->getUInt("FACE_RATIO"),  wxALIGN_LEFT|wxLEFT,10);

  sizer_1->Add(sizer_2, 1, wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

  // Build the button bar
  sizer_3->Add(runButton,   1, wxALIGN_BOTTOM | wxLEFT,           5);
  sizer_3->Add(oduChoice,   1, wxALIGN_BOTTOM | wxLEFT,           5);
  sizer_3->Add(setupButton, 1, wxALIGN_BOTTOM | wxLEFT,           5);
  sizer_3->Add(contButton,  1, wxALIGN_RIGHT  | wxLEFT  | wxTOP,  5);
  sizer_3->Add(redoButton,  1, wxALIGN_RIGHT  | wxRIGHT | wxTOP,  5);
  sizer_3->Add(cancelButton,1, wxALIGN_RIGHT  | wxRIGHT | wxTOP,  5);
  sizer_3->Add(exitButton,  1, wxALIGN_BOTTOM | wxRIGHT,          5);

  // The width of the window and at least 32px high
  sizer_3->SetMinSize(wxSize(p->getUInt("wxWIDTH"), 32));

  // And add it to the window
  sizer_1->Add(sizer_3,  0, wxBOTTOM, 10);

  SetSizer(sizer_1);
  Layout();
  
  return;
}

// Define the callbacks
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_BUTTON (ID_RUNBTN,       MyFrame::run)
  EVT_TOGGLEBUTTON (ID_SETBTN, MyFrame::setup)
  EVT_BUTTON (ID_CLRBTN,       MyFrame::clear)
  EVT_BUTTON (ID_EXTBTN,       MyFrame::exit)
  EVT_BUTTON (ID_CANCEL,       MyFrame::cancel)
  EVT_BUTTON (ID_CONTINUE,     MyFrame::cont)
  EVT_BUTTON (ID_REDO,         MyFrame::redo)
  EVT_CHOICE (ID_ODUID,        MyFrame::oduChoiceChange)
END_EVENT_TABLE()

void MyFrame::makeTimer( timer_t *timerID, int expire, int interval ) {
  struct sigevent te;
  struct itimerspec its;
  struct sigaction sa;
  int sigNo = SIGRTMIN;

  /* Set up signal handler. */
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = timerHandler;
  sigemptyset(&sa.sa_mask);

  if (sigaction(sigNo, &sa, NULL) == -1)
    perror("sigaction");

  /* Set and enable alarm */
  te.sigev_notify = SIGEV_SIGNAL;
  te.sigev_signo = sigNo;
  te.sigev_value.sival_ptr = timerID;
  timer_create(CLOCK_REALTIME, &te, timerID);

  its.it_interval.tv_sec = interval;
  its.it_interval.tv_nsec = 0; //intervalMS * 1000000;
  its.it_value.tv_sec = expire;
  its.it_value.tv_nsec = 0; //expireMS * 1000000;
  int retVal = timer_settime(*timerID, 0, &its, NULL);
  if ( retVal )
    perror("timer_settime() failed");

  return;
}

// Called from serial when the arduino sends "done"
// Sets the continue/redo/cancel question
void MyFrame::continueQC(void) {

  frmFace->clear(true);

  if ( cancelRun ) {
    if ( verbose )
      cout << "Run canceled\n";

    frmGraph->clear(true);

    if ( !automagick ) {
      cancelButton->Disable(); // Finish without saving the output
      redoButton->Disable();   // Rerun the connector
      contButton->Disable();   // Finish & write the output file
    }
    exitButton->Enable();

    if ( useRobot )
      arduino->send("h");

    return;
  }

  if ( automagick ) {
    frmFace->highlightConnector(connector+1);
    sleep(1);

    wxCommandEvent dummy;
    cont(dummy);
 
    return;
  }

  if ( connector+1 >= numConnectors || whichConnector > -1 || !p->getBool("RUN_ALL")) {

    if ( automagick ) {

      sleep(1);

      wxCommandEvent dummy;
      cont(dummy);
      return;
    }

    contButton->SetLabel(_("Finish"));
    contButton->Update();

    cancelButton->Enable(); // Finish without saving the output
    redoButton->Enable();   // Rerun the connector
    contButton->Enable();   // Finish & write the output file
    runButton->Disable();
    exitButton->Disable();

    /********************** Faux click for testing porpises *************
    cancelButton->Update();
    redoButton->Update();
    contButton->Update();
    runButton->Update();
    exitButton->Update();
    
    sleep(1);
    wxCommandEvent event;
    cont(event);
    *********************************************************************/
    return;
  }

  if (!automagick || connector >= 18 ) {
    setStatus(connector+2);

    frmFace->highlightConnector(connector+1);

    cancelButton->Enable();
    redoButton->Enable();
    contButton->Enable();
    
    contButton->SetFocus();
    runButton->Disable();
    exitButton->Disable();

    /********************** Faux click for testing porpises *************
    cancelButton->Update();
    redoButton->Update();
    contButton->Update();
    runButton->Update();
    exitButton->Update();
    
    sleep(1);
    wxCommandEvent event;
    cont(event);
    *********************************************************************/
  }

  return;
}

void MyFrame::oduChoiceChange(wxCommandEvent & WXUNUSED(event)) {
  uint choice = oduChoice->GetSelection();

  if ( choice != 0 )
    runButton->Enable();

  return;
}

void MyFrame::cancel(wxCommandEvent & WXUNUSED(event)) {
  cancelRun = true;
  QCTestResult = STATE_CANCEL;
  reset();
  return;
}
void MyFrame::cont(wxCommandEvent & WXUNUSED(event)) {

  frmGraph->clear(true);

  if ( !rerunConnector )
    rerunConnector = !checkResult(string(result));

  if ( rerunConnector ) {

    if ( ++numReRuns > maxReRuns ) {
      rerunConnector = false;

      QCTestResult = STATE_FAILED;

      cout << "Connector " << connector << " failed " 
	   << numReRuns << " times. Abandoning\n";
      numReRuns = 0;
    }
    else {
      QCTestResult -= STATE_FAILED;
      if ( QCTestResult == 0 )
	QCTestResult = STATE_PASSED;
      
      if ( verbose )
	cout << "Try " << numReRuns+1 << " for connector " << connector << "\n";
    }
  }
  else {
    connector++;
    numReRuns = 0;
  }

  // Look at all the things that can cause us to end this run
  bool finish = (connector >= numConnectors);
  finish |= !p->getBool("RUN_ALL");

  finish = (QCTestResult & STATE_ERROR)   ? true : finish;
  finish = (QCTestResult & STATE_STOPPED) ? true : finish;

  if ( automagick ) {
    finish &= !rerunConnector;
    finish = (QCTestResult & STATE_FAILED)  ? true : finish;
    finish = (QCTestResult & STATE_CANCEL)  ? true : finish;
  }
  if ( finish ) {

    rerunConnector = false;
    numReRuns = 0;
    
    if ( automagick ) {
      reset();
      return;
    }

    contButton->SetLabel(_("Continue"));
    contButton->Update();
    reset();
    return;
  }

  if ( !rerunConnector && p->getBool("SAVE_OUTPUT") ) {
    if ( !checkResult(result) )
      cout << "Consistancy check failed on connector " << connector 
	   << ", you should rerun\n";
    fprintf(dataFile, "%s", result);
  }
  memset((void *)result, 0, 576*sizeof(char));

  frmFace->clear();
  frmFace->highlightConnector(connector);
  frmGraph->clear();

  rerunConnector = false;
  numReRuns = 0;

  // Set the buttons accordingly
  contButton->Disable();
  redoButton->Disable();
  cancelButton->Enable();
  
  /*************** Move to the next connector to test **************/
  startMove();
  /*****************************************************************/

  return;
}
void MyFrame::redo(wxCommandEvent & WXUNUSED(event)) {

  memset((void *)result, 0, 576);
  frmFace->clear();
  frmFace->highlightConnector(connector);
  frmGraph->clear();

  // Set the buttons accordingly
  contButton->Disable();
  redoButton->Disable();
  cancelButton->Enable();
  
  /***************** Move to this connector again ******************/
  startMove();
  /*****************************************************************/

  return;
}

void MyFrame::exit(wxCommandEvent & WXUNUSED(event)) {
  Close( true );
}

void MyFrame::clear(wxCommandEvent & WXUNUSED(event)) {
  clearFace();
  clearGraph();
  return;
}

void MyFrame::setHardwareReady( bool isReady ) {
  this->ready = isReady;

  if ( ready ) {
    oduChoice->Enable();

    if ( useHardware && p->defined("USE_ROBOT") ) {
      useRobot = p->getBool("USE_ROBOT");
    }
    else {
      useRobot = false;
    }

    // Delay a moment...
    sleep(1);

  } else {
    oduChoice->Disable();
    useRobot = false;
    Layout();
  }
  if (dialog)
    dialog->setHasRobot(useRobot);

  // Show our status as ready to go
  setStatus(0);

  return;
}

void MyFrame::redrawGraph(int which) {
  frmGraph->setGraphType(which);
  return;
}

void MyFrame::clearFace(void) {
  frmFace->clear();
  return;
}
void MyFrame::clearGraph(void) {
  frmGraph->clear();
  return;
}

void MyFrame::updateFace( uint f ) {
  
  if ( cancelRun )
    return;

  frmFace->clear();
  frmFace->highlightConnector(connector);
  /*
   * Fibers count straight accross left-to-right.
   */
  //frmFace->update(connector, f);

  frmFace->loadUpdate(connector, f);
  this->fiber = f;

  return;
}

void MyFrame::updateWarningLimit( float warning ) {
  if ( warning >= 0.0f && warning <= 1.0f )
    frmGraph->updateWarningLimit(warning);
  return;
}

void MyFrame::updateFailureLimit( float failure ) {
  if ( failure >= 0.0f && failure <= 1.0f )
    frmGraph->updateFailureLimit(failure);
  return;
}

void MyFrame::updateGraph( float v ) {
  if ( cancelRun )
    return;
  frmGraph->update(fiber, v);

  return;
}
void MyFrame::updateGraph( int channel, float v ) {
  if ( cancelRun )
    return;
  frmGraph->update(channel, v);

  return;
}

void MyFrame::updateGraph(void) {
  cout << "Failed to get a good response for fiber " << fiber 
       << " and connector " << connector << "\n";
  setStatus((string)"Bad data received! Rerunning connector");
  rerunConnector = true;
  return;
}

void MyFrame::updateGraph( char * c ) {
  
  if ( cancelRun )
    return;

  // Add this to the results for this ODU
  strcat(result, c);
  if ( fiber < numChannels-1 )
    strcat(result, (char *)",\0");
  else
    strcat(result, (char *)"\n\0");

  char *b;
  for ( uint i=0; i<strlen(c); i++ ) {
    if ( c[i] == '/' ) {
      b = c + i + 1;
      break;
    }
  }

  float ratio = atof(c) / atof(b);
  //if ( fiber == 0 )
  //frmGraph->clear(true);

  if ( frmGraph->getGraphType() == PERCENT ) {

    if ( ratio > rerunFloor )
      frmGraph->loadUpdate( fiber, (int)(100*ratio) );
  }
  else {                                 // Plot raw ADC counts
    
    if ( ratio > rerunFloor ) {
      frmGraph->setADCCalibrationScheme();
      frmGraph->loadUpdate(fiber, atoi(b));

      frmGraph->setADCSignalScheme();
      frmGraph->loadUpdate(fiber, atoi(c));
    }
  }

  if ( ratio < warningLimit && ratio > rerunFloor ) {
    if ( ratio <= rerunTrigger ) {
      rerunConnector = true;
      QCTestResult = STATE_FAILED;
    }
    else
      QCTestResult |= STATE_WARNING;
  }

  return;
}

void MyFrame::updateReRunFloor( float rerun ) {
  if ( rerun >= 0.0f && rerun <= 1.0f )
    rerunFloor = rerun;
  return;
}

bool MyFrame::checkResult( string result ) {

  istringstream iss(result);
  string token;
  int channel = 1;

  while (getline(iss, token, ',')) {
    
    struct values v = parseReading(token);
    if ( v.Lpd < 0 || v.Lstd < 0 || v.Spd < 0 || v.Sstd < 0 ) {
      cout << "Invalid reading for channel " << channel 
	   << " on connector " << connector << ": " << result << "\n";
      return false;
    }
    channel++;
  }

  return true;
}

struct values MyFrame::parseReading( string reading ) {
  struct values v;

  if ( reading[0] == '#' )
    return v;

  istringstream iss(reading);
  string token;
  int a = 0, b = 0, counter = 0;
  string signal, noise;
  
  v.Lpd = v.Lstd = v.Spd = v.Sstd = -1;

  while (getline(iss, token, '/')) {

    a = 0;
    b = token.find("(");
    if ( a < 0 || b <= 0 || b > (int)token.length() ) {
      cout << "Problem parsing signal readings: " << token << "\n";
      return v;
    }
    signal = token.substr(a,b);

    a = token.find("(") + 1;
    b = token.find(")") - a;
    if ( a <=0 || b <= 0 || a+b > (int)token.length() ) {
      cout << "Problem parsing noise readings: " << token << "\n";
      return v;
    }
    noise  = token.substr(a, b);

    if ( !counter ) {
      v.Lpd  = atoi(signal.c_str());
      v.Lstd = atoi(noise.c_str());
      counter++;
    }
    else if ( counter == 1 ) {
      v.Spd  = atoi(signal.c_str());
      v.Sstd = atoi(noise.c_str());
      counter++;
    }
    else {
      cout << "Not quite certain how we got here!\n";
    }

  }

  return v;

}

void MyFrame::updateNorms( char * buf ) {

  // And return to normal
  setStatus(0);
  dialog->Enable();

  return;
}

void MyFrame::reset( void ) {

  cancelButton->Disable();
  redoButton->Disable();
  contButton->Disable();
  runButton->Disable();
  exitButton->Enable();
  setupButton->Enable();
  oduChoice->Enable();

  frmFace->clear();
  frmGraph->clear();
  fiber = connector = 0;

  wxString msg = oduChoice->GetString(oduChoice->GetSelection());
  if (QCTestResult & STATE_FAILED ) {
    setStatus(25);
    msg += wxT(" Failed");
  }
  else if ( QCTestResult & STATE_WARNING ) {
    setStatus(24);
    msg += wxT(" Passed\nwith warnings");
  }
  else if ( QCTestResult & STATE_PASSED ) {
    setStatus(23);
    msg += wxT(" Passed");
  }
  else if ( QCTestResult & STATE_CANCEL ) {
    setStatus(0);
    msg += wxT("\nTest canceled"); 
  }
  else if ( QCTestResult & STATE_STOPPED ) {
    setStatus(0);
    msg = wxT("Panicing");
  }
  else if ( QCTestResult & STATE_ERROR ) {
    setStatus(0);
    msg += wxT(" Fatal Error\ncheck for light leaks");
  }
  else {
    setStatus(0);
    msg += wxT(" Unknown result: ");
    msg += wxString::Format(wxT("%u"),QCTestResult);
  }
  if ( endMsg )
    endMsg->show(msg);

  /****************************************************************************/
  if ( p->getBool("SAVE_OUTPUT") ) {

    if ( !checkResult(result) )
      cout << "Consistancy check failed on connector " 
	   << connector << ", you should rerun\n";

    fprintf(dataFile, "%s", result);

    wxCharBuffer buffer=msg.ToUTF8();
    fprintf(dataFile, "%s\n", buffer.data());

    // Close the data file
    fclose(dataFile);

    // And the log file
    if ( log ) {
      cout.rdbuf(cbuf);
      delete log;
    }

    char m[128];
    sprintf(m, "Output written to %s", name);
    setStatus(m);

  } // End if ( p->getBool("SAVE_OUTPUT") && !cancelRun )

  // Clear the data from the last set we ran
  memset((void *)result, 0, 576);
  /****************************************************************************/

  if ( useRobot )
    arduino->send("h");  // Robot home command

  return;
} // End reset(void)

void MyFrame::setup(bool state) {
  setupButton->SetValue(state);
  return;
}

void MyFrame::setODUSelection( int which ) {
  if ( which >= 0 && which <= p->getInt("ODUID_MAX") )
    oduChoice->SetSelection(which);
  return;
}

void MyFrame::setup(wxCommandEvent & WXUNUSED(event)) {
  if ( setupButton->GetValue() ) {
    if ( p->getBool("FULLSCREEN") )
      dialog->ShowFullScreen(true, wxFULLSCREEN_NOMENUBAR);
    else
      dialog->Show(true);
  }
   else
    dialog->Show(false);

  return;
}

void MyFrame::run(wxCommandEvent & WXUNUSED(event)) {

  if ( !this->ready ) {
    cout << "Setup not completed\n";
    return;
  }

  if ( QCTestResult & STATE_ERROR || QCTestResult & STATE_STOPPED ) {
    if ( endMsg )
      endMsg->show(_("Please clear the\nerror first"));
    setStatus(26);
    return;
  }

  cancelRun = false;
  cancelButton->Enable();

  runButton->Disable();
  exitButton->Disable();
  setupButton->Disable();
  oduChoice->Disable();

  connector = (whichConnector > -1) ? whichConnector : 0;
  fiber = 0;

  if ( p->getBool("SAVE_OUTPUT") ) {
    makeUniqueName();
  }

  /****************** We haven't failed yet by golly ****************/
  QCTestResult = STATE_PASSED;

  /****************** Move to the first connector *******************/
  startMove();
  /*****************************************************************/

  return;
}

/********** Create a unique name for the output ************/
void MyFrame::makeUniqueName( void ) {
  
  memset(name, 0, 64*sizeof(char));

  time_t now = time(0);
  tm *ltime = localtime(&now);

  int type = p->getUInt("ODU_TYPE");
  int oid  = oduChoice->GetSelection();
  string s = string(oduChoice->GetString(oid).mb_str());

  if ( s.substr(0,3) == "ODU" ) {
    s = s.substr(4);
    if ( atoi(s.c_str()) < 10 )
      s = "0" + s;
  }

  // Check and see if we have an output directory
  string dir = p->getString("OUTPUT_DIRECTORY");
  if ( dir == "NULL" || dir == "." || dir == "NONE" )
    dir = "";
  else
    dir += "/";

  // Make sure the directory exists
  struct stat sb;
  if ( stat(dir.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode))
    mkdir (dir.c_str(), (mode_t)0776);

  sprintf(name, "%sHB_%i-%s_%02i%02i%02i%02i%02i%02i.txt", 
	  dir.c_str(),type, s.c_str(), 
	  ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year-100,
	  ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

  // Open a pipeline to the file
  dataFile = fopen( name, "w" );

  // And put in the header
  fprintf(dataFile, "# Format: Avg. Signal(Signal STD)/Avg. Calibration(Calibration STD) ...\n#\n");

  if ( p->getBool("LOGGING") ) {
    char logfile[64] = {'\0'};
    sprintf(logfile,  "%sHB_%i-%s_%02i%02i%02i%02i%02i%02i.log", 
	    dir.c_str(),type, s.c_str(), 
	    ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year-100,
	    ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

    // Create a new stream
    log  = new ofstream(logfile);

    // Save the old one....
    cbuf = cout.rdbuf();

    // And replace the STDOUT with our file
    cout.rdbuf(log->rdbuf());
  }

  return;
} // End makeUniqueName()

void MyFrame::setODUType( uint which ) {
  if ( which > 0 && which < 5 )
    frmFace->setType(which);

  return;
}

void MyFrame::setConnector( int c ) {
  if ( c >= -1 && c < 18 )
    this->whichConnector = c;

  return;
}

void MyFrame::startMove( void ) {
  
  if ( !useRobot ) {
    if ( verbose > 1 )
      cout << "No robot is attached\n";
    moveComplete();

    return;
  }

  setStatus(22);

  // Set an alarm
  if ( verbose > 1 )
    cout << "Setting alarm for 1 minute from now\n";
  makeTimer( &firstTimerID, 10, 10 );

  // Move to connector
  if ( verbose > 1 )
    cout << "Moving to connector " << connector << "\n";
  arduino->send("m " + itoa(connector+1));

  return;
}

void MyFrame::moveComplete( void ) {
  if ( verbose > 1 )
    cout << "Canceling alarm\n";
  if ( firstTimerID != 0 )
    timer_delete(firstTimerID);

  if ( verbose > 1 )
    cout << "Running the LEDs\n";
  setStatus(21);

  // Activate the circuit to sequence the LEDs
  arduino->send("s");

  return;
}

void MyFrame::PanicButton( void ) {

  // Don't allow anything else to proceed
  QCTestResult = STATE_STOPPED;

  // Are we currently running a test?
  if ( cancelButton->IsEnabled() ) {
    cancelRun = true;
    reset();
  }
  
  return;
}

void MyFrame::FatalError( void ) {
  if ( verbose )
    cout << "OH SHIT! Panic Time!\n";

  QCTestResult = STATE_ERROR;
  cancelRun = true;
  reset();

  return;
}
void MyFrame::FatalErrorCleared( void ) {
  QCTestResult = 0;
  whichConnector = -1;
  return;
}
