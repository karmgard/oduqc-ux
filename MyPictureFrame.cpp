#include <math.h>
#include <limits.h>

#include "MyPictureFrame.h"

using namespace Magick;
using namespace std;

/************************* Public Functions ************************/
PictureFrame::PictureFrame(string file) : wxWindow(),  bitmap(0,0) {
  this->file = file;
  return;
}

PictureFrame::PictureFrame(wxWindow *parent, wxWindowID wid, string file) : 
  wxWindow(),  bitmap(0,0) {
  this->file = file;
  this->Create(parent, wid);
  this->LoadImage();
  return;
}

PictureFrame::~PictureFrame(void) {
  delete image;
  delete base;
  return;
}

void PictureFrame::SetFile( string f ) { this->file = f; return; }
string PictureFrame::GetFile( void ) { return this->file; }

void PictureFrame::Create( wxWindow *parent, wxWindowID wid ) {
  wxWindow::Create(parent,wid);
  this->parent = parent;

  this->bkgColor = Color(p->getString("COLOR_BACKGROUND"));
  this->fgColor  = Color(p->getString("COLOR_FOREGROUND"));

  return;
}

void PictureFrame::LoadImage(void) {
  bitmap = makeImage();
  SetSize(width,height);
  image = new Image(*base);
  this->invalidate();
  return;
}

void PictureFrame::invalidate(void) { 
  this->Refresh(true);
  return;
}

void PictureFrame::clear(bool update) {

  if ( !base )
    return;

  if ( !image ) {
    image = new Image(*base);
    cout << "Making new image\n";
  }
  else
    *image = *base;
  this->invalidate();

  if ( update )
    this->Update();
  return;
}

/************************* Protected Functions ************************/
void PictureFrame::OnMouse(wxMouseEvent &event) {
  event.ResumePropagation(1); // Pass along mouse events (e.g. to parent)
  event.Skip();
}

void PictureFrame::OnPaint(wxPaintEvent &event) {
  if ( !image )
    return;

  if ( hasUpdate ) {
    update(parameter1, parameter2);
  }

  wxPaintDC dc(this);
  PrepareDC(dc);

  wxImage t_image( width, height );
  unsigned char *buffer = t_image.GetData();

  image->getConstPixels( 0, 0, width, height );
  image->writePixels( MagickCore::RGBQuantum, buffer );

  bitmap = wxBitmap( t_image );

  dc.DrawBitmap(bitmap, 0,0, true);
  return;
}

wxBitmap PictureFrame::makeImage(void) {
  InitializeMagick("");

  if ( fromFile ) {
    base = new Image(file);
    width  = base->columns();
    height = base->rows();
    depth  = base->depth();
  } else {
    if ( !width )
      width = p->getUInt("WIDTH");
    if ( !height )
      height = p->getUInt("HEIGHT");

    base = new Image(Geometry(width,height), bkgColor);
    base->depth(8);
  }
  b = wxBitmap( magickImage(*base) );

  return b;
}

wxImage PictureFrame::magickImage( Image image ) {
  wxImage *t_image = new wxImage( width, height );

  unsigned char *buffer = t_image->GetData();
   
  image.getConstPixels( 0, 0, width, height );
  image.writePixels( MagickCore::RGBQuantum, buffer );

  return *t_image;
}

// Event table for the picture frame class
BEGIN_EVENT_TABLE(PictureFrame,wxWindow)
  EVT_PAINT(PictureFrame::OnPaint)
  EVT_MOUSE_EVENTS(PictureFrame::OnMouse)
END_EVENT_TABLE()
