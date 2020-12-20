CC=gcc
CPP=g++
CFLAGS=
LDFLAGS=-l:libpcre2-8.a -l:llvm-10/lib/libLLVM.so
DEPS = main.h

%.o: %.c ./metaregex/%.c $(DEPS) ./llvmgen/llvmgen.h
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: ./metaregex/%.cpp
	$(CPP) -c -o $@ $< $(CFLAGS)

%.o: ./llvmgen/%.cpp ./llvmgen/llvmgen.h
	$(CPP) -c -o $@ $< $(CFLAGS)

cparser: main.o ./metaregex/parseregexfile.o ./metaregex/parseregexfilecpp.o namedcapture.o ./llvm/llvmgen.o
	$(CPP) -o $@ $^ $(LDFLAGS)

all: cparser