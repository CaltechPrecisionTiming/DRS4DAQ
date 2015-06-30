########################################################
#
#  Makefile for drsosc, drscl and drs_exam 
#  executables under linux
#
#  Requires wxWidgets 2.8.9 or newer
#
########################################################

# check if wxWidgets is installed
HAVE_WX       = $(shell which wx-config)
ifeq ($(HAVE_WX),)
$(error Error: wxWidgets required to compile "drsosc")
endif

ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
HASTHREAD    := $(shell $(ROOTCONFIG) --has-thread)
ROOTINC      := $(shell $(ROOTCONFIG) --incdir)

# CFLAGS        = -g -O2 -Wall -Wuninitialized -fno-strict-aliasing -Iinclude -DOS_LINUX -DHAVE_LIBUSB -DUSE_DRS_MUTEX -I$(ROOTSYS)/include
CFLAGS = -g -Wall -Wuninitialized -fno-strict-aliasing -Iinclude -DOS_LINUX -DHAVE_LIBUSB -DUSE_DRS_MUTEX -I$(ROOTCFLAGS)

LIBS         := -lpthread -lutil -lusb $(ROOTLIBS) $(ROOTGLIBS)

# wxWidgets libs and flags
WXLIBS        = $(shell wx-config --libs)
WXFLAGS       = $(shell wx-config --cxxflags)

CPP_OBJ       = DRS.o ConfigDialog.o DOFrame.o DOScreen.o DRSOsc.o MeasureDialog.o Measurement.o Osci.o InfoDialog.o DisplayDialog.o AboutDialog.o EPThread.o TriggerDialog.o rb.o
OBJECTS       = musbstd.o mxml.o strlcpy.o

all: drsosc drscl drs_exam drs_pet drs_ext2 

drsosc: $(OBJECTS) $(CPP_OBJ) main.o
	$(CXX) $(CFLAGS) $(OBJECTS) $(CPP_OBJ) main.o -o drsosc $(LIBS) $(WXLIBS)

drscl: $(OBJECTS) DRS.o drscl.o
	$(CXX) $(CFLAGS) $(OBJECTS) DRS.o drscl.o -o drscl $(LIBS) $(WXLIBS)

drs_exam: $(OBJECTS) DRS.o drs_exam.o
	$(CXX) $(CFLAGS) $(OBJECTS) DRS.o drs_exam.o -o drs_exam $(LIBS) $(WXLIBS)

drs_pet: $(OBJECTS) DRS.o drs_pet.o
	echo $(ROOTLIBS)
	$(CXX) $(CFLAGS) $(ROOTLDGLAGS) $(OBJECTS) DRS.o drs_pet.o -o drs_pet $(LIBS) $(WXLIBS)

drs_ext2: $(OBJECTS) DRS.o drs_ext2.o
	$(CXX) $(CFLAGS) $(OBJECTS) $(GLIBS) DRS.o drs_ext2.o -o drs_ext2 $(LIBS) $(WXLIBS)


main.o: src/main.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c $<

drscl.o: src/drscl.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

drs_exam.o: src/drs_exam.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

drs_pet.o: src/drs_pet.cpp include/mxml.h include/DRS.h
	$(CXX) -v $(CFLAGS) -c $<


drs_ext2.o: src/drs_ext2.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c   $(WXFLAGS)  $(CXXFLAGS) $(INCLUDES_C:%=-I%) $<



$(CPP_OBJ): %.o: src/%.cpp include/%.h include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c $<

$(OBJECTS): %.o: src/%.c include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -f *.o drscl drsosc

