#ifndef __PICTUREFRAME_H
#define __PICTUREFRAME_H

#include <Magick++.h>
#include <wx/wx.h>

#include <global.h>

class PictureFrame: public wxWindow {

 public:
  PictureFrame(string="");
  PictureFrame(wxWindow *, wxWindowID=-1, std::string="");
  ~PictureFrame(void);

  void Create( wxWindow *, wxWindowID=-1 );
  void LoadImage(void);
  void invalidate(void);
  void flashFibers( void ) ;
  void plotChannels( void );
  void clear( bool=false );
  void SetButtonState( bool );
  bool GetButtonState( void );

  void SetFile( string = "" );
  string GetFile( void );

  virtual void loadUpdate(uint, uint) {;}
  virtual void update(uint, uint) {;}

 protected:
    wxBitmap bitmap;
    wxWindow *parent;
    Magick::Image *image, *base;
    uint width, height, depth;
    bool fromFile;

    std::string file;

    Magick::Color bkgColor; 
    Magick::Color fgColor;

    void OnMouse(wxMouseEvent &);
    void OnPaint(wxPaintEvent &);
    wxBitmap makeImage(void);
    wxImage magickImage( Magick::Image );

    uint parameter1, parameter2;
    bool hasUpdate;

 private:
    DECLARE_EVENT_TABLE()

    wxBitmap b;
};

#endif
