
EXE=step-1-clean-raw-logs-2023

CXX = g++
INC_DIR = ../include
# CXXFLAGS = -Wall -std=c++17 -I$(INC_DIR) -O2 -g -pg
CXXFLAGS = -Wall -std=c++17 -I$(INC_DIR) -O2 -lre2
SRCS = step-1-clean-raw-logs-2023.cpp glob.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

.PHONY: all clean

all: $(EXE)
	cp $(EXE) ../

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o
	rm -f $(EXE)
