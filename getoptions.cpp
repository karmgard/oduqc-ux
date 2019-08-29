#include <getopt.h>
#include <unistd.h>
#include <wx/wx.h>

#include "global.h"

/*-- Function prototypes --*/
static string get_cmd_basics ( int, char ** );
static void   get_cmd_full   ( int, char ** );
static void   help           ( void );

/*-- Common switch definitions --*/
static struct option long_options[] = {
  {"test",            no_argument,       NULL,       't'},
  {"verbose",         no_argument,       NULL,       'v'},
  {"use-hardware",    no_argument,       NULL,       'u'},
  {"no-hardware",     no_argument,       NULL,       'U'},
  {"device",          required_argument, NULL,       'd'},

  {"margin",          required_argument, NULL,       'm'},
  {"shadow",          required_argument, NULL,       's'},
  {"gap",             required_argument, NULL,       'g'},
  {"channels",        required_argument, NULL,       'c'},
  {"seed",            required_argument, NULL,       'D'},
  {"chart-max",       required_argument, NULL,       'f'},

  {"width",           required_argument, NULL,       'w'},
  {"height",          required_argument, NULL,       'h'},

  {"parameter_file",  required_argument, NULL,       'f'},
  {"help",            no_argument,       NULL,       'q'},
  {0, 0, 0, 0}
};

static const char * short_options = "tvqm:s:g:c:d:f:w:h:uU";

void get_command_line( int argc, char** argv ) {

  string rcpFile = get_cmd_basics( argc, argv );
  p = new parameters(rcpFile);
  get_cmd_full( argc, argv );

  return;
}

void get_command_line( const int argc, wxChar** wx_argv ) {

  //char ** argv = new char * [argc];
  char * argv[argc];
  for ( int i=0; i<argc; i++ ) {
    
    argv[i] = new char ( wcslen(wx_argv[i])+1 );
    int ret = wcstombs( argv[i], wx_argv[i], wcslen(wx_argv[i]) );
    argv[i][ret] = '\0';

  }

  string rcpFile = get_cmd_basics( argc, argv );
  p = new parameters(rcpFile);
  get_cmd_full( argc, argv );

  //delete argv;

  return;
}

// Brief command line processor to deal with the preliminaries
// The rest of the command line is processed below
static string get_cmd_basics(int argc, char **argv) {

  int c;
  string user = getenv("USER");
  string rcpFile("/home/" + user + "/.config/oduqc/oduqc.rcp");

  /********
#if defined(x86_64_linux_gnu)
  string rcpFile("oduqc.rcp");
#elif defined(arm_linux_gnueabihf)
  string rcpFile("/home/pi/.config/oduqc/oduqc.rcp");
#else
#  warning "No architecture defined! Defaulting to ./oduqc.rcp"
  string rcpFile("./oduqc.rcp");
#endif
  *************/

  /* getopt_long stores the option index here. */
  int option_index = 0;

  // No unknown index errors this time through
  opterr = 0;

  int counter = 1;

  while (1) {
    c = getopt_long (argc, argv, short_options, long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
    case 0:  /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
	break;

    case 'q':
      help();
      break;

    case 'f':
      rcpFile = (string)optarg;
      break;

    default:
      break;
    }               // end switch(c)

    counter++;
  }                 // end while(1)
  //---------------------------------------------------------------------------

  // Rest the error and index options for a 
  // second call to process the rest of the 
  // arguments
  opterr = 1;
  optind = 1;

  return rcpFile;
} // End get_cmd_basics(int argc, char **argv)

static void get_cmd_full(int argc, char **argv) {

  int c;

  /*-- getopt_long stores the option index here. --*/
  int option_index = 0;

  while (1) {
    c = getopt_long (argc, argv, short_options, long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
    case 0:  /* If this option set a flag, do nothing else now. */
      if (long_options[option_index].flag != 0)
	break;

    case 't':
      cout << "This is a test\n";
      break;

    case 'v':
      p->setBool("VERBOSE", true);
      break;

    case 'u':
      p->setBool("USE_HARDWARE", true);
      break;
      
    case 'U':
      p->setBool("USE_HARDWARE", false);
      break;

    case 'm':
      break;

    case 's':
      break;

    case 'D':
      p->setLong("SEED", atol(optarg));
      break;

    case 'g':
      break;

    case 'c':
      break;

    case 'd':
      p->setString("DEVICE_NAME", optarg);
      break;

    case 'x':
      break;

    case 'w':
      p->setUInt("wxWIDTH", (uint)abs(atoi(optarg)));
      break;

    case 'h':
      p->setUInt("wxHEIGHT", (uint)abs(atoi(optarg)));
      break;

    default:
      break;
    }               // end switch(c)
  }                 // end while(1)
  //---------------------------------------------------------------------------

  return;
} // End get_cmd_full(int argc, char **argv)

static void help(void) {

  cout << "Program option summary\n";
  exit(0);      // Exit now... they just wanted some info
  return;
} // End help
