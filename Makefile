.PHONY: all runprog clean stlib course runcourse
CXX = g++
CXXFLAGS = -Wall -Werror
CLIB = -std=c++11

MSC = -lmyscomp
MT = -lmyterm
BC = -lmybigchars
RC = -lmyreadkey
MYLIBS = $(MSC) $(MT) $(BC) $(RC) -L lib

OUT = test.exe
COWRK = courseWork.exe

CPP = src/libmyscomp.cpp src/libmyterm.cpp src/libmybigchars.cpp src/libmyreadkey.cpp
HEADERS = src/libmyscomp.hpp src/libmyterm.hpp src/libmybigchars.hpp src/libmyreadkey.hpp
LIBS = lib/libmyscomp.a lib/libmyterm.a lib/libmybigchars.a lib/libmyreadkey.a

all: bin/$(OUT)

bin/$(OUT): build/main.o
		$(CXX) $< -o $@ $(MYLIBS)

build/main.o: src/main4.cpp $(HEADERS) $(LIBS)
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)

runprog:
		./bin/$(OUT)

course: bin/$(COWRK)

bin/$(COWRK): build/mainCourseWork.o
		$(CXX) $< -o $@ $(MYLIBS)

build/mainCourseWork.o: src/mainCourseWork.cpp $(HEADERS) $(LIBS)
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)

runcourse:
		./bin/$(COWRK)

stlib: $(LIBS)

lib/libmyscomp.a: src/libmyscomp.cpp
	$(CXX) -g -I src -c -o build/libmyscomp.o $<
	ar rcs $@ build/libmyscomp.o

lib/libmyterm.a: src/libmyterm.cpp
	$(CXX) -g -I src -c -o build/libmyterm.o $<
	ar rcs $@ build/libmyterm.o

lib/libmybigchars.a: src/libmybigchars.cpp
	$(CXX) -g -I src -c -o build/libmybigchars.o $<
	ar rcs $@ build/libmybigchars.o

lib/libmyreadkey.a: src/libmyreadkey.cpp
	$(CXX) -g -I src -c -o build/libmyreadkey.o $<
	ar rcs $@ build/libmyreadkey.o

clean:
		rm -rf bin/*.exe
		rm -rf build/*.o
