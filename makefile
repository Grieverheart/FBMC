SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp, bin/%.o, $(SRC))
EXE=fbmcSimulation.dll

CXX=g++
CC=gcc
CXXFLAGS=-Wall -O3 -std=c++0x -D DLL_EXPORT
CFLAGS=-Wall -O3 -g -std=c99
LDFLAGS=-shared -Wl,--out-implib,libfbmcSimulation.a
RM=del /q

vpath %.o bin/

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all
all: $(EXE)
	@echo Done

$(EXE): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $@
	
.PHONY: clean
clean:
	-$(RM) bin\*
	@echo Clean Done!
