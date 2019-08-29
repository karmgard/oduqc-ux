#include <serial.h>
using namespace std;

//#define TEST

#ifndef TEST
#include "MyFrame.h"
#else
#include "frame.h"
#endif

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6) {
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();
}

static void randomize( void );
char state[256];          // State array for the random number seed

// Keep a pointer to this class so we can call a class member to handle SIGIO
static serial *self;

// Timer variables & callback
static timer_t firstTimerID;

static void timerHandler( int sig, siginfo_t *si, void *uc ) {
  timer_t *tidp = (timer_t *)si->si_value.sival_ptr;
  static int firstCounter = 0;

  if ( *tidp == firstTimerID ) {
    string id = self->getID();

    firstCounter++;

    if ( firstCounter >= 4 ) {
      if ( self->getVerbose() )
	cout << "Too many ID queries. Abandoning\n";
      if ( firstTimerID != 0 )
	timer_delete( firstTimerID );

      self->setHardwareReady(false);

      return;
    }

    if ( id != "ODUQC" && id != "QCBot" && id != "dummy" ) {
      if ( self->getVerbose() )
	cout << "Nonsense reply -- " << id << " -- to ID query. Try #" 
	     << firstCounter << "\n";
      self->send("i");
    } else if ( firstTimerID != 0 )
      timer_delete( firstTimerID );
  }

  return;
}

// SIGIO callback for exchanging data on the serial line
static void callback( int status ) {
  if ( self ) {
    self->read_port();
  }
  else
    cout << "self not defined!\n";

  return;
}

/* Table of hex values in decimal by ASCII character value */
static const int hextable[] = {
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // bit aligned access into 
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // the table is much faster 
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, // on most modern processors.
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1, // For space reduce to a 
  -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, // signed char.
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

/** 
 * @brief convert a hexidecimal string to a signed long
 * will not produce or process negative numbers except 
 * to signal error.
 * 
 * @param hex without decoration, case insensitive. 
 * 
 * @return -1 on error, or result (max (sizeof(long)*8)-1 bits)
 */
/***
inline int hexdec(unsigned const char *hex) {
   int ret = 0; 
   while (*hex && ret >= 0) {
      ret = (ret << 4) | hextable[*hex++];
   }
   return ret; 
}
***/

serial::serial ( MyFrame * parent, string devName, unsigned int baud ) {
  this->parent = parent;
  setDevice(devName, baud);
  return;
}

bool serial::setDevice( string devName, unsigned int baud ) {

  this->verbose = p->getInt("VERBOSE");

  if ( verbose>=1 )
    cout << "setting up device " << devName << "\n";

  if (!ifstream(devName)) {
    if ( verbose )
    cout << devName << " does not exist" << endl;

    this->device = "dummy";
    this->fd = 0;  // This actually connects to STDOUT
    this->baudRate = 115200;

    // Init the random number generator
    randomize();

    //
    parent->setHardwareReady(false);

    return true;
  }

  this->first = true;
  this->device = devName;
  this->baudRate = baud;
  this->fd = this->setupSerialPort();
  this->ID = "Waiting";

  // Flush the port to get rid of any junk in there
  if ( fd )
    read_port();

  // And save a pointer to ourownsweetselves
  self = this;

  // Ask for an ID from the board
  send("i");

  return true;
}

serial::serial( MyFrame * parent ) {
  this->parent = parent;
  setDummyDevice();
  return;
}

serial::~serial() {

  while ( !cleanup )
    sleep(1);

  if ( verbose>=1 )
    cout << "All done, " << this->ID << " cleaned up\n";

  if ( fd > 0 ) {
    tcflush(fd, TCOFLUSH);
    close(fd);
  }

  return;
}

void serial::setHardwareReady( bool isReady ) {
  parent->setHardwareReady(isReady);
  return;
}

void serial::setDummyDevice( void ) {

  this->verbose = p->getBool("VERBOSE");

  this->device = "dummy";
  this->fd = 0;  // This actually connects to STDOUT
  this->baudRate = 9600;
  this->ID = "dummy";

  // Init the random number generator
  randomize();

  return;
}

string serial::getID(void) {
  return ID;
}
void serial::setID( string identity ) {
  this->ID = identity;
  return;
}

void serial::read_port( void ) {

  int c=0, n=0, counter = strlen(buf);

  tcflush(this->fd, TCOFLUSH);
  while ( (n=read(this->fd,&c,1)) ) {
    if ( n != 1 ) {
      perror(this->device.c_str());
      errno = 0;
      break;
    }
    buf[counter++] = c;
    if ( counter > 225 ) {
      // If there's a lot of crap on the line.... flush it
      memset( buf, '\0', bufferSize * sizeof(char) );
      tcflush(this->fd, TCOFLUSH);
      return;
    }
    tcflush(this->fd, TCOFLUSH);
  }

  static uint lastStamp = 0;

  if ( c == '\n' ) {
    counter--;
    while ( buf[counter] == '\n' || buf[counter] == '\r' )
      buf[counter--] = '\0';

    uint code = 0x0;
    char * cPtr = 0x0;
    if ( strlen(buf) > 3 ) {
      code  = hextable[(unsigned int)buf[0]]<<4 | hextable[(unsigned int)buf[1]];    
      cPtr = &buf[3];
    }
    else {
      // And clear the buffer for the next time we read the line
      memset( buf, '\0', bufferSize * sizeof(char) );
      // And bail
      return;
    }
    // We've got what we need, clear the line in case we get
    // called back to read the port before we're done here.
    tcflush(this->fd, TCOFLUSH);

    if ( code > 0xff ) {
      // And clear the buffer for the next time we read the line
      memset( buf, '\0', bufferSize * sizeof(char) );
      // And bail
      return;
    }

    // Clean up the results from the serial read.
    cleanResponse(cPtr);

    if ( verbose > 1 )
      cout << "Got code 0x" << std::hex << code << " " << cPtr << "\n";
    
    /***********************/
    if ( code==0xc1 || code==0xc2 || code==0x4 ) {
      uint stamp = (uint)atoi(&buf[3]);
      if ( stamp == lastStamp ) {
	//if ( verbose > 1 )
	  cout << "Dup code 0x" << std::hex << code << " " << cPtr << "\n";
	memset( buf, '\0', bufferSize * sizeof(char) );
	return;
      }
      lastStamp = stamp;
      cPtr = &buf[8];
    }
    /***********************/

    switch (code) {

    case 0xc2:                 // PhotoDiode readings
      parent->updateGraph(cPtr+3);
      break;

    case 0xc1:                 // LED # lit
      parent->updateFace( atoi( cPtr+4 ) );
      break;
      
    case 0x04:                 // sequencing Done for this connector
      parent->continueQC();
      break;

    case 0x00:                 // Ready, program loaded & running
      send("i");
      break;

    case 0x01:                 // ID (= QCBot, or ODUQC)
      if ( ID_REQUESTED ) {

	ID = string(cPtr);
	if ( ID == "QCBot" || ID == "ODUQC" ) {

	  ID_REQUESTED = false;

	  // If the RCP file says to use the robot, make
	  // sure it's actually there before attempting it
	  if ( p->getBool("USE_ROBOT") )
	    p->setBool("USE_ROBOT", (ID=="QCBot"));

	  if ( ID == "ODUQC" )
	    p->setBool("USE_ROBOT", false);

	  // If we're not going to use the robot, we can't use full auto
	  //if ( !p->getBool("USE_ROBOT") ) {
	  //p->setBool("FULL_AUTO", false);
	  //parent->autoRun_Changed(false);
	  //}
	  
	  // If we got a good response remove the timer
	  timer_delete(firstTimerID);

	  if ( first ) {
	    // If we're going to be using the robot.... wake it up
	    if ( p->getBool("USE_ROBOT") )
	      send("G" + to_string(p->getUInt("ODU_TYPE")));
	    // Otherwise start initializing the controller card
	    else
	      send("S " + to_string(p->getUInt("NUMBER_OF_SAMPLES")));
	  }
	}
      }
	
      break;
      
    case 0x02:                 // Lights out All done
      report(cPtr);
      cleanup = true;
      break;
      
    case 0x03:                 // POST complete
      report(cPtr);
      break;

    case 0x05:                 // Set background subtraction
      report(cPtr);
      if ( first )
	send("D" + to_string(p->getUInt("PER_FIBER_DELAY")));

      break;

    case 0x06:                 // Set number of channels
      report(cPtr);
      if ( first )             // Send background subtraction
	send("b " + to_string(p->getBool("SUBTRACT_NOISE")));

      break;

    case 0x07:                 // Set sample size
      report(cPtr);
      if ( first )             // Send number of channels
	send("n " + to_string(p->getUInt("NUMBER_OF_CHANNELS")));
      break;

    case 0x11:                 // Set the per fiber delay time
      report(cPtr);
      if ( first ) {
	first = false;
	parent->setHardwareReady(true);
      }
      break;
      
    case 0x08:                 // Motors arrived at home (Motors parked)
      report(cPtr);
      break;
      
    case 0x09:                 // Requested move complete
      parent->moveComplete();
      break;

    case 0xa1:                 // Turn the robot on
      report(cPtr);

      if ( first ) {
	parent->autoRun_Changed(true);
	send("S " + to_string(p->getUInt("NUMBER_OF_SAMPLES")));
      }

      break;

    case 0xa2:                 // Turn the robot off
      report(cPtr);
      parent->autoRun_Changed(false);
      break;
      
    case 0x10:                 // ODU type set
      report(cPtr);
      break;
      
    case 0xc3:                 // Calibrating
      report(cPtr);
      break;

    case 0xc4:                 // Calibration is done
      parent->updateNorms(cPtr);
      break;

    case 0xfd:                 // Panic button triggered
      report(cPtr);
      parent->PanicButton();
      break;

    case 0xf3:                 // Fatal Error occured
      report(cPtr);
      parent->FatalError();

    case 0xff:                 // Fatal Error cleared
      report(cPtr);
      parent->FatalErrorCleared();
      break;
      
    default:
      report(cPtr);
      break;
      
    } // End switch (code)

    // And clear the buffer for the next time we read the line
    memset( buf, '\0', bufferSize * sizeof(char) );

  } // End if ( c == '\n' )

  return;
}

void serial::cleanResponse( char * message ) {

  // Allocate a buffer for cleaning up the message, just in case
  char * clean = new char [strlen(message)+1];
  int counter = 0;

  // Step one, get rid of the funky characters
  for ( uint i=0; i<strlen(message); i++ ) {
    if ( message[i] == '\n' || message[i] == '\r' || message[i] == '\0' )
      break;
    if ( message[i] > 31 && message[i] < 127 )
      clean[counter++] = message[i];
  }
  clean[counter] = '\0';
  strcpy(message, clean);
  memset( (void *)clean, 0, sizeof clean);
  counter = 0;

  // Step two, loop through and see if this is an ID we recognize
  for ( uint i=0; i<strlen(message); i++ ) {
    bool goodChar = (message[i] == 'O') || (message[i] == 'D') || 
      (message[i] == 'U') || (message[i] == 'Q') || (message[i] == 'C');

    if ( goodChar )
      clean[counter++] = message[i];

  } // End for ( uint i=0; i<strlen(clean); i++ )
  clean[counter] = '\0';

  if ( !strncmp(message,"ODUQC", 5) ) {
    strcpy(message, clean);
    delete [] clean;
    return;
  }

  delete [] clean;
  return;
}

void serial::report( char * status, bool send_to_statbar ) {

  if ( !status )
    return;

  const uint size = ID.length() + strlen(status) + 24;

  char stat[size];
  stat[0] = '\0';

  if ( strlen(ID.c_str()) > 2 )
    strcat(stat, ID.c_str());
  else
    strcat(stat, device.c_str());

  if ( strlen(status) ) {
    strcat(stat, " says ");    
    strcat(stat, status);
  }

  if ( strlen(stat) ) {
    if ( verbose>=1 )
      cout << stat << "\n";

    if ( send_to_statbar && strlen(stat) > 5 )
      parent->setStatus(stat);
  }
  return;
}

void serial::fake_arduino(void) {

  uint c = p->getUInt("NUMBER_OF_CHANNELS");
  char results [128];

  for ( uint i=0; i<c; i++ ) {
    parent->updateFace(i);
#ifndef TEST
    wxYield();
#endif
    usleep( 100000 );

    float t = 1024 * randf();       // SAPD
    float q = 0.975 * t * (randf()+.333);  // LAPD

    float r = 0.1 * t * randf();    // SA Std
    float s = 0.1 * q * randf();    // LA Std

    sprintf(results, "%.2f(%.2f)/%.2f(%.2f)", q, s, t, r);

    if ( verbose > 1 )
      cout << "PD " << results << "\n";

    parent->updateGraph(results);

#ifndef TEST
    wxYield();
#endif
    usleep( 100000 );
  }

  parent->continueQC();

  return;
}

int serial::send( string message ) {

  if ( verbose>=1 ) {
    if ( ID != "" )
      cout << "Sending " << message << " to " << ID << "\n";
    else
      cout << "Sending " << message << " to " << device << "\n";
  }

  string msg = message + "\n\r";
  unsigned int n = write(this->fd, msg.c_str(), msg.length());
  if ( n != msg.length() ) {
    cout << "Write failed!\n";
    return -1;
  }
  
  // We're requesting an ID from the board
  if ( message == "i" ) {
    this->ID = "";
    this->ID_REQUESTED = true;

    // Set an alarm to check the identity if there isn't one already
    if ( firstTimerID == NULL )
     makeTimer(&firstTimerID, 5,2);
  }
  else if ( message == "Q" ) {
      cleanup = true;

  } // End else if ( message == "Q" )

  if ( fd == 0 ) {
    cout << endl;

  if ( message == "s" )
    fake_arduino();

  else if ( message == "c" )
    parent->FatalErrorCleared();

  } // End if ( fd == 0 )
  return n;
}

int serial::setupSerialPort(void) {

  if ( !this->device.length() )
    return -1;

  struct sigaction saio;   /* definition of signal action for SIGIO */
  struct termios toptions;
  int fd;

  // Open the device duplex, non-blocking mode
  fd = open(this->device.c_str(), O_RDWR | O_NONBLOCK );

  if (fd == -1)  {
    perror("serialport_init: Unable to open port ");
    return -1;
  }
  
  // install the signal handler before making the device asynchronous
  saio.sa_handler = callback;
  saio.sa_flags = SA_RESTART;
  saio.sa_restorer = NULL;

  if ( saio.sa_handler != 0x0 ) 
    sigaction(SIGIO,&saio,NULL);

  // allow the process to receive SIGIO asynchronously
  fcntl(fd, F_SETOWN, getpid());
  fcntl(fd, F_SETFL,  FASYNC);
  fcntl(fd, F_SETSIG, SIGIO);

  if (tcgetattr(fd, &toptions) < 0) {
    perror("serialport_init: Couldn't get term attributes");
    return -1;
  }

  cfsetispeed(&toptions, this->baudRate);
  cfsetospeed(&toptions, this->baudRate);

  // 8N1
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;

  // no flow control
  toptions.c_cflag &= ~CRTSCTS;

  // disable hang-up-on-close to avoid reset
  toptions.c_cflag &= ~HUPCL;

  toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
  toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

  toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
  toptions.c_oflag &= ~OPOST; // make raw

  // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
  toptions.c_cc[VMIN]  = 0;
  toptions.c_cc[VTIME] = 0;
    
  tcsetattr(fd, TCSANOW, &toptions);
  if( tcsetattr(fd, TCSAFLUSH, &toptions) < 0) {
    perror("init_serialport: Couldn't set term attributes");
    return -1;
  }

  // Return the file descriptor to the main program
  return fd;

}
static void randomize( void ) {
  unsigned long int seed = 0; //p->getLong("SEED");
  int filedes = 0;

  errno = 0;
  if ( (filedes=open("/dev/urandom", O_RDONLY)) > 0 ) {
    ssize_t res = read(filedes, (void *)&seed, sizeof(unsigned long int));
    if ( !res ) {
      perror("Unable to seed random number generator!\n");
      exit(errno);
    }
    close(filedes);
  } else {
    perror("Unable to seed random number generator!\n");
    exit(errno);
  }

  initstate( seed, (char *)state, 256);
  srandom(seed);

  return;
}

void serial::makeTimer( timer_t *timerID, int expire, int interval ) {
  struct sigevent te;
  struct itimerspec its;
  struct sigaction sa;
  int sigNo = SIGRTMIN;

  /* Set up signal handler. */
  sa.sa_flags = SA_SIGINFO | SA_RESTART;
  sa.sa_sigaction = timerHandler;
  sigemptyset(&sa.sa_mask);

  if (sigaction(sigNo, &sa, NULL) == -1)
    perror("sigaction");

  /* Set and enable alarm */
  te.sigev_notify = SIGEV_SIGNAL;
  te.sigev_signo = sigNo;
  te.sigev_value.sival_ptr = timerID;
  timer_create(CLOCK_REALTIME, &te, timerID);

  its.it_interval.tv_sec = interval;
  its.it_interval.tv_nsec = 0; //intervalMS * 1000000;
  its.it_value.tv_sec = expire;
  its.it_value.tv_nsec = 0; //expireMS * 1000000;
  int retVal = timer_settime(*timerID, 0, &its, NULL);
  if ( retVal )
    perror("timer_settime() failed");

  return;
}
