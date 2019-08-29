#include "message.h"
#include "MyFrame.h"

using namespace std;

message::message (wxWindow * parent, wxWindowID id, const wxString & title, const wxPoint &pos, const wxSize &size, long style, const wxString &name) : wxDialog( parent, id, title, pos, size, style) {

  this->parent = parent;

  wxSize sz ( 96, 32 );
  okButton = new wxButton( this, wxID_OK,   _("&OK"), wxDefaultPosition, sz, wxALIGN_CENTER);
  txtBox = new wxStaticText(this, wxID_ANY, _(""), wxDefaultPosition, wxSize(128,48), wxALIGN_CENTRE);

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *btnSizer  = new wxBoxSizer( wxHORIZONTAL );
  mainSizer->Add(txtBox, 1, wxEXPAND|wxLEFT, 60);
  btnSizer->Add(okButton, 1, wxEXPAND|wxLEFT, 48);
  mainSizer->Add(btnSizer);

  SetSizerAndFit(mainSizer);

  return;
}

message::message(wxWindow * parent) : wxDialog(parent,wxID_ANY,_("QCBot")) {
  
  this->parent = parent;

  okButton = 
    new wxButton(this, wxID_OK, _("OK"), wxDefaultPosition, wxSize(96,32));

  txtBox = 
    new wxStaticText(this, wxID_ANY, _(""), wxDefaultPosition, wxSize(128,48));

  wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *btnSizer  = new wxBoxSizer( wxHORIZONTAL );

  btnSizer->Add(okButton, 1, wxEXPAND|wxLEFT, 24);

  mainSizer->Add(txtBox, 1, wxEXPAND|wxLEFT, 24);
  mainSizer->Add(btnSizer);

  SetSizerAndFit(mainSizer);

  return;
}

message::~message(void) {
  return;
}

void message::setText( wxString label ) {
  txtBox->SetLabel( label );
  return;
}

void message::onOK( wxCommandEvent & WXUNUSED(event)) {
  ((MyFrame *)parent)->setODUSelection(0);
  this->Show(false);
  return;
}

void message::show( wxString label ) {
  setText(label);
  Show(true);
  return;
}

BEGIN_EVENT_TABLE(message, wxDialog)
  EVT_BUTTON   (wxID_OK,     message::onOK)
END_EVENT_TABLE()
