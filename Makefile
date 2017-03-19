CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

INC = $(shell pwd)
Repo = $(shell git rev-parse --show-toplevel)
Aux = $(Repo)/Aux
HggRazor = $(Repo)/HggRazor
CommonTools = $(HggRazor)/CommonTools

CPPFLAGS := $(shell root-config --cflags)
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR) -lRooFit -lRooFitCore

CPPFLAGS += -g -std=c++11

TARGET = Dat2RootV5

SRC = ConvertDat2Root_v5.cc 

OBJ = $(SRC:.cc=.o)


all : $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

%.o : %.cc
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<


clean :
	rm -f *.o app/*.o src/*.o $(CommonTools)/src/*.o $(Aux)/src/*.o $(TARGET)
