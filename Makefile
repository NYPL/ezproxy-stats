
EXE=step-1-clean-raw-logs

.PHONY: all clean

all:
	cd src && make

clean:
	rm -f $(EXE)
	cd src && make clean
