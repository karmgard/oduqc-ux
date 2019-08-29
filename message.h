#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <wx/wx.h>

class message : public wxDialog {

 public:
  message (wxWindow * parent, wxWindowID id, const wxString & title, 
	   const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
	   long style=wxDEFAULT_DIALOG_STYLE, const wxString &name=wxDialogNameStr);
  message(wxWindow * parent);
  ~message(void);

  void setText(wxString);
  void show(wxString);

 private:
  void do_layout(void);

  virtual void onOK( wxCommandEvent & );
  DECLARE_EVENT_TABLE()

 protected:

  wxButton     * okButton;
  wxStaticText * txtBox;
  wxWindow     * parent;

};

#endif
