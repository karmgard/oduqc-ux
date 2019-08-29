#
SHELL=/bin/bash

# Make sure the .dependencies file exists, otherwise the include at the bottom will choke
$(shell touch .dependencies)

SRC=main.cpp MyFrame.cpp MyPictureFrame.cpp parameters.cpp getoptions.cpp serial.cpp graph.cpp face.cpp dialog.cpp message.cpp
HDR=MyApp.h MyFrame.h MyPictureFrame.h parameters.h global.h serial.h graph.h face.h dialog.h message.h
OBJ=$(subst .cpp,.o,${SRC})

TESTSRC=test.cpp parameters.cpp
TESTHDR=
TESTOBJ=$(subst .cpp,.o,${TESTSRC})
TESTBIN=test

LIBSRC=
LIBHDR=
LIBOBJ=$(subst .cpp,.o,${LIBSRC})
LIBBIN=

GCC_VERSION=$(shell g++ -dumpversion)
ARCH=$(shell g++ -v 2>&1 | grep Target | cut -d ' ' -f2)
OS=linux

BACKUP=oduqc.tar.gz
EXTRA=Makefile oduqc.rcp oduqc/* README.nd GPL-2.0 .gitinit

INCLUDEPATHS= -I. $(shell /usr/bin/wx-config --cflags) $(shell Magick++-config --cflags)

LIBSEARCH=-L./ -L/lib/${ARCH} -L${HOME}/lib
LIBRARIES=$(shell /usr/bin/wx-config --libs) $(shell Magick++-config --libs) -lm -lrt -lX11
DEBUG=0

ifeq (${DEBUG},1)
  CPUOPT=-g3 -Wall -pg -fno-strict-aliasing -finline-functions -std=c++11 -Wno-unused-but-set-variable -D$(subst -,_,${ARCH})
  LIBS=${LIBSEARCH} ${LIBRARIES} -pg
else
  CPUOPT=-Wall -mhard-float -fno-strict-aliasing -finline-functions -std=c++11 -D$(subst -,_,${ARCH})
  LIBS=${LIBSEARCH} ${LIBRARIES}
endif

BIN=odu

RCPDIR=$(HOME)/.config/oduqc
RCPFILE=$(HOME)/.config/oduqc/oduqc.rcp

CC=g++ $(CPUOPT) $(INCLUDEPATHS)
LINK=g++ -o $(BIN) $(OBJ) $(LIBS)
LINKTEST=g++ -std=c++11 -o $(TESTBIN) $(TESTOBJ) -lrt

all:	$(BIN)

firmware:
	$(MAKE) -C ./oduqc/

clean:
	rm -f $(BIN) $(OBJ) $(LIBOBJ) $(LIBBIN) $(TESTBIN) $(TESTOBJ) *~ *.bak
	$(MAKE) -C ./oduqc/ clean

tidy:
	rm -f $(BIN) $(OBJ) $(LIBOBJ) $(LIBBIN)

force:	tidy all

${BIN}:	$(OBJ)
	@echo ">>>>>>>>>>>> Linking <<<<<<<<<<<<<"
	$(LINK)
        ifeq (${DEBUG},0)
	  @strip ${BIN} ${LIBBIN}
        endif

.cpp.o:	$(HDR)
	@echo ">>>>>>>>>>>> Compiling $< -> $@ <<<<<<<<<<<<<"
	$(CC) -c $< -o $@

${TESTBIN}: $(TESTHDR) $(TESTSRC) $(TESTOBJ)
	@echo ">>>>>>>>>>>> Linking <<<<<<<<<<<<<"
	$(LINKTEST)
        ifeq (${DEBUG},0)
	  @strip ${TESTBIN}
        endif

install:
	cp $(BIN) $(HOME)/bin
	if [ ! -d $(RCPDIR)  ];then mkdir $(RCPDIR);fi 
	if [ -f $(RCPFILE) ];then mv $(RCPFILE) $(RCPFILE).sav;fi
	cp oduqc.rcp $(RCPFILE);
lib:
	@echo ">>>>>>>>>>>> Making Library <<<<<<<<<<<<<"
	$(CC) -fPIC $(CPUOPT) -c ${LIBSRC}
	$(CC) -shared -o ${LIBBIN} ${LIBOBJ} -pthread 

backup:
	@tar -zcf $(BACKUP) $(SRC) $(HDR) $(LIBSRC) $(EXTRA)

depend dep: $(SRC) $(HDR)
	$(CC) -MM $^ > ./.dependencies;

include .dependencies
