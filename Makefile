CC=clang-11
CXX=clang++-11
CFLAGS=-I$(HOME)/localperl/lib/5.32.0/x86_64-linux/CORE -g -O0 -I/usr/include/llvm-11 -I/usr/include/llvm-c-11
CXXFLAGS=$(CFLAGS) -std=c++20  -I/usr/include/llvm-11
LDFLAGS=-l:libpcre2-8.a $(shell llvm-config-11 --libs --ldflags) -L$(HOME)/localperl/lib/5.32.0/x86_64-linux/CORE -lperl -lm -lcrypt -lpthread
DEPS = main.h ./llvm/llvmgen.h

%.o: ./utility/%.cpp $(DEPS) 
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.o: ./llvm/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.c: %.xs
	xsubpp $< > $@

cparser: main.o ./utility/utility.o namedcapture.o ./llvm/llvmgen.o perltoc.o
	$(CXX) -o $@ $^ $(LDFLAGS)

all: cparser

clean: 
	rm main.o ./utility/utility.o namedcapture.o ./llvm/llvmgen.o perltoc.o cparser