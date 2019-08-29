#ifndef __PARAMETERS_H
#define __PARAMETERS_H 1

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef __DARWIN__
  // gcc on Apple puts unordered_map in an unstandard place
  #include <tr1/unordered_map>
  using namespace tr1;

  // Type defs for gcc on Darwin
  typedef unsigned int uint;
  typedef unsigned long ulong;
  typedef unsigned short int ushort;

#else
  #include <unordered_map>
#endif

#include <vector>

#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_UL_INT 0xffffffff
#define MAX_INT    0x7fffffff

using namespace std;

#define SSTR( x ) static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x )).str()

#define FSTR( x ) static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
							setiosflags(ios::fixed) << \
							std::setprecision(1) << x )).str()

union Values {

  char asChar;
  short asShort;
  unsigned short asUShort;
  long asLong;
  unsigned long asULong; 
  bool asBool;
  int asInt;
  float asFloat;
  double asDouble;
  char* asCString;
  unsigned int asUInt;
  float *asFloatArray;
  double *asDoubleArray;
  int *asIntArray;
  uint *asUIntArray;

  Values() { asULong = 0; }
  Values(unsigned char in) { asChar = in; }
  Values(short in) { asShort = in; }
  Values(unsigned short in) { asUShort = in; }
  Values(long in) { asLong = in; }
  Values(unsigned int in) { asUInt = in; }
  Values(unsigned long in) { asULong = in; }
  Values(bool in) { asBool = in; }
  Values(int in) { asInt = in; }
  Values(float in) { asFloat = in; }
  Values(double in) { asDouble = in; }
  Values(char* in) { asCString = in; }
  Values(float *in) { asFloatArray = in; }
  Values(double *in) { asDoubleArray = in; }
  Values(uint *in) { asUIntArray = in; }
  Values(int *in) { asIntArray = in; }

  operator char() { return asChar; }
  operator short() { return asShort; }
  operator unsigned short() { return asUShort; }
  operator long() { return asLong; }
  operator unsigned int() { return asUInt; }
  operator unsigned long() { return asULong; }
  operator bool() { return asBool; }
  operator int() { return asInt; }
  operator float() { return asFloat; }
  operator double() { return asDouble; }
  operator char*() { return asCString; }
};

typedef unordered_map <string, Values> paramMap;

class parameters {

 public:

  parameters( string );
  ~parameters( void );

  // Debugging
  void    dumpKeys       (  void  );

  // Basic is it there check
  bool    defined        ( string );

  // Getters for various types
  ulong   getULong       ( string );
  bool    getBool        ( string );
  int     getInt         ( string );
  int    *getIntArray    ( string );
  float   getFloat       ( string );
  float  *getFloatArray  ( string );
  double  getDouble      ( string );
  double *getDoubleArray ( string );
  string  getString      ( string );
  char   *getCString     ( string );
  uint    getLong        ( string );
  uint    getUInt        ( string );
  uint   *getUIntArray   ( string );
  char    getChar        ( string );
  short   getShort       ( string );
  ushort  getUShort      ( string );

  // Setters for the various types
  void    setUInt        ( string, uint );
  void    setLong        ( string, long );
  void    setInt         ( string, int );
  void    setChar        ( string, char );
  void    setShort       ( string, short );
  void    setUShort      ( string, ushort );
  void    setULong       ( string, ulong );
  void    setBool        ( string, bool );
  void    setFloat       ( string, float );
  void    setDouble      ( string, double );
  void    setString      ( string, char * );
  void    setString      ( string, string );
  void    setCString     ( string, string );
  void    setIntArray    ( string, vector<int> );
  void    setUIntArray   ( string, vector<uint> );
  void    setFloatArray  ( string, vector<float> );
  void    setDoubleArray ( string, vector<double> );

  bool    setFloatArray  ( string, float *, uint=0 );
  bool    setDoubleArray ( string, float *, uint=0 );

  void    flush          ( string="", string="" );
  void    flushOne       ( string );
  string  toString       ( string, string="" );

  string getInfile       ( void ) { return this->inFile; }

 protected:

 private:
  paramMap pMap;
  vector<string> allocated_arrays;
  string inFile;
};

#endif
