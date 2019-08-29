#include "face.h"
#include <global.h>

using namespace Magick;
using namespace std;

face::face(wxWindow *parent, wxWindowID id, std::string file) {
  this->file = file;
  if ( this->file == "No Such String" ) {
    this->file = "face";
    this->fromFile = false;
  } else
    this->fromFile = true;

  width  = p->getUInt("fWIDTH");
  height = p->getUInt("fHEIGHT");

  this->Create(parent, id);
  this->LoadImage();

  this->type = p->getUInt("ODU_TYPE");

  this->n = p->getUInt("NUMBER_OF_CHANNELS");
  this->N = p->getUInt("NUMBER_OF_CONNECTORS");

  this->offset = 10;
  this->H  = height - 2*offset;
  this->W  = width  - 2*offset;

  this->h  = (H/(N-2) + 0.333);
  this->w  = (W - h);

  this->r = 0.1*h;

  hasUpdate = false;
  
  this->drawFacePlate();

  this->fiberColorString = p->getString("COLOR_FIBER");
  this->fiberColor = Color(this->fiberColorString);

  return;
}

void face::highlightConnector( uint connector, string color ) {
  uint x0=0,y0=0,xf=0,yf=0;

  //Draw the connector rectangles on the face.
  // h & w are calculated in the instantiator
  if ( connector < 16 ) {
    if (type == 2 || type == 4)
      x0 = offset + h;
    else
      x0 = offset;
    xf = x0 + w;

    y0 = offset + connector * h;
    yf = y0 + 0.8*h;
  }
  else {
    if ( type == 2 || type == 4 )
      x0 = offset;
    else
      x0 = 1.08*w;
    xf = x0 + 0.8*h;

    y0 = 2*offset;
    if ( connector == 17 )
      y0 += w + offset;
    yf = y0 + w;

  }
  
  image->strokeWidth(1);        // 1px border
  image->strokeColor(color);    // Outline color 
  image->fillColor(color);      // Drawing color

  image->draw( DrawableRectangle(x0,y0,xf,yf) );

  // Force a refresh/repaint event
  this->invalidate();

  return;
}

void face::loadUpdate( uint p1, uint p2 ) {
  parameter1 = p1;
  parameter2 = p2;

  hasUpdate = true;
  
  // Force a refresh/repaint event
  this->invalidate();

  return;
}

// This gets called from OnPaint when loadUpdate has been called
void face::update(uint connector, uint fiber) {

  if ( !image ) {
    image = new Image(*base);

    if ( !image )
      return;
  }

  try {
    // get a unique lock on the image
    image->modifyImage();
  }
  catch (Exception e) {
    cout << "Unable to get exclusive lock on the image!\n";
    return;
  }

  // Draw the image of a fiber on the connector
  // offset r, h, & w are calculated in the instantiator
  uint x=0,y=0;
  if ( connector < 16 ) {
    x = 2*offset + fiber*4.33*r;
    if ( type == 2 || type == 4 )
      x += h;
    y = offset + (connector+0.375) * h;
  }
  else {
    x = offset + 0.4 * h;
    if ( type == 1 || type == 3 )
      x+= w + 0.15*h;
    y = 3*offset + fiber*4.33*r;
    if ( connector == 17 )
      y += w + offset;
  }

  try {
    image->strokeWidth(0);           // No border
    image->strokeColor(bkgColor);    // Outline color 
    image->fillColor(fiberColor);    // Drawing color

    if ( x < width && y < height )
      image->draw( DrawableCircle(x, y, x+r, y+r) );
    else
      *image = *base;
  }
  catch (Exception e) {
    cout << "Caught exception trying to update face image\n";
  }

  hasUpdate = false;
  
  return;

} // End void update(uint connector, uint fiber)


void face::drawFacePlate( void ) {

  for ( uint i=0; i<N; i++ )
    highlightConnector(i, "#222222ff");
  
  image->strokeColor("#000000");   // Outline color 
  image->fillColor("#000000");     // color 

  this->fiberColor = Color("#d5d5d5");

  image->strokeWidth(2);
  image->draw( DrawableRectangle(0,0,0,height) );
  image->draw( DrawableRectangle(width,0,width,height) );
  image->draw( DrawableRectangle(0,0,width,0) );
  image->draw( DrawableRectangle(0,height,width,height) );

  for ( uint i=0; i<N; i++ ) {
    for ( uint j=0; j<n; j++ )
      this->update(i, j);
  }

  *base = *image;
  this->clear();
  this->fiberColor = Color(this->fiberColorString);

  return;

}

uint face::getType( void ) {
  return this->type;
}

void face::setType( uint which ) {

  if ( which < 1 || which > 4 )
    return;

  bool redraw = false;
  if ( (type % 2) != (which%2) )
    redraw = true;

  this->type = which;

  if ( redraw ) {

    this->LoadImage();
    drawFacePlate();

    this->invalidate();
  }

  return;
}
