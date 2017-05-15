CC=g++
CFLAGS=-c -Wall -Wno-unused-local-typedefs -Wno-deprecated-declarations
INC=-L/usr/share -L/usr/local/lib -I/usr/local/include -I/home/elliott/boost_1_56_0
DOXYGEN = doxygen

all: test
debug: CFLAGS +=  -g -O0 -Wextra
debug: test

test: shellbase.o gaussian.o porphyry.o distbase.o
	$(CC) $(INC) shellbase.o gaussian.o porphyry.o distbase.o -o test -lreadline -lncurses -ltermcap -ldl -lboost_regex -lboost_iostreams
shellbase.o: shellbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++0x shellbase.cpp
gaussian.o: gaussian.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++0x gaussian.cpp
porphyry.o: porphyry.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++0x porphyry.cpp
distbase.o: distbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++0x distbase.cpp
clean:
	rm -rf *o test
docs:
	$(DOXYGEN) Doxyfile
