CXX=g++
CC=gcc

CPPFLAGS=-DNDEBUG
CXXFLAGS=-g3 -O2 -Wall -m32


# These flags are necessary to compile correctly; the 'override'
# command makes sure these are added even if the user sets CPPFLAGS on
# the command line.
override CPPFLAGS+=-I../../.. -D_REENTRANT

.PHONY:all clean

BIN=dump_syms

all:$(BIN)

DUMP_OBJ=dump_symbols.o guid_creator.o dump_syms.o file_id.o md5.o

dump_syms:$(DUMP_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

dump_symbols.o:../../../common/linux/dump_symbols.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

guid_creator.o:../../../common/linux/guid_creator.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

file_id.o:../../../common/linux/file_id.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $^

md5.o:../../../common/md5.c
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $^

clean:
	rm -f $(BIN) $(DUMP_OBJ)
