

CXX = g++
INC_DIR = ../include
CXXFLAGS = -Wall -std=c++17 -I$(INC_DIR)
SRCS = step-1-clean-raw-logs-2023.cpp glob.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

.PHONY: all clean
all: step-1-clean-raw-logs-2023
	cp step-1-clean-raw-logs-2023 ../

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

step-1-clean-raw-logs-2023: $(OBJS)
	$(CXX) -o $@ step-1-clean-raw-logs-2023.o glob.o $(CXXFLAGS)

clean:
	rm *.o
	rm step-1-clean-raw-logs-2023
