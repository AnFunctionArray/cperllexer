CC=clang
CXX=clang++
CFLAGS=-I$(HOME)/localperl/lib/5.35.6/darwin-thread-multi-2level/CORE -g -O0 -I/usr/local/include/llvm -I/usr/local/include/llvm-c -Wno-error=implicit-function-declaration
CXXFLAGS=$(CFLAGS) -std=c++2b -I$(HOME)/vcpkg/packages/range-v3_arm64-osx/include
LDFLAGS=$(shell llvm-config --libs --ldflags) -L$(HOME)/localperl/lib/5.35.6/darwin-thread-multi-2level/CORE -lperl -lm -lpthread -lz -lcurses
DEPS = src/main.h

%.o: ./llvm/%.cpp $(DEPS) 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.c: %.xs
	$(HOME)/localperl/bin/xsubpp $< > $@

cparser: src/main.o llvm/llvmgen.o src/perltoc.o
	$(CXX) -o $@ $^ $(LDFLAGS)

all: cparser

clean: 
	rm main.o src/main.o llvm/llvmgen.o src/perltoc.o src/perltoc.c cparser