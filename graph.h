#include "MyPictureFrame.h"
#include "global.h"

#include <string>
#include <stdlib.h>

class graph : public PictureFrame {

 public:
  graph(wxWindow *, wxWindowID=wxID_ANY, std::string="No Such String");
  void loadUpdate(uint,uint);
  void update(uint, uint);
  void setGraphType(int=ADC);
  uint getGraphType(void) { return graphType; }
  void setColor( uint whichColor, string color );
  void setADCSignalScheme(void);
  void setADCCalibrationScheme(void);
  void setPercentageScheme(void);
  void updateWarningLimit(float);
  void updateFailureLimit(float);
  
 private:
  // Channel plot parameters
  uint n;
  float fh, fm, fs, fg;
  float xb, dx, margin;
  float warn, fail, ch;
  float offset;
  uint  graphType;

  void graduatePercent(void);
  void graduateADC(void);

  Magick::Color failColor, shdwColor, passColor, warnColor;

};
