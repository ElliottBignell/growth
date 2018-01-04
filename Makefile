CXX=g++
CXXFLAGS=-c -Wall -Wno-unused-local-typedefs -Wno-deprecated-declarations
INC=-L/usr/share -L/usr/local/lib -L/usr/lib -I/usr/local/include -I/usr/include/boost -I/usr/lib/x86_64-linux-gnu/
DOXYGEN = doxygen
DEBUGFLAGS=-g -O0 -Wextra

all: test

test: curveofile.o shellbase.o gaussian.o porphyry.o distbase.o xml.o shapes.o surface.o surfacecol.o whorl.o whorldata.o
	$(CXX) $(INC) shellbase.o whorldata.o whorl.o surface.o surfacecol.o shapes.o gaussian.o porphyry.o distbase.o xml.o -o test -lreadline -lncurses -ldl -lboost_regex -lboost_iostreams -lxerces-c
shapes.o: shapes.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 shapes.cpp
whorldata.o: whorldata.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 whorldata.cpp
surfacecol.o: surfacecol.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 surfacecol.cpp
surface.o: surface.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 surface.cpp
curveofile.o: curveofile.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 curveofile.cpp
shellbase.o: shellbase.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 shellbase.cpp
gaussian.o: gaussian.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 gaussian.cpp
porphyry.o: porphyry.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 porphyry.cpp
distbase.o: distbase.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 distbase.cpp
xml.o: xml.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 xml.cpp
whorl.o: whorl.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -std=c++14 whorl.cpp
clean:
	rm -rf *o test
docs:
	$(DOXYGEN) Doxyfile
