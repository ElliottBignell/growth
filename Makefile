CXX=g++
CXXFLAGS=-c -Wall -Wno-unused-local-typedefs -Wno-deprecated-declarations
INC=-L/usr/share -L/usr/local/lib -L/usr/lib -I/usr/local/include -I/usr/include/boost -I/usr/lib/x86_64-linux-gnu/
DOXYGEN = doxygen
DEBUGFLAGS=-g -O0 -Wextra
OUT = bin

$(shell   mkdir -p $(OUT))

.PHONY: all

all: growth

growth: $(OUT)/curveofile.o $(OUT)/shellbase.o $(OUT)/gaussian.o $(OUT)/porphyry.o $(OUT)/distbase.o $(OUT)/xml.o $(OUT)/shapes.o $(OUT)/surface.o $(OUT)/surfacecol.o $(OUT)/stepper.o $(OUT)/whorl.o $(OUT)/whorldata.o
	$(CXX) $(INC) $(OUT)/shellbase.o $(OUT)/whorldata.o $(OUT)/whorl.o $(OUT)/surface.o $(OUT)/surfacecol.o $(OUT)/stepper.o $(OUT)/shapes.o $(OUT)/gaussian.o $(OUT)/porphyry.o $(OUT)/distbase.o $(OUT)/xml.o -o growth -lreadline -lcurses -ldl -lboost_regex -lboost_iostreams -lxerces-c
$(OUT)/shapes.o: shapes.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ shapes.cpp
$(OUT)/whorldata.o: whorldata.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ whorldata.cpp
$(OUT)/surfacecol.o: surfacecol.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ surfacecol.cpp
$(OUT)/surface.o: surface.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ surface.cpp
$(OUT)/stepper.o: stepper.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ stepper.cpp
$(OUT)/curveofile.o: curveofile.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ curveofile.cpp
$(OUT)/shellbase.o: shellbase.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ shellbase.cpp
$(OUT)/gaussian.o: gaussian.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ gaussian.cpp
$(OUT)/porphyry.o: porphyry.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ porphyry.cpp
$(OUT)/distbase.o: distbase.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ distbase.cpp
$(OUT)/xml.o: xml.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ xml.cpp
$(OUT)/whorl.o: whorl.cpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) $(INC) -lreadline -lncurses -ldl -fconcepts-ts -std=c++17 -o $@ whorl.cpp
clean:
	rm -rf *o bin/*o growth
docs:
	$(DOXYGEN) Doxyfile
