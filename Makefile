CC=g++
CFLAGS=-c -Wall -Wno-unused-local-typedefs -Wno-deprecated-declarations
INC=-L/usr/share -L/usr/local/lib -L/usr/lib -I/usr/local/include -I/usr/include/boost
DOXYGEN = doxygen

all: test
debug: CFLAGS +=  -g -O0 -Wextra
debug: test

test: shellbase.o gaussian.o porphyry.o distbase.o
	$(CC) $(INC) shellbase.o gaussian.o porphyry.o distbase.o -o test -lreadline -lncurses -ltermcap -ldl -lboost_regex -lboost_iostreams; \
    ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
shellbase.o: shellbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 shellbase.cpp
gaussian.o: gaussian.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 gaussian.cpp
porphyry.o: porphyry.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 porphyry.cpp
distbase.o: distbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 distbase.cpp
#whorl.o: whorl.cpp
#	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 whorl.cpp
clean:
	rm -rf *o test
docs:
	$(DOXYGEN) Doxyfile
