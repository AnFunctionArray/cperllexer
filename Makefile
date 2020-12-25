CC=gcc
CPP=g++
CFLAGS=-I/usr/local/lib/perl5/5.32.0/x86_64-linux/CORE -g
CPPFLAGS=$(CFLAGS) -std=gnu++2a
LDFLAGS=-l:libpcre2-8.a -l:llvm-10/lib/libLLVM.so -L/usr/local/lib/perl5/5.32.0/x86_64-linux/CORE -lperl -lm -lcrypt
DEPS = main.h

%.o: %.c ./metaregex/%.c $(DEPS) ./llvmgen/llvmgen.h
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: ./metaregex/%.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)

%.o: ./llvmgen/%.cpp ./llvmgen/llvmgen.h
	$(CPP) -c -o $@ $< $(CPPFLAGS)

%.c: %.xs
	xsubpp $< > $@

cparser: main.o ./metaregex/parseregexfile.o ./metaregex/parseregexfilecpp.o namedcapture.o ./llvm/llvmgen.o perltoc.o
	$(CPP) -o $@ $^ $(LDFLAGS)

all: cparser