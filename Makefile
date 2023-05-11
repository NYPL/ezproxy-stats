
OUTPUTFILE=step-1-clean-raw-logs-2023

CXX = g++

INC_DIR = ./include
CXXFLAGS = -Wall -std=c++17 -I$(INC_DIR)

.PHONY: all
all: $(OUTPUTFILE)

$(OUTPUTFILE): step-1-clean-raw-logs-2023.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)


