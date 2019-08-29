#include "dialog.h"
#include "MyFrame.h"

#include <parameters.h>
extern parameters *p;

#include <wx/tooltip.h>

BasicDialog::BasicDialog ( MyFrame * parent ) :
  wxDialog(parent, wxID_ANY, _("Settings")) {
  this->parent = parent;
 
  optPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(600,400), 
			 wxDOUBLE_BORDER|wxRAISED_BORDER|wxTAB_TRAVERSAL);

  const wxString typeRadioBox_choices[] = {
    _("Type 1"),
    _("Type 2"),
    _("Type 3"),
    _("Type 4")
  };
  typeRadioBox = new wxRadioBox(optPanel, ID_TYPE, _("ODU"), wxDefaultPosition, wxSize(80,133), 4, typeRadioBox_choices, 5, wxRA_SPECIFY_ROWS);

  const wxString runOptRadioBox_choices[] = {
    _("All Connectors"),
    _("One Connector")
  };
  runOptRadioBox = new wxRadioBox(optPanel, ID_RUN, _("Run"), wxDefaultPosition, wxDefaultSize, 2, runOptRadioBox_choices, 0, wxRA_SPECIFY_ROWS);

  const wxString plotRadioBox_choices[] = {
    _("Raw ADC Counts"),
    _("Percentage")
  };
  plotRadioBox = new wxRadioBox(optPanel, ID_PLOT, _("Plot"), wxDefaultPosition, wxDefaultSize, 2, plotRadioBox_choices, 0, wxRA_SPECIFY_ROWS);

  const wxString connectorChoice_choices[] = {
    _("Connector 1"),
    _("Connector 2"),
    _("Connector 3"),
    _("Connector 4"),
    _("Connector 5"),
    _("Connector 6"),
    _("Connector 7"),
    _("Connector 8"),
    _("Connector 9"),
    _("Connector 10"),
    _("Connector 11"),
    _("Connector 12"),
    _("Connector 13"),
    _("Connector 14"),
    _("Connector 15"),
    _("Connector 16"),
    _("Connector 17"),
    _("Connector 18")
  };
  connectorChoice = new wxChoice(optPanel, ID_CONN, wxDefaultPosition, wxDefaultSize, 18, connectorChoice_choices, 0);

  noiseCheckBox = new wxCheckBox(optPanel, ID_SUB,   _("Subtract Noise"));
  saveCheckBox  = new wxCheckBox(optPanel, ID_SAVE,  _("Save Output"));
  autoCheckBox  = new wxCheckBox(optPanel, ID_AUTO,  _("Auto Run"));
  robotCheckBox = new wxCheckBox(optPanel, ID_ROBOT, _("Use Robot"));
  logCheckBox   = new wxCheckBox(optPanel, ID_LOG,   _("Logging"));

  /***********************************************/
  int samples = p->getUInt("NUMBER_OF_SAMPLES");
  int leds    = p->getUInt("NUMBER_OF_CHANNELS");
  uint delay  = p->getUInt("PER_FIBER_DELAY");
  float floor = p->getFloat("RERUN_FLOOR");
  int verbose = p->getInt("VERBOSE");
  float warn  = p->getFloat("WARNING_LIMIT");
  float fail  = p->getFloat("FAILURE_LIMIT");

  slider_1 = new wxSlider(optPanel, ID_SAMPLES, samples, 8, 2048);
  slider_2 = new wxSlider(optPanel, ID_LEDS, leds, 0, 18);
  slider_3 = new wxSlider(optPanel, ID_VERBOSE, verbose, 0, 2);

  slider_4 = new wxSlider(optPanel, ID_DELAY, delay, 0, 6000);
  slider_4->SetPageSize(300);

  slider_5 = new wxSlider(optPanel, ID_FLOOR, 100*floor, 0, 100);
  slider_5->SetPageSize(5);

  slider_6 = new wxSlider(optPanel, ID_WARNING, 100*warn, 0, 100);
  slider_6->SetPageSize(5);

  slider_7 = new wxSlider(optPanel, ID_FAILURE, 100*fail, 0, 100);
  slider_7->SetPageSize(5);

  label_1 = new wxStaticText(optPanel, wxID_ANY, _("Sample Size"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  label_2 = new wxStaticText(optPanel, wxID_ANY, _("Number of LEDs"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  label_3 = new wxStaticText(optPanel, wxID_ANY, _("Verbose"));
  label_4 = new wxStaticText(optPanel, wxID_ANY, _("Diode Read Delay"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  label_5 = new wxStaticText(optPanel, wxID_ANY, _("Graph Display Floor (%)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  label_6 = new wxStaticText(optPanel, wxID_ANY, _("Warning Level (%)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
  label_7 = new wxStaticText(optPanel, wxID_ANY, _("Failure Level (%)"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);


  uint w = p->getUInt("wxWIDTH") / 8;
    
  wxSize sz = wxSize(w, 32);
  okButton = new wxButton( optPanel, wxID_OK, _("&Dismiss"), wxDefaultPosition, sz);
  svButton = new wxButton( optPanel, ID_SAVE, _("&Save"), wxDefaultPosition, sz);

  homeBtn   = new wxButton( optPanel, ID_HOME,   _("&Home"),  wxDefaultPosition, sz);
  reHomeBtn = new wxButton( optPanel, ID_REHOME, _("&Reset"), wxDefaultPosition, sz);
  chkButton = new wxButton( optPanel, ID_PLACE,  _("Chec&k"), wxDefaultPosition, sz);
  clrButton = new wxButton( optPanel, ID_CLEAR,  _("Cl&ear"), wxDefaultPosition, sz);
  cbButton  = new wxButton( optPanel, ID_CAL,    _("&Calibrate"), wxDefaultPosition, sz);

  set_properties();
  do_layout();

  return;
}

BasicDialog::~BasicDialog(void) {
  return;
}

void BasicDialog::setHasRobot( bool hasRobot ) {

  robotCheckBox->Show(hasRobot);
  autoCheckBox->Show(hasRobot);
  homeBtn->Show(hasRobot);
  reHomeBtn->Show(hasRobot);
  chkButton->Show(hasRobot);

  /*
  robotCheckBox->SetValue(hasRobot);
  robotCheckBox->Enable(hasRobot);

  autoCheckBox->SetValue(hasRobot);
  autoCheckBox->Enable(hasRobot);

  homeBtn->Enable(hasRobot);
  reHomeBtn->Enable(hasRobot);
  chkButton->Enable(hasRobot);
  */
  return;
}

void BasicDialog::set_properties(void) {
  optPanel->SetBackgroundColour(wxColour(165,165,165));

  SetTitle(_("Settings"));
  homeBtn->SetToolTip(_("Send all the motors to the home position"));
  reHomeBtn->SetToolTip(_("Send all the motors to home and reset what \"home\" means"));
  chkButton->SetToolTip(_("Check the ODU placement by plugging in to a random connector"));
  clrButton->SetToolTip(_("Clear a fatal error or panic button on the Arduino"));

  cbButton->SetToolTip(_("Calibate the QC station. Make sure the connector is set against the acrylic light guide"));

  okButton->SetToolTip(_("Close this settings panel without saving. Changes will be lost when the program closes"));

  svButton->SetToolTip(_("Save your changes to the configuration file and close this settings panel. Changes will be maintained and loaded as defaults on the next program start"));
  
  uint type = p->getUInt("ODU_TYPE")-1;
  typeRadioBox->SetSelection(type);

  if ( p->getBool("RUN_ALL") ) {
    runOptRadioBox->SetSelection(0);
    connectorChoice->Enable(false);
  } else {
    runOptRadioBox->SetSelection(1);
    connectorChoice->Enable(true);
  }
  connectorChoice->SetSelection(parent->getConnector());

  if ( p->getString("GRAPH_STYLE") == "ADC" )
    plotRadioBox->SetSelection(0);
  else
    plotRadioBox->SetSelection(1);

  noiseCheckBox->SetValue( p->getBool("SUBTRACT_NOISE") );
  saveCheckBox->SetValue(p->getBool("SAVE_OUTPUT") );
  autoCheckBox->SetValue(p->getBool("FULL_AUTO") );
  robotCheckBox->SetValue(p->getBool("USE_ROBOT") );

  // Start with the auto/robot disabled, wait for the main window to
  // tell us that the robot exists and is healthy
  autoCheckBox->Enable(false);
  robotCheckBox->Enable(false);
  homeBtn->Enable(false);
  reHomeBtn->Enable(false);
  chkButton->Enable(false);
  
  slider_1->SetMinSize(wxSize(225,20));
  slider_2->SetMinSize(wxSize(225,20));
  slider_3->SetMinSize(wxSize( 75,20));
  slider_4->SetMinSize(wxSize(225,20));
  slider_5->SetMinSize(wxSize(225,20));
  slider_6->SetMinSize(wxSize(225,20));
  slider_7->SetMinSize(wxSize(225,20));

  return;
}

void BasicDialog::do_layout(void) {

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  //wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* optPanelSizer = new wxBoxSizer(wxVERTICAL);
  wxGridSizer* choiceSizer = new wxGridSizer(5, 2, 0, 0);
  wxBoxSizer* rowSizer1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* rowSizer2 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* typeSizer = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* optSizer = new wxBoxSizer(wxVERTICAL);

  typeSizer->Add(typeRadioBox, 0, wxALL, 5);
  optSizer->Add(runOptRadioBox, 0, wxRIGHT|wxTOP|wxBOTTOM, 5);
  optSizer->Add(plotRadioBox, 0, wxRIGHT|wxBOTTOM, 5);
  typeSizer->Add(optSizer, 0,wxEXPAND, 0);

  wxBoxSizer *logLevelSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *verboseSizer  = new wxBoxSizer(wxHORIZONTAL);

  logLevelSizer->Add( connectorChoice, 0, wxTOP, 20 );
  connectorChoice->Enable( !p->getBool("RUN_ALL") );

  verboseSizer->Add( label_3, 0, wxRIGHT, 5);
  verboseSizer->Add( slider_3, 0, 0, 0 );

  logLevelSizer->Add(verboseSizer, 0, wxTOP, 20);
  logLevelSizer->Add(logCheckBox, 0, wxTOP, 10);

  typeSizer->Add(logLevelSizer, 0, wxLEFT, 25);
  optPanelSizer->Add(typeSizer, 1, 0, 0);
  
  /***********************************************/
  choiceSizer->Add(noiseCheckBox, 0, 0, 0);
  choiceSizer->Add(saveCheckBox, 0, 0, 0);

  if ( p->getBool("USE_ROBOT") ) {
    choiceSizer->Add(autoCheckBox, 0, 0, 0);
    choiceSizer->Add(robotCheckBox, 0,0,0);
  }
  
  choiceSizer->Add(label_1, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_1, 0, wxALL|wxALIGN_TOP,  0);

  choiceSizer->Add(label_2, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_2, 0, wxALL|wxALIGN_TOP,  0);

  choiceSizer->Add(label_4, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_4, 0, wxALL|wxALIGN_TOP,  0);

  choiceSizer->Add(label_5, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_5, 0, wxALL|wxALIGN_TOP,  0);

  choiceSizer->Add(label_6, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_6, 0, wxALL|wxALIGN_TOP,  0);

  choiceSizer->Add(label_7, 10, wxLEFT|wxALIGN_TOP, 10);
  choiceSizer->Add(slider_7, 0, wxALL|wxALIGN_TOP,  0);

  rowSizer1->Add(okButton,    0, wxLEFT, 10);
  rowSizer1->Add(svButton,    0, wxLEFT, 10);
  
  rowSizer1->Add(clrButton,   0, wxLEFT, 20);
  rowSizer1->Add(cbButton,    0, wxLEFT|wxRIGHT, 10);

  if ( p->getBool("USE_ROBOT") ) {
    rowSizer2->Add(chkButton,   0, wxRIGHT, 2);
    rowSizer2->Add(homeBtn,     0, wxRIGHT, 2);
    rowSizer2->Add(reHomeBtn,   0, wxRIGHT, 15);
    choiceSizer->Add(rowSizer2, 0, wxBOTTOM, 5);
  }
  /***********************************************/
  optPanelSizer->Add(choiceSizer, 0, wxBOTTOM, 25);
  optPanelSizer->Add(rowSizer1,   0, wxBottom, 10);

  optPanel->SetSizer(optPanelSizer);
  optPanel->CenterOnParent();
  
  panelSizer->Add(optPanel, 2, 0, 0);
  mainSizer->Add(panelSizer, 5, wxALL, 2);
  //buttonSizer->Add(okButton, 1, wxALIGN_TOP|wxLEFT, 5);
  //buttonSizer->Add(svButton, 1, wxALIGN_TOP|wxLEFT, 5);
  //mainSizer->Add(buttonSizer, 1, wxLEFT|wxRIGHT, 5);

  SetSizer(mainSizer);
  mainSizer->Fit(this);

  Layout();

  return;
}

BEGIN_EVENT_TABLE(BasicDialog, wxDialog)
  EVT_RADIOBOX (ID_TYPE,     BasicDialog::typeRadio)
  EVT_RADIOBOX (ID_RUN,      BasicDialog::runOptRadio)
  EVT_RADIOBOX (ID_PLOT,     BasicDialog::plotRadio)
  EVT_CHECKBOX (ID_SUB,      BasicDialog::subtractCheckChange)
  EVT_CHECKBOX (ID_SAVE,     BasicDialog::saveCheckChange)
  EVT_CHECKBOX (ID_AUTO,     BasicDialog::autoCheckChange)
  EVT_CHECKBOX (ID_ROBOT,    BasicDialog::robotCheckChange)
  EVT_CHECKBOX (ID_LOG,      BasicDialog::logCheckChange)

  EVT_CHOICE   (ID_CONN,     BasicDialog::connectorChoiceChange)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_SAMPLES,  BasicDialog::sampleSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_SAMPLES,  BasicDialog::sampleSliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_LEDS,     BasicDialog::ledSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_LEDS,     BasicDialog::ledSliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_DELAY,    BasicDialog::delaySliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_DELAY,    BasicDialog::delaySliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_FLOOR,    BasicDialog::floorSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_FLOOR,    BasicDialog::floorSliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_WARNING,  BasicDialog::warnSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_WARNING,  BasicDialog::warnSliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_FAILURE,  BasicDialog::failSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_FAILURE,  BasicDialog::failSliderEnd)

  EVT_COMMAND_SCROLL_THUMBTRACK   (ID_VERBOSE,  BasicDialog::vrbSliderChange)
  EVT_COMMAND_SCROLL_THUMBRELEASE (ID_VERBOSE,  BasicDialog::vrbSliderEnd)

  EVT_BUTTON   (wxID_OK,     BasicDialog::OnOk)
  EVT_BUTTON   (wxID_CANCEL, BasicDialog::OnCancel)

  EVT_BUTTON   (ID_HOME,     BasicDialog::onHomeMotors)
  EVT_BUTTON   (ID_REHOME,   BasicDialog::onResetMotorsHome)

  EVT_BUTTON   (ID_SAVE,     BasicDialog::OnSave)
  EVT_BUTTON   (ID_CAL,      BasicDialog::OnCalibrate)
  EVT_BUTTON   (ID_PLACE,    BasicDialog::checkPlacement)
  EVT_BUTTON   (ID_CLEAR,    BasicDialog::clearError)

END_EVENT_TABLE()

// Clear a fatal error on the Arduino
void BasicDialog::clearError(wxCommandEvent & WXUNUSED(event)) {
  parent->sendToSerial("c");
  return;
}

// Run a quick plug-in test to make sure the ODU is aligned
void BasicDialog::checkPlacement(wxCommandEvent & WXUNUSED(event)) {

  uint connector = 1 + rand() % 5;
  while ( connector > p->getUInt("NUMBER_OF_CONNECTORS") )
    connector = 1 + rand() % 5;

  if ( p->getInt("VERBOSE") )
    cout << "Checking connector " << connector << " for placement\n";

  parent->sendToSerial("a"+itoa(connector));

  return;
}

// Send the motors home on both axes
void BasicDialog::onHomeMotors(wxCommandEvent & WXUNUSED(event)) {
  parent->sendToSerial("h");
  return;
}

// Have the controller reset where home is
void BasicDialog::onResetMotorsHome(wxCommandEvent & WXUNUSED(event)) {
  parent->sendToSerial("H");
  return;
}

void BasicDialog::sampleSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_1->GetValue();
  label_1->SetLabel(wxString::Format(wxT("%i"),value));
  return;
}

void BasicDialog::ledSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_2->GetValue();
  label_2->SetLabel(wxString::Format(wxT("%i"),value));

  return;
}

void BasicDialog::delaySliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_4->GetValue();
  label_4->SetLabel(wxString::Format(wxT("%i"),value));

  return;
}

void BasicDialog::floorSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_5->GetValue();
  label_5->SetLabel(wxString::Format(wxT("%i"),value));

  return;
}

void BasicDialog::warnSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_6->GetValue();
  label_6->SetLabel(wxString::Format(wxT("%i"),value));

  return;
}

void BasicDialog::failSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_7->GetValue();
  label_7->SetLabel(wxString::Format(wxT("%i"),value));

  return;
}

void BasicDialog::vrbSliderChange(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_3->GetValue();
  switch (value) {
    case 0:
      label_3->SetLabel(_("Off"));
      break;
    case 1:
      label_3->SetLabel(_("Medium"));
      break;

    case 2:
      label_3->SetLabel(_("High"));
      break;
    }

  return;
}

void BasicDialog::sampleSliderEnd(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_1->GetValue();
  parent->sendToSerial("S " + to_string(value));
  p->setUInt("NUMBER_OF_SAMPLES", value);
  label_1->SetLabel(_("Sample Size"));

  return;
}

void BasicDialog::ledSliderEnd(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_2->GetValue();
  parent->sendToSerial("l " + to_string(value));
  p->setUInt("NUMBER_OF_CHANNELS", value);
  label_2->SetLabel(_("Number of LEDs"));

  return;
}

void BasicDialog::delaySliderEnd(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_4->GetValue();
  parent->sendToSerial("D " + to_string(value));
  p->setUInt("PER_FIBER_DELAY", value);
  label_4->SetLabel(_("Diode Read Delay"));

  return;
}

void BasicDialog::floorSliderEnd(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_5->GetValue();

  p->setFloat("RERUN_FLOOR", 0.01f*value);
  label_5->SetLabel(_("Graph Display Floor (%)"));

  parent->updateReRunFloor(0.01f * value );
  if ( p->getInt("VERBOSE") > 1 )
    cout << "RERUN_FLOOR set to " << p->getFloat("RERUN_FLOOR") << "\n";

  return;
}

void BasicDialog::warnSliderEnd(wxScrollEvent & WXUNUSED(event)) {
  int value = slider_6->GetValue();

  p->setFloat("WARNING_LIMIT", 0.01f*value);
  label_6->SetLabel(_("Warning Level (%)"));

  parent->updateWarningLimit( 0.01f * value );
  
  if ( p->getInt("VERBOSE") > 1 )
    cout << "WARNING_LIMIT set to " << p->getFloat("WARNING_LIMIT") << "\n";

  return;
}

void BasicDialog::failSliderEnd(wxScrollEvent & WXUNUSED(event)) {
  int value = slider_7->GetValue();
  p->setFloat("FAILURE_LIMIT", 0.01f*value);
  label_7->SetLabel(_("Failure Level (%)"));

  parent->updateFailureLimit( 0.01f * value );
  
  if ( p->getInt("VERBOSE") > 1 )
    cout << "FAILURE_LIMIT set to " << p->getFloat("FAILURE_LIMIT") << "\n";

  return;
}

void BasicDialog::vrbSliderEnd(wxScrollEvent & WXUNUSED(event)) {

  int value = slider_3->GetValue();
  parent->verboseLevelChange(value);
  p->setInt("VERBOSE", value);
  label_3->SetLabel(_("Verbose"));

  return;
}

void BasicDialog::typeRadio(wxCommandEvent &event) {
  int choice = typeRadioBox->GetSelection() + 1;
  if ( choice > 0 && choice < 5 )
    parent->setODUType(choice);
  p->setUInt("ODU_TYPE", choice);

  if ( p->getBool("USE_ROBOT") )
    parent->sendToSerial("T" + to_string(choice));
  
  return;
}

void BasicDialog::runOptRadio(wxCommandEvent & WXUNUSED(event)) {
  int choice = runOptRadioBox->GetSelection();

  if ( choice == 1 )
    connector = connectorChoice->GetSelection();
  else
    connector = -1;

  p->setBool("RUN_ALL", (choice==0));

  connectorChoice->Enable( choice == 1 );
  parent->setConnector( connector );

  return;
}

void BasicDialog::plotRadio(wxCommandEvent & WXUNUSED(event)) {
  parent->redrawGraph(plotRadioBox->GetSelection());
  return;
}

void BasicDialog::subtractCheckChange(wxCommandEvent & WXUNUSED(event)) {
  bool choice = noiseCheckBox->GetValue();
  parent->sendToSerial("b " + to_string(choice));
  p->setBool("SUBTRACT_NOISE", choice);

  return;
}

void BasicDialog::autoCheckChange(wxCommandEvent & WXUNUSED(event)) {
  bool choice = autoCheckBox->GetValue();
  p->setBool("FULL_AUTO", choice);
  parent->autoRun_Changed(choice);

  homeBtn->Enable(choice);
  reHomeBtn->Enable(choice);
  chkButton->Enable(choice);
  
  return;
}

void BasicDialog::robotCheckChange(wxCommandEvent & event) {
  bool choice = robotCheckBox->GetValue();

  // Set our RCP flag
  p->setBool("USE_ROBOT", choice);
  parent->useRobot_Changed(choice);

  // Assume the user also wants automation
  autoCheckBox->SetValue(choice);
  autoCheckChange(event);
  
  // Let the arduino know what happened
  parent->sendToSerial( (choice) ?
			"G" + to_string(p->getUInt("ODU_TYPE")) : "g" );

  // And set the buttons to reflect our new choice
  homeBtn->Enable(choice);
  reHomeBtn->Enable(choice);
  chkButton->Enable(choice);
  
  return;
}

void BasicDialog::logCheckChange(wxCommandEvent & WXUNUSED(event)) {
  p->setBool( "LOGGING", logCheckBox->GetValue() );
  return;
}

void BasicDialog::saveCheckChange(wxCommandEvent & WXUNUSED(event)) {
  bool choice = saveCheckBox->GetValue();
  p->setBool("SAVE_OUTPUT", choice);

  return;
}

void BasicDialog::connectorChoiceChange(wxCommandEvent & WXUNUSED(event)) {
  int choice = connectorChoice->GetSelection();
  connector = choice;
  ((MyFrame *)parent)->setConnector( choice );
  return;
}

void BasicDialog::OnOk( wxCommandEvent & WXUNUSED(event)) {
  this->Show(false);
  parent->setup(false);
  return;
}

void BasicDialog::OnCancel( wxCommandEvent & WXUNUSED(event)) {
  this->Show(false);
  parent->setup(false);
  return;
}

void BasicDialog::OnCalibrate( wxCommandEvent & WXUNUSED(event)) {
  parent->sendToSerial("C");
  this->Disable();
  return;
}

void BasicDialog::OnSave( wxCommandEvent & WXUNUSED(event)) {
  p->flush(p->getInfile(), p->getInfile());
  return;
}
