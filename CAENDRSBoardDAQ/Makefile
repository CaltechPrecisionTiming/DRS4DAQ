
CC            = gcc
CXX           = g++
LD            = g++
OutPutOpt     = -o # keep whitespace after "-o"


CXXFLAGS      = -O2 -Wall -fPIC
LDFLAGS       = -O2
SOFLAGS       = -shared



CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)


#------------------------------------------------------------------------------

pro           = caen-01

OBJS          = $(pro).o

#------------------------------------------------------------------------------


KERNELREV:=$(shell uname -r)

INCLUDES_C =   /usr/include/vme /lib/modules/$(KERNELREV)/build/include

LIBS2  = -Llib -lvme


#------------------------------------------------------------------------------

all:            $(pro)


$(pro):       $(OBJS)
	$(LD)  $(OBJS) $(LIBS) $(GLIBS) $(LIBS2) -o $(pro)



clean:
	@rm -f *.o core.* *~

#------------------------------------------------------------------------------



%.o:%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES_C:%=-I%) -c $<

%.o:%.c
	$(CC) $(CXXFLAGS) $(INCLUDES_C:%=-I%) -c $<


