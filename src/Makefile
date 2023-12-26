

COMPTYPE  := debug

EXE=step-1-clean-raw-logs-2023

CXX 	  := g++
INCDIR    := ../include

CXXFLAGS  := -std=c++2b -O2 -I. -Wall -Wextra -Werror -Wpedantic -Wshadow
CXXFLAGS  += -Wconversion -Wsign-conversion -Wunreachable-code
CXXFLAGS  += -Weffc++ -Wmissing-braces -Wnon-virtual-dtor -Winline
CXXFLAGS  += -Wnormalized -Wold-style-cast -Wrange-loop-construct
CXXFLAGS  += -Wreturn-local-addr -Wredundant-move -Wsuggest-final-types
CXXFLAGS  += -Wsuggest-override -Wvirtual-inheritance -Wvirtual-move-assign
CXXFLAGS  += -Wuninitialized -Wswitch-enum -Wswitch
CXXFLAGS  += -DIOSTREAMINPUT
INCFLAGS  := -I$(INCDIR)
LDLIBS    := -lfmt

ifeq ($(COMPTYPE), debug)
	# CXXFLAGS += -fsanitize=address -fsanitize=undefined
	# CXXFLAGS  += -g -pg
	CXXFLAGS  += -DSAMPLE
endif

SRCS      := step-1-clean-raw-logs-2023.cpp glob.cpp
OBJS      := $(subst .cpp,.o,$(SRCS))

.PHONY: all clean

all: $(EXE)
	cp $(EXE) ../

$(EXE): $(OBJS) -lfmt
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCFLAGS) $(LDLIBS)

$(EXE).o: $(EXE).cpp
	$(CXX) -c $< $(CXXFLAGS) $(INCFLAGS) $(LDLIBS)

glob.o: glob.cpp
	$(CXX) -c $< -O2 $(INCFLAGS)

clean:
	rm -f *.o
	rm -f $(EXE)
