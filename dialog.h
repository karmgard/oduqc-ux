#ifndef __DIALOG_H
#define __DIALOG_H

#include <wx/wx.h>
#include "global.h"

class MyFrame;

enum {
  ID_TYPE,
  ID_RUN,
  ID_PLOT,
  ID_SUB,
  ID_SAVE,
  ID_AUTO,
  ID_ROBOT,
  ID_CAL,
  ID_CONN,
  ID_ADC,
  ID_DISMISS,
  ID_SAMPLES,
  ID_LEDS,
  ID_HOME,
  ID_REHOME,
  ID_PLACE,
  ID_CLEAR,
  ID_VERBOSE,
  ID_LOG,

  ID_DELAY,
  ID_FLOOR,
  ID_WARNING,
  ID_FAILURE
};

class BasicDialog : public wxDialog {
 public:
 
  BasicDialog ( MyFrame * parent, wxWindowID id, const wxString & title,
		const wxPoint & pos = wxDefaultPosition,
		const wxSize & size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE );
  BasicDialog( MyFrame * parent );
  ~BasicDialog(void);

  wxString GetText();
  void setHasRobot( bool );

 protected:

  MyFrame * parent;

  wxButton     *okButton, *svButton, *cbButton, *homeBtn, *reHomeBtn, *chkButton, *clrButton;
  wxRadioBox   *typeRadioBox, *runOptRadioBox, *plotRadioBox;
  wxCheckBox   *noiseCheckBox, *saveCheckBox, *autoCheckBox,*robotCheckBox,*logCheckBox;
  wxChoice     *connectorChoice;
  wxPanel      *optPanel;
  wxSlider     *slider_1, *slider_2,*slider_3,*slider_4,*slider_5,*slider_6,*slider_7;
  wxStaticText *label_1, *label_2, *label_3,*label_4,*label_5,*label_6,*label_7;

  int connector;

 private:

  void set_properties(void);
  void do_layout(void);

  virtual void OnOk                  ( wxCommandEvent & );
  virtual void OnSave                ( wxCommandEvent & );
  virtual void onHomeMotors          ( wxCommandEvent & );
  virtual void onResetMotorsHome     ( wxCommandEvent & );
  virtual void OnCalibrate           ( wxCommandEvent & );
  virtual void OnCancel              ( wxCommandEvent & );
  virtual void typeRadio             ( wxCommandEvent & );
  virtual void runOptRadio           ( wxCommandEvent & );
  virtual void plotRadio             ( wxCommandEvent & );
  virtual void subtractCheckChange   ( wxCommandEvent & );
  virtual void saveCheckChange       ( wxCommandEvent & );
  virtual void autoCheckChange       ( wxCommandEvent & );
  virtual void robotCheckChange      ( wxCommandEvent & );
  virtual void logCheckChange        ( wxCommandEvent & );
  virtual void connectorChoiceChange ( wxCommandEvent & );
  virtual void checkPlacement        ( wxCommandEvent & );
  virtual void clearError            ( wxCommandEvent & );
  virtual void sampleSliderChange    ( wxScrollEvent & );
  virtual void ledSliderChange       ( wxScrollEvent & );
  virtual void delaySliderChange     ( wxScrollEvent & );
  virtual void floorSliderChange     ( wxScrollEvent & );
  virtual void warnSliderChange      ( wxScrollEvent & );
  virtual void failSliderChange      ( wxScrollEvent & );
  virtual void sampleSliderEnd       ( wxScrollEvent & );
  virtual void ledSliderEnd          ( wxScrollEvent & );
  virtual void delaySliderEnd        ( wxScrollEvent & );
  virtual void floorSliderEnd        ( wxScrollEvent & );
  virtual void warnSliderEnd         ( wxScrollEvent & );
  virtual void failSliderEnd         ( wxScrollEvent & );
  virtual void vrbSliderEnd          ( wxScrollEvent & );
  virtual void vrbSliderChange       ( wxScrollEvent & );

  DECLARE_EVENT_TABLE()
};

#endif
