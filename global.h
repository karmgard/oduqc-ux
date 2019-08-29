#ifndef __GLOBAL_H
#define __GLOBAL_H

#define ADC     0
#define PERCENT 1

#define STATE_RUNNING 1
#define STATE_PAUSED  2
#define STATE_STOPPED 4
#define STATE_ERROR   8
#define STATE_CANCEL  16
#define STATE_PASSED  32
#define STATE_FAILED  64
#define STATE_WARNING 128
#define STATE_RERUN   256

#include "parameters.h"
extern parameters *p;     // Parameter structure declared in main

#endif
