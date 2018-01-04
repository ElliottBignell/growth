CC=g++
CFLAGS=-c -Wall -Wno-unused-local-typedefs -Wno-deprecated-declarations
INC=-L/usr/share -L/usr/local/lib -L/usr/lib -I/usr/local/include -I/usr/include/boost -I/usr/lib/x86_64-linux-gnu/
DOXYGEN = doxygen

all: test
debug: CFLAGS +=  -g -O0 -Wextra
debug: test

test: curveofile.o shellbase.o gaussian.o porphyry.o distbase.o xml.o shapes.o surface.o surfacecol.o whorl.o whorldata.o
	$(CC) $(INC) shellbase.o whorldata.o whorl.o surface.o surfacecol.o shapes.o gaussian.o porphyry.o distbase.o xml.o -o test -lreadline -lncurses -ltermcap -ldl -lboost_regex -lboost_iostreams -lxerces-c; \
    ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
shapes.o: shapes.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 shapes.cpp
whorldata.o: whorldata.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 whorldata.cpp
surfacecol.o: surfacecol.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 surfacecol.cpp
surface.o: surface.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 surface.cpp
curveofile.o: curveofile.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 curveofile.cpp
shellbase.o: shellbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 shellbase.cpp
gaussian.o: gaussian.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 gaussian.cpp
porphyry.o: porphyry.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 porphyry.cpp
distbase.o: distbase.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 distbase.cpp
xml.o: xml.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 xml.cpp
whorl.o: whorl.cpp
	$(CC) $(CFLAGS) $(INC) -lreadline -lncurses -ltermcap -ldl -std=c++14 whorl.cpp
clean:
	rm -rf *o test
docs:
	$(DOXYGEN) Doxyfile
