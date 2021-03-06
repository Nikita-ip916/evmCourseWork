.PHONY: all runlab clean stlib course run sat sbt runsat runsbt
CXX = g++
CXXFLAGS = -Wall -Werror
CLIB = -std=c++11

MSC = -lmyscomp
MT = -lmyterm
BC = -lmybigchars
RC = -lmyreadkey
ASM = -lasm
BSC = -lbsc
MYLIBS = $(MSC) $(MT) $(BC) $(RC) $(ASM) $(BSC) -L lib

OUT = test.exe
COWRK = courseWork.exe
ASMOUT = sattest.exe
BSCOUT = sbttest.exe

CPP = src/libmyscomp.cpp src/libmyterm.cpp src/libmybigchars.cpp src/libmyreadkey.cpp
HEADERS = src/libmyscomp.hpp src/libmyterm.hpp src/libmybigchars.hpp src/libmyreadkey.hpp src/interface.hpp
LIBS = lib/libmyscomp.a lib/libmyterm.a lib/libmybigchars.a lib/libmyreadkey.a

all: bin/$(OUT)

bin/$(OUT): build/main.o
		$(CXX) $< -o $@
build/main.o: src/main5.cpp src/lab7.hpp
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)
runlab:
		./bin/$(OUT)


course: bin/$(COWRK)

bin/$(COWRK): build/mainCourseWork.o
		$(CXX) $< -o $@ $(MYLIBS)
build/mainCourseWork.o: src/mainCourseWork.cpp $(HEADERS) $(LIBS)
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)
run:
		./bin/$(COWRK)


sat: ./bin/$(ASMOUT)

./bin/$(ASMOUT): ./build/mainAsm.o ./lib/libasm.a
		$(CXX) $< -o $@ $(MYLIBS)
./build/mainAsm.o: ./src/mainAsm.cpp $(HEADERS) src/asm.hpp $(LIBS)
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)
runsat:
		./bin/$(ASMOUT)


sbt: ./bin/$(BSCOUT)

./bin/$(BSCOUT): ./build/mainBsc.o ./lib/libasm.a ./lib/libbsc.a
		$(CXX) $< -o $@ $(MYLIBS)
./build/mainBsc.o: ./src/mainBsc.cpp $(HEADERS) src/bsc.hpp $(LIBS)
		$(CXX) $(CXXFLAGS) -I src -c $< -o $@ $(CLIB)
runsbt:
		./bin/$(BSCOUT)


stlib: $(LIBS)

./lib/libasm.a: src/asm.cpp
		$(CXX) -g -I src -c $< -o build/asm.o
		ar rvs $@ ./build/asm.o
./lib/libbsc.a: src/bsc.cpp
		$(CXX) -g -I src -c $< -o build/bsc.o
		ar rvs $@ ./build/bsc.o
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
