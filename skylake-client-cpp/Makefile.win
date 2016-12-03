# Project: client
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = CarState.o client.o SimpleDriver.o SimpleParser.o WrapperBaseDriver.o CarControl.o $(RES)
LINKOBJ  = CarState.o client.o SimpleDriver.o SimpleParser.o WrapperBaseDriver.o CarControl.o $(RES)
LIBS =  -L"C:/Programmi/Dev-Cpp/lib" WS2_32.Lib  
INCS =  -I"C:/Programmi/Dev-Cpp/include" 
CXXINCS =  -I"C:/Programmi/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"C:/Programmi/Dev-Cpp/include/c++/3.4.2/backward"  -I"C:/Programmi/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"C:/Programmi/Dev-Cpp/include/c++/3.4.2"  -I"C:/Programmi/Dev-Cpp/include" 
BIN  = client.exe
CXXFLAGS = $(CXXINCS)   -Wall
CFLAGS = $(INCS)   -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before client.exe all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "client.exe" $(LIBS)

CarState.o: CarState.cpp
	$(CPP) -c CarState.cpp -o CarState.o $(CXXFLAGS)

client.o: client.cpp
	$(CPP) -c client.cpp -o client.o $(CXXFLAGS)

SimpleDriver.o: SimpleDriver.cpp
	$(CPP) -c SimpleDriver.cpp -o SimpleDriver.o $(CXXFLAGS)

SimpleParser.o: SimpleParser.cpp
	$(CPP) -c SimpleParser.cpp -o SimpleParser.o $(CXXFLAGS)

WrapperBaseDriver.o: WrapperBaseDriver.cpp
	$(CPP) -c WrapperBaseDriver.cpp -o WrapperBaseDriver.o $(CXXFLAGS)

CarControl.o: CarControl.cpp
	$(CPP) -c CarControl.cpp -o CarControl.o $(CXXFLAGS)
