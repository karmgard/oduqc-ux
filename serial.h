#ifndef __SERIAL_H
# define __SERIAL_H 1

#include <string>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <termios.h>  // POSIX terminal control definitions 
#include <string.h>

class MyFrame;        // Forward declaration of the calling class
                      // so we can keep a reference to it

static const float ONE_OVER_RAND_MAX = 1.0f/(RAND_MAX + 1.0f);
static inline float randf( void ) {
  return ONE_OVER_RAND_MAX*random();
}

#include <parameters.h>
extern parameters *p;

#define bufferSize 280

class serial {

 public:
  serial( MyFrame *, std::string, unsigned int=9600 );
  serial( MyFrame * );
  ~serial( void );

  int send( std::string );
  void read_port(void);
  void fake_arduino(void);
  bool setDevice( std::string, unsigned int=9600 );
  std::string getDevice(void) {return this->device;}
  std::string getID(void);
  void setID( std::string );
  void setDummyDevice( void );

  void setCleanup( bool c ) {cleanup=c;}
  bool getCleanup( void   ) {return cleanup;}

  void setVerbose( bool v ) {verbose=v;}
  bool getVerbose( void   ) {return verbose;}

  void setHardwareReady(bool=true);

 protected:

 private:

  void cleanResponse ( char * );
  void makeTimer( timer_t *, int, int );

  MyFrame * parent;

  std::string device;
  int fd;
  int baudRate;
  char buf[bufferSize];
  int setupSerialPort(void);
  void report(char *, bool=true);
  bool first;
  std::string ID;
  bool ID_REQUESTED;
  int verbose;
  bool cleanup;
  uint norm;
  
};

# endif
