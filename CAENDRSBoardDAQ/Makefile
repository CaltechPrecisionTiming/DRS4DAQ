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
pro1          = CMSTiming
pro2          = CMSTiming3

OBJS          = $(pro).o $(pro1).o $(pro2).o

#------------------------------------------------------------------------------


KERNELREV:=$(shell uname -r)

INCLUDES_C =   /usr/include/vme /lib/modules/$(KERNELREV)/build/include

LIBS2  = -Llib -lvme


#------------------------------------------------------------------------------

all: $(pro) $(pro1) $(pro2)


$(pro): $(pro).o
	$(LD)  $(<) $(LIBS) $(GLIBS) $(LIBS2) -o $(pro)

$(pro1): $(pro1).o 
	$(LD)  $(<) $(LIBS) $(GLIBS) $(LIBS2) -o $(pro1)

$(pro2): $(pro2).o 
	$(LD)  $(<) $(LIBS) $(GLIBS) $(LIBS2) -o $(pro2)


clean:
	@rm -f *.o core.* *~

#------------------------------------------------------------------------------



%.o:%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES_C:%=-I%) -c $<

%.o:%.c
	$(CC) $(CXXFLAGS) $(INCLUDES_C:%=-I%) -c $<
