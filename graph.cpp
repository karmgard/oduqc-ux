#include "graph.h"

using namespace Magick;
using namespace std;

graph::graph(wxWindow *parent, wxWindowID wid, std::string file) {
  this->file = file;
  if ( this->file == "No Such String" ) {
    this->file = "graph";
    this->fromFile = false;
  } else
    this->fromFile = true;

  width  = p->getUInt("gWIDTH");
  height = p->getUInt("gHEIGHT");

  this->Create(parent, wid);
  this->LoadImage();

  this->fm = p->getFloat("FRACTION_MARGIN");
  this->fs = p->getFloat("FRACTION_SHADOW");
  this->fg = p->getFloat("FRACTION_GAP");
  this->ch = p->getFloat("CHART_HEIGHT");

  this->fh = p->getFloat("FRACTION_HEIGHT");
  this->n  = p->getUInt("NUMBER_OF_CHANNELS");

  this->warn = p->getFloat("WARNING_LIMIT");
  this->fail = p->getFloat("FAILURE_LIMIT");

  this->passColor  = Color(p->getString("COLOR_PASS"));
  this->warnColor  = Color(p->getString("COLOR_WARN"));
  this->failColor  = Color(p->getString("COLOR_FAIL"));
  this->shdwColor  = Color(p->getString("COLOR_SHADOW"));

  this->offset = p->getFloat("OFFSET");

  image->strokeWidth(1);           // No border
  image->strokeColor(shdwColor);   // Outline color 

  margin = fm*width;

  xb = (0.99*width - 2*margin - offset*width) / ( n * ( 1 + fs + fg ) - fg );
  dx = (1 + fs + fg) * xb;
  
  if ( p->getString("GRAPH_STYLE") == "ADC" )
    setGraphType(ADC);
  else
    setGraphType(PERCENT);

  hasUpdate = false;
  
  return;
}

void graph::updateWarningLimit( float warning ) {
  if ( warning >= 0.0f && warning <= 1.0f )
    this->warn = warning;
  return;
}

void graph::updateFailureLimit( float failure ) {
  if ( failure >= 0.0f && failure <= 1.0f )
    this->fail = failure;
  return;
}

void graph::setADCSignalScheme(void) {
  passColor = warnColor = failColor = Color(p->getString("COLOR_SIGNAL"));
  shdwColor = Color("#ffffff00");
  return;
}

void graph::setADCCalibrationScheme(void) {
  passColor = warnColor = failColor = Color(p->getString("COLOR_CALIB"));
  shdwColor = Color("#ffffff00");
  return;
}

void graph::setPercentageScheme(void) {
  this->passColor  = Color(p->getString("COLOR_PASS"));
  this->warnColor  = Color(p->getString("COLOR_WARN"));
  this->failColor  = Color(p->getString("COLOR_FAIL"));
  this->shdwColor  = Color(p->getString("COLOR_SHADOW"));

  return;
}

void graph::loadUpdate( uint p1, uint p2 ) {
  parameter1 = p1;
  parameter2 = p2;

  hasUpdate = true;
  
  // Force a refresh/repaint event
  this->invalidate();

  return;
}

// This gets called from OnPaint when loadUpdate has been called
void graph::update(uint channel, uint percent) {

  float value = 0.01 * percent;

  if ( !image ) {
    cout << "Making new image\n";
    image = new Image(*base);
    if ( !image )
      return;
  }

  if ( value <= 0.0f )
    return;

  try {
    // get a unique lock on the image
    image->modifyImage();
  }
  catch (Exception e) {
    cout << "Unable to get exclusive lock on the image!\n";
    return;
  }
  
  if ( value < 0.0f )
    value = 0.0f;
  if ( value > 1.0f )
    value = 0.999f;

  int x1 = round(offset *width + margin + channel * dx);
  int x2 = round(x1 + xb);

  try {
    if ( value > warn )
      image->fillColor(passColor);     // Fill color 
    else if ( value > fail )
      image->fillColor(warnColor);
    else
      image->fillColor(failColor);
  }
  catch (Exception e) {
    cout << "Caught exception trying to set fill color\n";
  }
  
  int y1 = round(height - ch * value * height);

  try {
    // Draw the image on the canvas
    image->strokeWidth(0);           // no border
    image->draw( DrawableRectangle(x1,y1,x2,0.99*height) );
  }
  catch (Exception e) {
    cout << "Caught exception trying to draw ("
	 << x1 << ", " << y1 << ", " << x2 << ", " << 0.99*height << ") on image\n";
  }
  // Rescale to draw a shadow
  x1  = x2;
  x2  = x1 + fs*xb;

  y1 = round(height - ch * fh * value * height);

  try {
    image->strokeColor(shdwColor);   // Outline color 
    image->fillColor(shdwColor);     // Fill color 
    image->draw( DrawableRectangle(x1,y1,x2,0.99*height) );
  }
  catch (Exception e) {
    cout << "Caught exception trying to draw shadow at ("
	 << x1 << ", " << y1 << ", " << x2 << ", " << 0.99*height << ") on image\n";
  }

  hasUpdate = false;
  
  return;
}

void graph::setGraphType(int which) {

  graphType = which;

  // Clear the image
  image->fillColor(bkgColor);     // Fill color 
  image->draw( DrawableRectangle(0, 0, width, height) );

  if (which != ADC)
    graduatePercent();
  else
    graduateADC();
  return;
}

void graph::graduatePercent(void) {

  int x,y;

  int x0 = offset*width;
  int xf = 0.99*width;

  image->strokeColor("#000000");   // Outline color 
  image->fillColor("#000000");     // color 

  image->strokeWidth(2);

  image->draw( DrawableRectangle(0,0,0,height) );
  image->draw( DrawableRectangle(width,0,width,height) );
  image->draw( DrawableRectangle(0,0,width,0) );
  image->draw( DrawableRectangle(0,height,width,height) );

  char p[5];
  image->strokeWidth(1);
  for ( int i=1; i<=10; i++ ) {
    y = round(height * (1 - (ch * .1 * i)));

    image->draw( DrawableRectangle(x0,y,xf,y) );

    sprintf(p, "%i%%", 10*i);
    image->annotate( p, Geometry( 1,1, 0.15*x0, y ) );

  }
  image->draw( DrawableRectangle(x0,(1-ch)*height,x0,0.99*height) );
  image->draw( DrawableRectangle(xf,(1-ch)*height,xf,0.99*height) );
  image->draw( DrawableRectangle(x0,0.99*height,xf,0.99*height) );

  for ( uint i=1; i<=n; i++ ) {
    x = round(offset * width + margin + (i-1) * dx );
    image->annotate( to_string(i), Geometry( 4,4, x, 0.025*height ) );
  }
  *base = *image;
  this->clear();

  return;
}
void graph::graduateADC(void) {

  int x,y;

  int x0 = offset*width;
  int xf = 0.99*width;

  image->strokeColor("#000000");   // Outline color 
  image->fillColor("#000000");     // color 

  image->strokeWidth(2);

  image->draw( DrawableRectangle(0,0,0,height) );
  image->draw( DrawableRectangle(width,0,width,height) );
  image->draw( DrawableRectangle(0,0,width,0) );
  image->draw( DrawableRectangle(0,height,width,height) );

  // Annotate light level on the left y-axis
  char p[5];
  image->strokeWidth(1);
  for ( int i=1; i<=8; i++ ) {
    y = round(height * (1 - (ch * .125 * i)));

    image->draw( DrawableRectangle(x0,y,xf,y) );

    sprintf(p, "%i", 128*i);
    image->annotate( p, Geometry( 1,1, 0.15*x0, y ) );

  }
  image->draw( DrawableRectangle(x0,(1-ch)*height,x0,0.99*height) );
  image->draw( DrawableRectangle(xf,(1-ch)*height,xf,0.99*height) );
  image->draw( DrawableRectangle(x0,0.99*height,xf,0.99*height) );

  // Annotate channel numbers on the top x-axis
  for ( uint i=1; i<=n; i++ ) {
    x = round(offset * width + margin + (i-1) * dx );
    image->annotate( to_string(i), Geometry( 4,4, x, 0.025*height ) );
  }
  *base = *image;
  this->clear();

  return;
}
