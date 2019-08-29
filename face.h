#include "MyPictureFrame.h"

#include <string>
#include <stdlib.h>

class face : public PictureFrame {

public:
  face(wxWindow *, wxWindowID=wxID_ANY, std::string="No Such String");
  void loadUpdate(uint,uint);
  void update(uint, uint);
  void highlightConnector( uint, string = "#dddd4488" );

  void setType( uint = 1 );
  uint getType( void );

 private:
  unsigned int r, type;
  string fiberColorString;
  Magick::Color fiberColor;
  void drawFacePlate(void);
  float offset, H, W, h, w;
  uint n, N;
};
