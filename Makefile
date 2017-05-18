CXX = $(shell root-config --cxx)
LD = $(shell root-config --ld)

INC = $(shell pwd)
Repo = $(shell git rev-parse --show-toplevel)

CPPFLAGS := $(shell root-config --cflags)
LDFLAGS := $(shell root-config --glibs) $(STDLIBDIR) -lRooFit -lRooFitCore

CPPFLAGS += -g -std=c++11

TARGET = Dat2RootV5
TARGET1 = ConvertDat2Root_AndTimingAnalysis

SRC  = ConvertDat2Root_v5.cc
SRC1 = ConvertDat2Root_AndTimingAnalysis.cc

OBJ = $(SRC:.cc=.o)
OBJ1 = $(SRC1:.cc=.o)

all : $(TARGET) $(TARGET1)

$(TARGET) : $(OBJ)
	$(LD) $(CPPFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

$(TARGET1) : $(OBJ1)
	$(LD) $(CPPFLAGS) -o $(TARGET1) $(OBJ1) $(LDFLAGS)
	@echo $@
	@echo $<
	@echo $^

%.o : %.cc
	$(CXX) $(CPPFLAGS) -o $@ -c $<
	@echo $@
	@echo $<


clean :
	rm -f *.o app/*.o src/*.o $(CommonTools)/src/*.o $(Aux)/src/*.o $(TARGET) $(TARGET1)
